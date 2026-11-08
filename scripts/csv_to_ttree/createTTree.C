
#include <iostream>
#include "climate_class.h"
#include "file_processing.h"
#include<vector>
#include<string>
#include<fstream>

void createTTree(std::string fileNames) {

std::ifstream input(fileNames);

std::vector<std::string> fileList;
std::string line;

while (std::getline(input,line)){
    fileList.push_back(line);
};

csvToTree(fileList);

}
