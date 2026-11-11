#include "smhi_cleaner.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

constexpr double minTemperature = -45.0;
constexpr double maxTemperature = 35.0;
constexpr double maxRate = 3.0;
constexpr std::string_view bom = "\xEF\xBB\xBF";

const std::regex datePattern(R"([0-9]{4}-[0-9]{2}-[0-9]{2})");
const std::regex timePattern(R"([0-9]{2}:[0-9]{2}:[0-9]{2})");
const std::regex temperaturePattern(R"([+-]?(?:[0-9]+(?:\.[0-9]+)?|\.[0-9]+))");
const std::regex stationPattern(
    R"(smhi-opendata_1_[0-9]+_[0-9]{8}_[0-9]{6}_(.+?)(?:\([0-9]+\))?\.csv)",
    std::regex::icase);

struct Record {
    std::size_t lineIndex;
    std::optional<long long> timestamp;
    std::optional<double> temperature;
    std::vector<std::string_view> errors;
};

// The Python script parses one physical line at a time, using semicolons
// outside quoted fields and interpreting a doubled quote as a literal quote.
std::vector<std::string> readFields(std::string_view line)
{
    std::vector<std::string> fields;
    if (line.empty()) return fields;

    std::string field;
    bool quoted = false;
    bool atStart = true;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (quoted) {
            if (ch != '"') {
                field += ch;
            } else if (i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else {
                quoted = false;
            }
        } else if (ch == ';') {
            fields.push_back(field);
            field.clear();
            atStart = true;
        } else {
            if (atStart && ch == '"') quoted = true;
            else field += ch;
            atStart = false;
        }
    }
    fields.push_back(field);
    return fields;
}

std::string fieldAt(const std::vector<std::string>& fields, std::size_t index)
{
    return index < fields.size() ? fields[index] : "";
}

std::string_view withoutNewline(std::string_view line)
{
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
        line.remove_suffix(1);
    }
    return line;
}

std::vector<std::string_view> physicalLines(std::string_view data)
{
    if (data.substr(0, bom.size()) == bom) data.remove_prefix(bom.size());
    std::vector<std::string_view> lines;
    for (std::size_t begin = 0; begin < data.size();) {
        const auto ending = data.find_first_of("\r\n", begin);
        if (ending == std::string_view::npos) {
            lines.push_back(data.substr(begin));
            break;
        }
        auto end = ending + 1;
        if (data[ending] == '\r' && end < data.size() && data[end] == '\n') ++end;
        lines.push_back(data.substr(begin, end - begin));
        begin = end;
    }
    return lines;
}

bool isLeapYear(int year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

std::optional<long long> parseTimestamp(const std::string& date, const std::string& time)
{
    if (!std::regex_match(date, datePattern) || !std::regex_match(time, timePattern)) {
        return std::nullopt;
    }

    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));
    const int hour = std::stoi(time.substr(0, 2));
    const int minute = std::stoi(time.substr(3, 2));
    const int second = std::stoi(time.substr(6, 2));
    const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year == 0 || month < 1 || month > 12 || day < 1 ||
        hour > 23 || minute > 59 || second > 59) {
        return std::nullopt;
    }
    const bool leap = isLeapYear(year);
    if (day > monthDays[month - 1] + (month == 2 && leap)) return std::nullopt;

    // Count Gregorian days since 0001-01-01. All times are UTC.
    const int years = year - 1;
    int days = 365 * years + years / 4 - years / 100 + years / 400 + day - 1;
    for (int m = 1; m < month; ++m) days += monthDays[m - 1];
    if (leap && month > 2) ++days;
    return days * 86400LL + hour * 3600 + minute * 60 + second;
}

std::optional<double> parseTemperature(const std::string& value)
{
    if (!std::regex_match(value, temperaturePattern)) return std::nullopt;
    const double temperature = std::strtod(value.c_str(), nullptr);
    if (!std::isfinite(temperature)) return std::nullopt;
    return temperature;
}

