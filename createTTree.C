
#include <iostream>
#include "climate_class.h"
#include "file_processing.h"
#include<vector>
#include<string>


void createTTree() {

std::vector<std::string> filelist = {"datasets/Falsterbo_Clean.csv"};

csvToTree(filelist);

}
