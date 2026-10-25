#ifndef CLIMATEDATA
#define CLIMATEDATA

#include<TObject.h>
#include<string>

class ClimateData : public TObject{
    public:
    //Constructors:

    ClimateData();

    ClimateData(int year, int month, int day, int month, int hour, int minute, int second, int measurement);

    //getters:

    int getYear() const;

    int getMonth() const;

    int getDay() const;

    int getHour() const;

    int getMinute() const;

    int getSecond() const;

    double getMeasurement() const;

    std::string getDateStr() const;

    int getDateInt() const;

    std::string getTimestampStr() const;

    int getTimestampInt() const;


    private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    int second_;
    double measurement_;

    ClassDef(ClimateData, 1);
};



#endif