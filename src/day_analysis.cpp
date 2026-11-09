#include "day_analysis.h"
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>
#include <Math/ProbFuncMathCore.h>
#include <cmath>
#include <iostream>
#include <TEntryList.h>
#include <TDirectory.h>
#include <TString.h>


std::vector<double> dailyMeansPerYear(TTree* tree, const char* station,
                                      int month, int day,
                                      int year_from, int year_to) {



    

int year_, month_, day_;
double measurement_;
tree->SetBranchAddress("year_",        &year_);
tree->SetBranchAddress("month_",       &month_);
tree->SetBranchAddress("day_",         &day_);
tree->SetBranchAddress("measurement_", &measurement_);


//one date per year - sum and count of measurements
int nYears = year_to - year_from + 1;
std::vector<double> sum(nYears, 0.0);
std::vector<int> count(nYears, 0);


TString cut = Form("stationName_ == \"%s\" && month_ == %d && day_ == %d",
                   station, month, day);


tree->Draw(">>elist", cut, "entrylist");

TEntryList* elist = (TEntryList*)gDirectory->Get("elist");

//loop
for (Long64_t j = 0; j < elist->GetN(); ++j) {
    tree->GetEntry(elist->GetEntry(j));
    if (year_ < year_from || year_ > year_to) continue;
    sum[year_ - year_from] += measurement_;
    count[year_ - year_from]++;
}

    
//average of one day per year
std::vector<double> values;
for (int k=0; k < nYears; ++k)
        if (count[k] >0) values.push_back(sum[k]/count[k]);

return values;
                                         }

void DayHistogram(TTree* tree, const char* station,
                  int month, int day, int year_from, int year_to) {
    auto values = dailyMeansPerYear(tree, station, month, day, year_from, year_to);
    if (values.empty()) { std::cout << "No data for that day\n"; return; }



//HISTOGRAM


gStyle->SetOptStat(0);     
gStyle->SetOptTitle(1);    


auto histogram = new TH1D("temperature",
Form ("Average Temperature of %d/%d Every Year from Falsterbo;Temperature [#circC];Entries", day, month, station), 100, 5, 30); 
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


    

     

