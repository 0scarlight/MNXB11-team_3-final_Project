
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>
#include <Math/ProbFuncMathCore.h>
#include <cmath>



 


// chose day of the year to analyse 
void day_hist( int month = 6, int day = 30,
            int year_from =1880, int year_to =2025,
            const char* file = "Climate_data_file.root") {

            
            
// DO FROM BEFORE 1997 - JUST LESS READINGS FROM BEFORE THEN 



TFile* f = TFile::Open(file);

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'tree_file.root'." << std::endl;
        return;
    }


 // Get the TTree from the file
    TTree* tree = (TTree*)f->Get("tree");
    if (!tree) {
        std::cerr << "Error: Could not find tree 'tree' in file." << std::endl;
        f->Close();
        return;
    }

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


// loop over the tree once
Long64_t nEntries = tree->GetEntries();
for (Long64_t i = 0; i< nEntries; ++i){
    tree->GetEntry(i);




if (year_ < year_from || year_ > year_to) continue;
if (month_ !=month || day_ != day) continue;

sum [year_ - year_from] += measurement_;
count [year_ - year_from] ++;


}

    
//average of one day per year
std::vector<double> values;
for (int k=0; k < nYears; ++k)
        if (count[k] >0) values.push_back(sum[k]/count[k]);

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

