#include "day_analysis.h"
#include <TFile.h>
#include <TTree.h>
#include <iostream>


std::vector<double> dailyMeansPerYear(const char* file, int month, int day,
     int year_from, int year_to) {



        TFile* f = TFile::Open(file);

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'tree_file.root'." << std::endl;
        return{};
    }


 // Get the TTree from the file
    TTree* tree = (TTree*)f->Get("tree");
    if (!tree) {
        std::cerr << "Error: Could not find tree 'tree' in file." << std::endl;
        f->Close();
        return{};
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

return values;
     }

