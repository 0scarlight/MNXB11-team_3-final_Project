# Introduction:
This repository contains and documents a solution to the final project of the course MNXB11 given at LU made and implemented by Team 3. The given task was to extract, format, and analyze a data set to extract and represent the data using the ROOT framework.

# Project structure: 
The project has been split into two major parts, with each part subsequently being split into subtasks.  

## Data Handling 
The data handling is the part where the raw data gets vetted and converted into a more manageable format for the data analysis and consists of:

See the [Work Plan](Workplan.md) for the workflow and responsibilities and the [LaTeX report source](LaTeX/main.tex) for the scientific write-up.

### Data cleaning

The C++ batch cleaner runs four quality checks on the SMHI station data. From the repository root in the MNXB11 container, run the following. Skip the extraction line if the raw station CSVs are already in `datasets/`.

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
* Set the `CLEAN_DATA_PATH` in [config.txt](scripts/csv_to_ttree/config.txt) to the directory containing all the data files that are to be processed
* Add the filenames of all the files to `FILENAMES` in [config.txt](scripts/csv_to_ttree/config.txt)
* Run the following in the root of this repository:
```bash
scripts/csv_to_ttree/process_files.sh
```

Run the conversion from the MNXB11 course host. If you used the C++ cleaner above, the existing station list is ready to use. Pass the directory containing its cleaned CSVs with:

```bash
CLEAN_DATA_PATH=results/data_cleaning/dataset_clean bash scripts/csv_to_ttree/process_files.sh
```

The conversion creates `Climate_data_file.root` in the repository root.

## Data Analysis

The project covers three ways to analyse the data.

Start in the repository root inside the MNXB11 container. The build uses C++17 and the container's ROOT installation (`root-config --version` shows the installed version). The repository includes `Climate_data_file.root`, the input for all three analyses.

```bash
make
./main
```

From the MNXB11 course host, you can also use `bash scripts/buildproject.sh` and `bash scripts/runproject.sh` to build and run inside the container.

| Analysis | Output in the repository root | Reference results |
| --- | --- | --- |
| Mean temperature for each calendar day across years | `daily_mean_over_years.pdf` | [Task 3.2 figures](plots/daily_mean_temperature/) |
| Temperature distribution for one selected calendar day across years | `day_analysis.pdf` | [Task 3.1 figures](plots/selected_day_temperature/) |
| Annual Cold Days (days with mean temperature below 0 °C) | `annual_cold_days.pdf`, `annual_cold_days.png` | [Topic 3 method](scripts/data_analysis_topic_3.md) and [reference results](plots/annual_cold_days/) |