std::vector<Record> inspect(const std::vector<std::string_view>& lines)
{
    std::vector<Record> records;
    std::size_t firstRecord = 0;
    std::size_t dateColumn = 0, timeColumn = 0, temperatureColumn = 0;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        const auto fields = readFields(withoutNewline(lines[i]));
        const auto date = std::find(fields.begin(), fields.end(), "Datum");
        const auto time = std::find(fields.begin(), fields.end(), "Tid (UTC)");
        const auto temperature = std::find(fields.begin(), fields.end(), "Lufttemperatur");
        if (date != fields.end() && time != fields.end() && temperature != fields.end()) {
            dateColumn = date - fields.begin();
            timeColumn = time - fields.begin();
            temperatureColumn = temperature - fields.begin();
            firstRecord = i + 1;
            break;
        }
    }
    if (firstRecord == 0) {
        throw std::runtime_error("SMHI header with Datum / Tid (UTC) / Lufttemperatur not found");
    }

    const auto requiredColumns = std::max({dateColumn, timeColumn, temperatureColumn}) + 1;
    for (std::size_t i = firstRecord; i < lines.size(); ++i) {
        const auto raw = withoutNewline(lines[i]);
        if (raw.find_first_not_of(" \t\v\f\r\n") == std::string_view::npos) continue;
        const auto fields = readFields(raw);
        if (fields.size() < requiredColumns && raw.find(';') == std::string_view::npos) {
            if (!records.empty()) break; // SMHI footer
            continue;
        }

        Record row{i, std::nullopt, parseTemperature(fieldAt(fields, temperatureColumn)), {}};
        if (fields.size() >= requiredColumns) {
            row.timestamp = parseTimestamp(fields[dateColumn], fields[timeColumn]);
        }
        if (!row.timestamp || !row.temperature) row.errors.push_back("INVALID_FORMAT");

        // Compare adjacent original records, even if the preceding one will be deleted.
        if (!records.empty()) {
            const auto& previous = records.back();
            if (row.timestamp && previous.timestamp) {
                const double hours = (*row.timestamp - *previous.timestamp) / 3600.0;
                if (hours <= 0) {
                    row.errors.push_back("TIME_ORDER");
                } else if (row.temperature && previous.temperature &&
                           std::abs((*row.temperature - *previous.temperature) / hours) > maxRate) {
                    row.errors.push_back("TEMPERATURE_RATE");
                }
            }
        }
        if (row.temperature && (*row.temperature < minTemperature ||
                                *row.temperature > maxTemperature)) {
            row.errors.push_back("TEMPERATURE_RANGE");
        }
        records.push_back(row);
    }
    if (records.empty()) throw std::runtime_error("No measurement records found");
    return records;
}

void writeCsvField(std::ostream& output, std::string_view field)
{
    if (field.find_first_of(",\"\r\n") == std::string_view::npos) {
        output << field;
        return;
    }
    output << '"';
    for (char ch : field) {
        if (ch == '"') output << '"';
        output << ch;
    }
    output << '"';
}

void writeStation(const fs::path& source, const fs::path& cleanPath, const fs::path& logPath)
{
    std::ifstream input(source, std::ios::binary);
    if (!input) throw std::runtime_error("Cannot open input");
    const std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const auto lines = physicalLines(data);
    const auto records = inspect(lines);

    std::ofstream cleaned(cleanPath, std::ios::binary);
    std::ofstream removed(logPath, std::ios::binary);
    cleaned.exceptions(std::ios::badbit | std::ios::failbit);
    removed.exceptions(std::ios::badbit | std::ios::failbit);
    cleaned << bom;
    removed << bom << "csv_line,date,time_utc,temperature,errors,original_record\r\n";
    std::size_t nextLine = 0;
    for (const auto& row : records) {
        if (row.errors.empty()) continue;
        while (nextLine < row.lineIndex) cleaned << lines[nextLine++];
        ++nextLine;

        const auto raw = withoutNewline(lines[row.lineIndex]);
        const auto fields = readFields(raw);
        removed << row.lineIndex + 1;
        for (std::size_t i = 0; i < 3; ++i) {
            removed << ',';
            writeCsvField(removed, fieldAt(fields, i));
        }
        removed << ',';
        for (std::size_t i = 0; i < row.errors.size(); ++i) {
            if (i > 0) removed << " | ";
            removed << row.errors[i];
        }
        removed << ',';
        writeCsvField(removed, raw);
        removed << "\r\n";
    }
    while (nextLine < lines.size()) cleaned << lines[nextLine++];
    cleaned.close();
    removed.close();
}

