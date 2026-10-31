#include"climate_class.h"
#include<sstream>
#include<iomanip>
#include<chrono>

ClassImp(ClimateData);

//constructors 

ClimateData::ClimateData() : year_(0), month_(0), day_(0), hour_(0), minute_(0), second_(0), measurement_(0) {}

ClimateData::ClimateData(
    int year, 
    int month, 
    int day, 
    int hour, 
    int minute, 
    int second, 
    double measurement, 
    std::string stationName) 
    : year_(year), 
    month_(month), 
    day_(day), 
    hour_(hour), 
    minute_(minute), 
    second_(second), 
    measurement_(measurement), 
    stationName_(stationName)
    {   
        //Only works in C++ 20 and up, the ROOT installation uses C++ 17
        //auto ymd = std::chrono::year_month_day(year_/month_/day_);
        //auto date = sys_days(ymd);

        //auto time = date + std::chrono::hours{hour_} + std::chrono::minutes{minute_} + std::chrono::seconds{second_};
        //unixTime_ = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    }

//getters

int ClimateData::getYear() const {
    return year_;
};

int ClimateData::getMonth() const {
    return month_;
};

int ClimateData::getDay() const {
    return day_;
};

int ClimateData::getHour() const {
    return hour_;
};

int ClimateData::getMinute() const {
    return minute_;
};

int ClimateData::getSecond() const {
    return second_;
};

//std::int64_t ClimateData::getUnixTime() const {
//    return unixTime_;
//};

double ClimateData::getMeasurement() const {
    return measurement_;
};

std::string ClimateData::getStationName() const {
    return stationName_;
};

std::string ClimateData::getDateStr() const{
    std::stringstream dateStr;

    dateStr << std::setfill('0') << std::setw(4) << year_ << "-" << std::setw(2) << month_ << "-" << std::setw(2) << day_;

    return dateStr.str();
};

int ClimateData::getDateInt() const{
    std::stringstream dateStr;

    dateStr << std::setfill('0') << std::setw(4) << year_ << std::setw(2) << month_ << std::setw(2) << day_;

    return std::stoi(dateStr.str());
};

std::string ClimateData::getTimestampStr() const{
    std::stringstream timeStr;

    timeStr << std::setfill('0') << std::setw(2) << hour_ << ":" << std::setw(2) << minute_ << ":" << std::setw(2) << second_;

    return timeStr.str();
};

int ClimateData::getTimestampInt() const{
    std::stringstream timeStr;

    timeStr << std::setfill('0') << std::setw(2) << hour_ << std::setw(2) << minute_ << std::setw(2) << second_;

    return stoi(timeStr.str());
};
//setter

void ClimateData::setData(int year, int month, int day, int hour, int minute, int second, double measurement, std::string stationName) {
    year_ = year;
    month_ = month;
    day_ = day;
    hour_ = hour;
    minute_ = minute;
    second_ = second;
    measurement_ = measurement;
    stationName_ = stationName;

    //auto ymd = std::chrono::year_month_day(year_/month_/day_);
    //auto date = sys_days(ymd);

    //auto time = date + std::chrono::hours{hour_} + std::chrono::minutes{minute_} + std::chrono::seconds{second_};
    //unixTime_ = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    return;
};

