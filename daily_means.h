#pragma once
#include <vector>

class TTree;
class TH1D;

std::vector<double> CalculateDailyMean(TTree* tree);
std::vector<double> CalculateDailySD(Tree* tree);

TH1D* MakeHistogram(
    const std::vector<double>& means,
    const std::vecotr<double>& SDs,
)

void RunAnalysis(TTree* tree);

void 