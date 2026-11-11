#include "lund_cold_days.h"

#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>


namespace {

// ============================================================
// 1. Data types
// ============================================================

struct Reading {
    int year;
    int date;       // Month and day, e.g. 105 for January 5
    double temp;    // Temperature in degrees Celsius
};

struct YearCount {
    int year;
    int coldDays;   // Days with a mean temperature below zero
};


// ============================================================
// 2. Read the Lund measurements
// ============================================================

std::vector<Reading> readLund(TTree& tree)
{
    std::vector<Reading> data;
    TTreeReader reader(&tree);  // Read one entry at a time

    // Branch names from the ROOT file
    TTreeReaderValue<std::string> station(reader, "stationName_");
    TTreeReaderValue<int> year(reader, "year_");
    TTreeReaderValue<int> month(reader, "month_");
    TTreeReaderValue<int> day(reader, "day_");
    TTreeReaderValue<double> temperature(reader, "measurement_");

    while (reader.Next()) {
        if (*station != "Lund") continue;

        // Keep month and day together for date comparisons
        int date = (*month) * 100 + (*day);
        data.push_back({*year, date, *temperature});
    }

    return data;
}


// ============================================================
// 3. Count cold days in each year
// ============================================================

std::vector<YearCount> countColdDays(const std::vector<Reading>& data)
{
    std::vector<YearCount> result;
    if (data.empty()) return result;

    // The measurements are in date order
    const int firstYear = data.front().year;
    const int lastYear = data.back().year;
    std::size_t i = 0;  // Keep reading from here when the year changes

    for (int currentYear = firstYear; currentYear <= lastYear; ++currentYear) {
        if (i == data.size()) break;
        if (data[i].year != currentYear) continue;  // No readings this year

        int coldDays = 0;
        int daySum = 0;

        while (i < data.size() && data[i].year == currentYear) {
            int currentDate = data[i].date;

            // Sum in tenths of a degree so opposite readings cancel exactly
            daySum += std::lround(data[i].temp * 10);
            ++i;

            // A new date, a new year, or the end of the data
            if (i == data.size() || data[i].year != currentYear
                                 || data[i].date != currentDate) {
                // The daily sum and the daily mean have the same sign
                if (daySum < 0) ++coldDays;
                daySum = 0;
            }
        }

        result.push_back({currentYear, coldDays});
    }

    return result;
}


// ============================================================
// 4. Plot the yearly counts and fit a straight line
// ============================================================

void plotResults(const std::vector<YearCount>& result)
{
    TCanvas canvas("lundColdDaysCanvas", "Cold days in Lund");
    TGraph graph;

    for (const YearCount& row : result) {
        // Add the next point: year on x, cold days on y
        graph.SetPoint(graph.GetN(), row.year, row.coldDays);
    }

    // Plot title, x-axis title, y-axis title
    graph.SetTitle("Days with mean temperature below 0 #circC in Lund;Year;Days per year");
    graph.Draw("A*");  // Draw axes and points

    // Fit y = a + bx; Q keeps the fit output quiet
    if (graph.Fit("pol1", "Q") != 0) {
        std::cerr << "Linear fit failed.\n";
        return;
    }

    TF1* line = graph.GetFunction("pol1");
    double daysPerDecade = 10 * line->GetParameter(1);  // Annual slope times ten

    TLegend legend(0.52, 0.80, 0.89, 0.89);
    legend.AddEntry(line, Form("Linear fit: %.2f days per decade", daysPerDecade), "l");
    legend.Draw();

    std::cout << "Linear trend: " << daysPerDecade << " days per decade\n";
    canvas.SaveAs("lund_cold_days.png");
    canvas.SaveAs("lund_cold_days.pdf");
}

}  // namespace

void PlotLundColdDays(TTree* tree)
{
    const auto data = readLund(*tree);
    const auto result = countColdDays(data);

    if (result.size() < 2) {
        std::cerr << "Need at least two years for the Lund linear fit.\n";
        return;
    }

    plotResults(result);
}
