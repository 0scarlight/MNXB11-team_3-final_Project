#ifndef CLIMATEDATA
#define CLIMATEDATA

#include<TObject.h>
#include<string>
#include<cstdint>


class ClimateData : public TObject{
    public:
    //Constructors:

    ClimateData();

    ClimateData(int year, int month, int day, int hour, int minute, int second, double measurement, std::string stationName);

    //getters:

    int getYear() const;

    int getMonth() const;

    int getDay() const;

    int getHour() const;

    int getMinute() const;

    int getSecond() const;

    //std::int64_t getUnixTime() const;

    double getMeasurement() const;
    
    std::string getStationName() const;

    std::string getDateStr() const;

    int getDateInt() const;

    std::string getTimestampStr() const;

    int getTimestampInt() const;


    //setters

    void setData(int year, int month, int day, int hour, int minute, int second, double measurement, std::string stationName);

    private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    int second_;
    //std::int64_t unixTime_;
    double measurement_;
    std::string stationName_;

    ClassDef(ClimateData, 1);
};



#endif