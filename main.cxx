#include "daily_means.h"
/// #include "insert task 3.1 header file here"
/// #include "insert own task header file here"
#include <iostream>
#include <TFile.h>
#include <TTree.h>

int main(){
  
  /// open our file
  TFile* file = TFile::Open("Climate_data_file.root");

  /// print if file does not exist 
  if (!file || file->IsZombie()) {
        std::cout << "Error: Could not open file!" << std::endl;
        return 1;
  }

  /// get the tree
  TTree* tree = nullptr;
  file->GetObject("tree", tree);

  PlotDailyMean(tree);
  ///call function for task 3.1 here
  ///call function for our made up task here

  file->Close();
  delete file;
  return 0;
}