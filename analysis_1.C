#include "day_analysis.h"
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <string>
#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>
#include <Math/ProbFuncMathCore.h>
#include <cmath>



// chose day of the year to analyse 
void day_hist( int month = 6, int day = 30,
            int year_from =1880, int year_to =2025,
            const char* file = "Climate_data_file.root") {

auto values = dailyMeansPerYear(file, month, day, year_from, year_to);


// if no value is put in then print "no data"
    if (values.empty()) {std::cout << "No data for that day\n";
     return; }



//HISTOGRAM


gStyle->SetOptStat(0);     
gStyle->SetOptTitle(1);    


auto histogram = new TH1D("temperature",
Form ("Average Temperature of %d/%d Every Year from Falsterbo;Temperature [#circC];Entries", day, month), 100, 5, 30); 
histogram->SetFillColor(kRed + 1);

   
for (double v : values) histogram->Fill(v);


double mean = histogram->GetMean(); //The mean of the distribution
double stdev = histogram->GetRMS(); //The standard deviation

std::cout << "Mean = " << mean << " Standard Deviation = " << stdev <<
 "\n";


auto canvas = new TCanvas();
histogram->Draw();

//guassian curve from mean and standard deviation
double binWidth = histogram->GetBinWidth(1);
double amp = histogram->GetEntries() * binWidth / (stdev * std::sqrt(2 * M_PI));

TF1* g = new TF1("g", "gaus", 5, 30);
g->SetParameters(amp, mean, stdev);
g->SetLineColor(kBlue);
g->SetLineWidth(3);
g->Draw("same");               


//legend
auto legend = new TLegend(0.55, 0.75, 0.88, 0.88);    
legend->AddEntry(histogram, Form("Temperature on %d/%d", day, month), "f");
legend->AddEntry(g, "Gaussian (sample mean, std dev)", "l");
legend->Draw();



//PROBABILITY METHODS 

//Empirical method 
double lo = 16, hi = 18;
int inRange = 0;
for (double v : values) if (v >= lo && v < hi) inRange++;
double p_emp = double(inRange) / values.size();

std::cout << "P_1(" << lo << " < T < " << hi << ") = " << p_emp << "\n";



//Guassian method 
double p_range = ROOT::Math::normal_cdf(hi, stdev, mean)
               - ROOT::Math::normal_cdf(lo, stdev, mean);
double p_above = 1.0 - ROOT::Math::normal_cdf(20, stdev, mean);


std::cout << "P_2(" << lo << " < T < " << hi << ") = " << p_range << "\n";
std::cout << "P(T > 20) = " << p_above << "\n";



}

