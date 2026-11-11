#include "daily_means.h"
#include "day_analysis.h"
#include "lund_cold_days.h"

#include <TFile.h>
#include <TTree.h>

#include <iostream>

int main()
{
    TFile file("Climate_data_file.root");
    if (file.IsZombie()) {
        std::cerr << "Error: Could not open Climate_data_file.root.\n";
        return 1;
    }

    TTree* tree = nullptr;
    file.GetObject("tree", tree);
    if (!tree) {
        std::cerr << "Error: Could not read the temperature tree.\n";
        return 1;
    }

    PlotDailyMean(tree);
    PlotLundColdDays(tree);
    DayHistogram(tree, "Falsterbo", 6, 30);
    return 0;
}
