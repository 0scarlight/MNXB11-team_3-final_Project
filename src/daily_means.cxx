
#include "daily_means.h"

#include <TTree.h>
#include <TString.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TMath.h>

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <limits>
#include <cmath>

std::pair<std::vector<double>, std::vector<double>>
CalculateDailyMean(TTree* tree, std::string station)
{
    /// check if tree exists
    if (!tree) {
        std::cout << "Error: Tree not found!" << std::endl;
        return {};
    }

    /// get number of entries for station
    Long64_t nEntries = tree->Draw(
        "measurement_:day_:month_:year_",
        Form("stationName_ == \"%s\"", station.c_str()),
        "goff"
    );

    /// get the stored datapoints
    double* temperatures = tree->GetV1();
    double* days = tree->GetV2();
    double* months = tree->GetV3();
    double* years = tree->GetV4();

    /// print if no entries for station
    if (nEntries == 0) {
        std::cout << "No measurements found for " << station << std::endl;
        return {};
    }

    /// find the first and last year in the selected data
    int min_year = static_cast<int>(years[0]);
    int max_year = static_cast<int>(years[nEntries-1]);

    /// days in a non-leap year
    const int days_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    /// days in a year and years in the data
    const int days_per_year = 365;
    const int number_of_years = max_year - min_year + 1;

    /// placeholder for temperature sums and number of measurements
    /// for every year and day
    std::vector<std::vector<double>> sums(
        number_of_years, std::vector<double>(days_per_year, 0.0)
    );
    std::vector<std::vector<int>> counts(
        number_of_years, std::vector<int>(days_per_year, 0)
    );

    /// iterate through every measurement
    for (Long64_t i = 0; i < nEntries; i++) {

        int year = static_cast<int>(years[i]);
        int month = static_cast<int>(months[i]);
        int day = static_cast<int>(days[i]);

        double temperature = temperatures[i];

        /// ignore February 29 
        if (month == 2 && day == 29) {
            continue;
        }

        /// calculate day-of-year index (0 to 364).
        int dayIndex = day - 1;

        for (int m = 0; m < month - 1; m++) {
            dayIndex += days_month[m];
        }

        /// find the year index
        int yearIndex = year - min_year;

        /// add the measurement to sums and advance counts
        sums[yearIndex][dayIndex] += temperature;
        counts[yearIndex][dayIndex]++;
    }

    /// placeholders for climatological mean and standard deviation
    std::vector<double> dailyMeans;
    std::vector<double> dailySD;

    for (int dayIndex = 0; dayIndex < days_per_year; dayIndex++) {

        std::vector<double> yearlyMeans;

        /// find mean for specific calendar day in each year.
        for (int yearIndex = 0; yearIndex < number_of_years; yearIndex++) {

            /// if given day has measurements, calculate mean
            if (counts[yearIndex][dayIndex] > 0) {

                double yearlyMean =
                    sums[yearIndex][dayIndex] / counts[yearIndex][dayIndex];

                yearlyMeans.push_back(yearlyMean);
            }
        }

        /// placeholder for the mean of the yearly means
        double sum_final = 0.0;

        /// find sum of temperatures on a given day across all years
        for (double mean : yearlyMeans) {
            sum_final += mean;
        }

        /// find average of those temperatures
        double finalMean = sum_final / yearlyMeans.size();

        /// SD placeholder
        double standardDeviation = 0.0;

        /// use TMath::RMS to calculate standard deviation for each day
        standardDeviation = TMath::RMS(
            yearlyMeans.begin(), yearlyMeans.end()
        );
        
        /// add final results to placeholder vectors
        dailyMeans.push_back(finalMean);
        dailySD.push_back(standardDeviation);
    }

    return {dailyMeans, dailySD};
}

/// plotting the results
void PlotDailyMean(TTree* tree)
{
    auto results = CalculateDailyMean(tree, "Falsterbo");

    auto mean = results.first;
    auto SD = results.second;

    /// check the size 
    if (mean.size() != 365 || SD.size() != 365) {
        std::cout << "Could not create histogram: expected 365 days of data."
                  << std::endl;
        return;
    }

    /// create histogram
    TH1D* h1 = new TH1D(
        "hist",
        "Mean temperature on each day of the year;Day of year;Temperature [#circC]",
        365, 0.5, 365.5
    );

    h1->SetDirectory(nullptr);

    /// set contents of each bin
    for (int bin = 1; bin <= 365; bin++) {
        h1->SetBinContent(bin, mean[bin - 1]);
        h1->SetBinError(bin, SD[bin - 1]);
    }
    
    /// create canvas
    TCanvas* c1 = new TCanvas("canvas", "Daily temperatures Falsterbo");

    h1->Draw("E1");
    h1->GetXaxis()->CenterTitle(true);
    h1->GetYaxis()->CenterTitle(true);

    c1->SaveAs("daily_mean_over_years.pdf");

    /// cleanup
    delete c1;
    delete h1;
}
