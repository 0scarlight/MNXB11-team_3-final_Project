#include<iostream>
#include<sting>
#include<fstream>
#include<sstream>
#include"file_processing.h"

#include"climate_class.h"
#include<TFile.h>
#include<TTree.h>
#include<TObject>

void csvToTree(const std::vector<string> inputList, const std::string startTime, const std::string endTime){

ClimateData *object = nullptr;

TFile *F = new TFile("Climate_data" + "_file.root", "RECREATE")

TTree *T = new TTree("tree" , "Climate data")

//using splitlevel to autocreate branches for each member variable of climateData
T->Branch("Climate data", &object, 32000, 1)


for (std::string inputName : inputList){

    std::ifstream input(inputName);

    std::string stationName = inputName.substr(0, inputName.find("_"))

    std::string line;

    //line format YYYY-MM-DD;HH:MM:SS;measurement;quality 
    while (std::getline(input,line)) {
        //check for correct line
        if (line.find(";G") == line.npos &&
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
        
        object = new ClimateData(std::stoi(year), std::stoi(month), std::stoi(day), std::stoi(hour), std::stoi(minute), std::stoi(second) std::stod(measurement), stationName);
        
        T->Fill()
        delete object
        object = nullptr;
    };

    input.close()
};

T->Write()
F->Close()
    return;
};


