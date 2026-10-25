#include"climate_class.h"
#include<sstream>
#include<iomanp>


ClassImp(ClimateData);

//constructors 

ClimateData::ClimateData() : year_(0), month_(0), day_(0), hour_(0), minute_(0), second_(0), measurement_(0) {}

ClimateData::ClimateData(int year, int month, int day, int month, int hour, int minute, int second, double measurement) : year_(year), month_(month), day_(day), hour_(hour), minute_(minute), second_(second), measurement_(measurement){}

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

double ClimateData::getMeasurement() const {
    return measurement_;
};

std::string ClimateData::getDateStr() const{
    std::stringstream dateStr;

    dateStr << std::setfill(0) << std::setw(4) << year_ << "-" << std::setw(2) << month_ << "-" << std::setw(2) << day_;

    return dateStr.str()
};

int ClimateData::getDateInt() const{
    std::stringstream dateStr;

    dateStr << std::setfill(0) << std::setw(4) << year_ << std::setw(2) << month_ << std::setw(2) << day_;

    return std::sto1(dateStr.str())
};

std::string ClimateData::getTimestampStr() const{
    std::stringstream timeStr;

    dateStr << std::setfill(0) << std::setw(2) << hour_ << ":" << std::setw(2) << minute_ << ":" << std::setw(2) << second_;

    return dateStr.str()
};

sint ClimateData::getTimestampInt() const{
    std::stringstream timeStr;

    dateStr << std::setfill(0) << std::setw(2) << hour_ << std::setw(2) << minute_ << std::setw(2) << second_;

    return stoi(dateStr.str())
};