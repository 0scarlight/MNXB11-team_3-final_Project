# Introduction:
This repository contains and documents the a solution to the final project of the course MNXB11 given at LU made and implemented by Team 3. The given task was to extract, format, and analyze a data set to extract and represent the data using the ROOT framework.

# Project structure: 
The project has been split into two major parts, with each part subsequently being split into subtasks.  

## Proposed Project Workflow

![Proposed workflow for data preparation and analysis](planing_structure/project_workflow.svg)

## Data Handling 
The data handling is the part where the raw data gets vetted and converted into a more manageable format for the data analysis and consists of:

### Data Cleaning

The C++ batch cleaner runs four quality checks on the SMHI station data. From the repository root in the MNXB11 container, extract the archive if needed, then build and run the cleaner:

```bash
tar -xzf datasets/datasets.tgz -C datasets
make smhi_cleaner
./smhi_cleaner
```

The cleaner reads station CSVs from `datasets/` and writes cleaned data to `results/data_cleaning/dataset_clean/` and deletion logs to `results/data_cleaning/removed_records/`. See the [C++ guide](scripts/Data%20Cleaning%20-%20C%2B%2B/README.md) for details.

For a more visual, interactive look at the cleaning process, you can also use the [Python cleaner](scripts/Data%20Cleaning%20-%20py/).

### File processing 
The file processing part converts the outputted cleaned csv-files from the data cleaning part and converts it into a TTree stored in a `.root` file using the class `ClimateData`. 

To run the file Processing:

* Locate the cleaned data files that are to be processed
  * Ensure that all the files are in the same directory
* Set `CLEAN_DATA_PATH` to the cleaned CSV directory, as shown below. The default in [config.txt](scripts/csv_to_ttree/config.txt) points to the Python cleaning folder’s `dataset_clean/` directory.
* Add the filenames of all the files to `FILENAMES` in [config.txt](scripts/csv_to_ttree/config.txt)
* Run the following in the root of this repository:
```bash
CLEAN_DATA_PATH=results/data_cleaning/dataset_clean bash scripts/csv_to_ttree/process_files.sh
```


## Data Analysis
The data analysis is where the data actually gets used and represented. This project covers 3 different ways to analyze the data, those being:

1. Zuzia — Mean temperature for each calendar day across years ([Task 3.2 figures](plots/daily_mean_temperature/)).
2. Rebecca — Temperature distribution for a selected calendar day across years ([Task 3.1 figures](plots/selected_day_temperature/)).
3. Jie — [Annual cold-day trend in Lund](scripts/data_analysis_topic_3.md) ([figures and data](plots/lund_cold_days/)).

From the repository root inside the MNXB11 container, build and run all three analyses with the shared executable:

```bash
make
./main
```

The executable reads `Climate_data_file.root` from the repository root. If that file needs to be regenerated, follow the data handling steps above. From the course host, the existing wrappers `bash scripts/buildproject.sh` and `bash scripts/runproject.sh` perform the same build and run steps inside the container.