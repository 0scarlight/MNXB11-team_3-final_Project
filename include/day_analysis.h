#ifndef DAY_ANALYSIS_H
#define DAY_ANALYSIS_H

#include <vector>

class TTree;

std::vector<double> dailyMeansPerYear(TTree* tree, const char* station,
                                      int month, int day,
                                      int year_from, int year_to);

void DayHistogram(TTree* tree, const char* station,
                  int month = 6, int day = 30,
                  int year_from = 1880, int year_to = 2025);


#endif


