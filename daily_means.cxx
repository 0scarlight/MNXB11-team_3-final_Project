#include "daily_means.h"

#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TFile.h>

#include <iostream>
#include <vector>

std::vector<double> CalculateDailyMean(TTree* tree, std::string station) 
{
if (!tree) {
    std::cout << "Error: Tree not found!" << std::endl;
    return {}; }

std::vector<double> dailyMeans;

int min_year = tree->GetMinimum("year_");
int max_year = tree->GetMaximum("year_");

int days_month[] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

for (int month = 1; int month <= 12, month++){
    month.
    for (int day = 1; day <= days_month[month -1]; day++){
        std::vector<double> yearlyMeans;
        for (int year = min_year; year <= max_year; ++year){
            Long64_t n = tree->Draw("measurement_", 
                "year_ == year && month_ == month && day_ == day" && stationname_ == station,
                "goff");
                double* temperatures = tree->GetV1();

}
}
}
}


std::vector<double> CalculateDailySD(TTree* tree)
{

}