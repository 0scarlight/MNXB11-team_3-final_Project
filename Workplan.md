# Workplan for MNXB11 project

Team number: 3
Team members: Rebecca Batchelor, Zuzanna Popek, Jie Qi, Oscar Tang 

# Plan

## 1 File preprocessing - sorting and handling the dataset
The initial step to having workable data and should at the very least:
- Sort and remove any immediately bad data
- Reformat the dataset to a standard CSV file 

## 1.2 File processing - preparing the preprocessed file for data analysis 
This the the final part of the file processing and should at the very least:
- Make a class for later data analysis
- Convert the data from the cleaned .csv files to a .root file
  - Should be able to handle multiple datasets at once
- Make a function to verify if two datasets have matching entries 

## 2.1 Data analysis 
This is where the data is actually utilised and should at the very least:
- Select the correct branches to plot from
- Apply any necessary operations to the data in the TTree


## 2.2 Representing the data  
This is where the results are presented and should at the very least:
- Plot the data in plots
  - Correct Axises
  - Correct Units
  - Correct Title
- Save the plots