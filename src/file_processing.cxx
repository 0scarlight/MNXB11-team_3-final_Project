#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include"file_processing.h"

#include"climate_class.h"
#include<TFile.h>
#include<TTree.h>
#include<TObject.h>
#include<iostream>

void csvToTree(const std::vector<string> inputList){

ClimateData *object = nullptr;

TFile *F = new TFile("Climate_data_file.root", "RECREATE");

 if (!F || F->IsZombie()) {
        std::cerr << "Could not create ROOT file\n";
        delete object;
        delete F;
        return;
    };

TTree *T = new TTree("tree" , "Climate data");

T->Branch("Climate data", &object);

for (std::string inputName : inputList){

    std::ifstream input(inputName);

    if (!input.is_open()){
        std::cerr << "Could not open file " << inputName << std::endl;
        continue;

    };


    std::string stationName = inputName.substr(0, inputName.find("_"));

    std::string line;

    object = new ClimateData();

    //line format YYYY-MM-DD;HH:MM:SS;measurement;quality 
    while (std::getline(input,line)) {
        //check for correct line
        if (line.find(";G") == line.npos &&
            line.find(";Y") == line.npos) {
                continue;
            };
        std::stringstream working_line(line);

        //extract the data present in the  file into their own strings
        std::string date, timestamp, measurement; 
        std::getline(working_line, date, ';');
        std::getline(working_line, timestamp, ';');
        std::getline(working_line, measurement, ';');
        
        //extract individual strings for each subcomponent of the date
        std::stringstream dateStream(date);
        std::string year, month, day;
        std::getline(dateStream, year, '-');
        std::getline(dateStream, month, '-');
        std::getline(dateStream, day, '-');

        //extract individual strings for each subcomponent of the timestamp
        std::stringstream timeStream(timestamp);
        std::string hour, minute, second;
        std::getline(timeStream, hour, ':');
        std::getline(timeStream, minute, ':');
        std::getline(timeStream, second, ':');
        
        object->setData(std::stoi(year), std::stoi(month), std::stoi(day), std::stoi(hour), std::stoi(minute), std::stoi(second), std::stod(measurement), stationName);
        
        T->Fill();
        //T->Print();
        
        ;
    };

    delete object;
    object = nullptr;
    input.close();
};

F->cd();
T->Write();
std::cout << "Entries: " << T->GetEntries() << '\n';
F->Close();
delete F;
    return;
};


