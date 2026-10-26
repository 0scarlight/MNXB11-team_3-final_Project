#ifndef CSVPARSING
#define CSVPARSING
#include<string>

//Removes unecessary data  
void csvCleaner(const std::string inputName, const std::string outputName);

void csvTotree (const std::string inputName, const std::string startTime, const std::string endTime); 
#endif