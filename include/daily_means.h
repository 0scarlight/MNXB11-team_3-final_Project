#pragma once
#include <vector>
#include <string>
#include <utility>

class TTree;
class TH1D;

std::pair<std::vector<double>, std::vector<double>>
CalculateDailyMean(TTree* tree, std::string station);

void PlotDailyMean(TTree* tree);