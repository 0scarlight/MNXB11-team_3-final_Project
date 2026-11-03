
#include <iostream>
#include "climate_class.h"
#include "file_processing.h"
#include<vector>
#include<string>


void createTTree() {

std::vector<std::string> filelist = {
"scripts/data_cleaning/dataset_clean/Borås_Clean.csv",
"scripts/data_cleaning/dataset_clean/Falsterbo_Clean.csv",
"scripts/data_cleaning/dataset_clean/Falun-Lugnet_Clean.csv",
"scripts/data_cleaning/dataset_clean/Halmstad_flygplats_Clean.csv",
"scripts/data_cleaning/dataset_clean/Karlstad_Flygplats_Clean.csv",
"scripts/data_cleaning/dataset_clean/Luleå-Kallax_Flygplats_Clean.csv",
"scripts/data_cleaning/dataset_clean/Lund_Clean.csv",
"scripts/data_cleaning/dataset_clean/Söderarm_A_Clean.csv",
"scripts/data_cleaning/dataset_clean/Umeå_Flygplats_Clean.csv",
"scripts/data_cleaning/dataset_clean/Uppsala_Flygplats_Clean.csv",
"scripts/data_cleaning/dataset_clean/Visby_Flygplats_Clean.csv",
};


csvToTree(filelist);

}
