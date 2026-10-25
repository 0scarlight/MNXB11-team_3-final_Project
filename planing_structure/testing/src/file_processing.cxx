#include<iostream>
#include<sting>
#include<fstream>
#include<sstream>
#include"file_preprocessing.h"

void csvCleaner(const std::string filename, const std::string name){


    std::ifstream input(filename);
    std::ofstream output(name + "_cleaned.csv");

    std::string line;

    //not needed for this project, skips header in input file and writes header in outputfile
    //std::getline(input,line);
    //output << "Year , Month, Day , Hour , Minute , Second , Measurement\n"

    //line format YYYY-MM-DD;HH:MM:SS;measurement;quality 
    while (std::getline(input,line)) {
        //check for correct line
        if (line.find(";G") == line.npos ||
            line.find(";Y") == line.npos) {
                continue;
            };
        std::stringstream working_line(line)

        //extract the data present in the  file into their own strings
        std::string date, timestamp, measurement; 
        std::getline(line, date, ";");
        std::getline(line, timestamp, ";");
        std::getline(line, measurement, ";");

        //extract individual strings for each subcomponent of the date
        std::string year, month, dat;
        std::getline(date, year, "-");
        std::getline(date, month, "-");
        std::getline(date, day, "-");

        //extract individual strings for each subcomponent of the timestamp
        std::string hour, minute, second;
        std::getline(timestamp, hour, ":");
        std::getline(timestamp, minute, ":");
        std::getline(timestamp, second, ":");

        output << std::stoi(year) << "," 
            << std::stoi(month) << "," 
            << std::stoi(day) << "," 
            << std::stoi(hour) << "," 
            << std::stoi(minute) << "," 
            << std::stoi(second) << "," 
            << std::stod(measurement) << "," 
            << "\n"
    }

    input.close()
    output.close()
    return;
};





