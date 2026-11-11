# MNXB11 Final Project — SMHI Temperature Analysis

Team 3 uses historical SMHI observations to analyse temperatures and cold-day trends. See the [Work Plan](Workplan.md) for the workflow and responsibilities, and [the LaTeX report source](LaTeX/main.tex) for the scientific write-up.

## Build and run the analyses

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
| Annual count and trend of days with mean temperature below 0 °C in Lund | `lund_cold_days.pdf`, `lund_cold_days.png` | [Topic 3 method](scripts/data_analysis_topic_3.md) and [reference results](plots/lund_cold_days/) |

## Rebuild the analysis input from raw data

### Data cleaning

The C++ batch cleaner runs four quality checks on the SMHI station data. From the repository root in the MNXB11 container, run the following. Skip the extraction line if the raw station CSVs are already in `datasets/`.

```bash
tar -xzf datasets/datasets.tgz -C datasets
make smhi_cleaner
./smhi_cleaner
```

The cleaner reads station CSVs from `datasets/` and writes cleaned data to `results/data_cleaning/dataset_clean/` and deletion logs to `results/data_cleaning/removed_records/`. See the [C++ guide](scripts/Data%20Cleaning%20-%20C%2B%2B/README.md) for details.

For a more visual, interactive look at the cleaning process, you can also use the [Python cleaner](scripts/Data%20Cleaning%20-%20py/).

### Convert the cleaned CSVs to ROOT

From the repository root on the MNXB11 course host, run the conversion wrapper. It launches ROOT in the course container and writes `Climate_data_file.root` in the repository root.

```bash
CLEAN_DATA_PATH=results/data_cleaning/dataset_clean bash scripts/csv_to_ttree/process_files.sh
```

[The conversion configuration](scripts/csv_to_ttree/config.txt) already lists the bundled stations. Update the list there if you are using a different set of station files. Once conversion finishes, use the build and run commands above.