bool isWithin(const fs::path& path, const fs::path& directory)
{
    const auto relative = path.lexically_relative(directory);
    return !relative.empty() && *relative.begin() != "..";
}

}  // namespace

void cleanSmhiDirectory(const fs::path& inputDirectory, const fs::path& outputDirectory)
{
    if (!fs::is_directory(inputDirectory)) {
        throw std::runtime_error("Input directory not found: " + inputDirectory.u8string());
    }
    const auto inputRoot = fs::canonical(inputDirectory);
    const auto outputRoot = fs::weakly_canonical(outputDirectory);
    if (isWithin(outputRoot, inputRoot)) {
        throw std::runtime_error("Choose an output directory outside the raw-data directory");
    }

    std::map<std::string, fs::path> stations;
    for (const auto& entry : fs::recursive_directory_iterator(inputRoot)) {
        if (entry.is_symlink() || !entry.is_regular_file()) continue;
        const auto name = entry.path().filename().u8string();
        std::smatch match;
        if (!std::regex_match(name, match, stationPattern)) continue;
        const auto station = match[1].str();
        const auto [previous, inserted] = stations.emplace(station, entry.path());
        if (!inserted) {
            throw std::runtime_error("Multiple inputs for station " + station + ": " +
                                     previous->second.u8string() + " and " + entry.path().u8string());
        }
    }
    if (stations.empty()) {
        throw std::runtime_error("No SMHI station CSVs found in " + inputRoot.u8string() +
                                 ". Extract datasets/datasets.tgz first if using the bundled data.");
    }

    const auto cleanDirectory = outputRoot / "dataset_clean";
    const auto logDirectory = outputRoot / "removed_records";
    if (fs::is_symlink(cleanDirectory) || fs::is_symlink(logDirectory)) {
        throw std::runtime_error("Output subdirectories must not be symbolic links");
    }
    if (isWithin(cleanDirectory, inputRoot) || isWithin(logDirectory, inputRoot)) {
        throw std::runtime_error("Output subdirectories must be outside the raw-data directory");
    }
    fs::create_directories(cleanDirectory);
    fs::create_directories(logDirectory);

    // Stage results in a fresh directory so writing never follows an existing
    // output file back to an input, including through a hard link.
    const auto staging = outputRoot / ".smhi_cleaner_staging";
    if (isWithin(staging, inputRoot)) {
        throw std::runtime_error("Staging directory must be outside the raw-data directory");
    }
    if (!fs::create_directory(staging)) {
        throw std::runtime_error("Staging directory already exists: " + staging.u8string());
    }
    try {
        for (const auto& [station, source] : stations) {
            const auto cleanName = fs::u8path(station + "_Clean.csv");
            const auto logName = fs::u8path(station + "_removed_records.csv");
            try {
                writeStation(source, staging / cleanName, staging / logName);
            } catch (const std::exception& error) {
                throw std::runtime_error(source.u8string() + ": " + error.what());
            }
        }
        for (const auto& stationEntry : stations) {
            const auto& station = stationEntry.first;
            const auto cleanName = fs::u8path(station + "_Clean.csv");
            const auto logName = fs::u8path(station + "_removed_records.csv");
            fs::rename(staging / cleanName, cleanDirectory / cleanName);
            fs::rename(staging / logName, logDirectory / logName);
        }
    } catch (...) {
        for (const auto& entry : fs::directory_iterator(staging)) {
            std::error_code ignored;
            fs::remove(entry.path(), ignored);
        }
        std::error_code ignored;
        fs::remove(staging, ignored);
        throw;
    }
    fs::remove(staging);
}
