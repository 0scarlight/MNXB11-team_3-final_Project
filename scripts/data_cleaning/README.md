
# SMHI Temperature Data Cleaning

This is my part of our course project, focusing on **data cleaning** of historical temperature observations from SMHI.

## Project Workflow

![Project Workflow](<Data Cleaning and Analysis Workflow.png>)

## Data Cleaning Procedure

The script performs four checks, in the following order:

1. Data format
2. Chronological order
3. Temperature change rate
4. Temperature range

This is intentionally a simple, coarse-cleaning approach: any observation flagged by one or more checks is removed from the cleaned dataset.

## 1. Data Format

The script checks whether each observation follows the expected SMHI format, with consistent date, time, and temperature fields.

The required formats are:

- Date: `YYYY-MM-DD`
- Time: `HH:MM:SS` (UTC)
- Temperature: A valid numeric value using a decimal point

The date and time must follow exactly the same format, including separators.

The quality field is not checked as strictly.

Records with invalid formats are flagged and removed.

## 2. Chronological Order

Observations are checked **in their original file order**, without sorting.

For each row, the script calculates the elapsed time since the preceding row:

`Δt = t_current − t_previous`

The interval must be **strictly positive** (`Δt > 0`).

- `Δt = 0`: Repeated timestamp
- `Δt < 0`: Timestamp goes backwards

Both cases are treated as errors, and the corresponding records are removed.

## 3. Temperature Change Rate

To detect unusually rapid temperature changes, the script calculates the average temperature change rate between consecutive observations:

`rate = (T_current − T_previous) / Δt`

where `Δt` is measured in hours.

If the **absolute value** of the rate exceeds **3 °C/hour**, the current observation is flagged and removed.

In other words:

`|ΔT / Δt| > 3 °C/hour`

This calculation is only performed when both temperature values are valid and `Δt > 0`.

The method uses the actual time interval between observations rather than assuming a fixed sampling frequency.

This is particularly important because historical SMHI records have different sampling frequencies in different periods.

## 4. Temperature Range

The script checks whether each temperature falls within the following range:

`−45 °C ≤ T ≤ +35 °C`

Values outside this range are treated as extreme outliers and removed.

This step aims to eliminate potentially incorrect observations with unusually high or low temperature values.

## Visualization and Manual Inspection

In addition to these four checks, the script provides interactive visualizations of the original data.

Four variables are plotted:

1. **Sampling interval (Δt)** — Time difference between consecutive observations.
2. **Temperature difference (ΔT)** — Temperature change between consecutive observations.
3. **Temperature change rate (ΔT/Δt)** — Average hourly temperature change.
4. **Raw temperature (T)** — Recorded temperature over time.

The plots support:

- Zooming and panning
- Interactive point selection
- Displaying the original CSV line number
- Displaying timestamps and corresponding values

Importantly, **all four plots use the original, uncleaned data**.

This allows us to inspect the data manually and identify potential anomalies that may not be adequately captured by the automatic cleaning procedure.

After closing the plots, the script reports the detected errors in the terminal.

For each flagged observation, the terminal displays:

- Error category
- Original CSV line number
- Two preceding observations
- The flagged observation
- Two following observations

A summary of the number of errors in each category is also provided.

## Limitations and Feedback

**Identifying abnormal temperature change rates was the most challenging part of this cleaning process.**

A rapid temperature change may indicate an incorrect measurement, but it can also be a genuine weather event.

Distinguishing between these two situations is difficult.

We chose a threshold of **3 °C/hour** as a compromise.

If we choose a higher threshold, some abnormal observations may not be detected.

However, even with a threshold of 3 °C/hour, some genuine temperature changes can exceed this limit.

As a result, this method inevitably produces some **false positives**, meaning that some valid observations may be incorrectly identified as errors and removed.

We considered and experimented with several alternative approaches to distinguish genuine rapid temperature changes from measurement errors.

However, for this course project, these approaches did not provide a clear practical improvement over the simple threshold-based method.

Therefore, we decided to use this relatively straightforward coarse-cleaning approach.

**Suggestions and improvements are very welcome!**

If you have a better method for distinguishing actual measurement errors from genuine rapid temperature changes, especially one that can reduce the removal of valid observations while still identifying abnormal data effectively, please feel free to share your ideas.

## How to Run

### Requirements

- Python 3.9+
- pandas
- matplotlib
- numpy

Install the required packages:

```bash
pip install pandas matplotlib numpy
```

Set the `file_path` near the beginning of `smhi_cleaner_four_steps.py` to the SMHI CSV file you want to process.

For example:

```python
file_path = "smhi-opendata_1_53430_20261007_164857_Lund.csv"
```

Then run:

```bash
python smhi_cleaner_four_steps.py
```

The original CSV file will not be overwritten.

For a station named Lund, the script generates:

- `Lund_Clean.csv` — Cleaned data with original metadata and headers preserved.
- `Lund_removed_records.csv` — Removed observations and their corresponding error categories.
- `Lund_suspicious_records.csv` — An empty report containing only the header, retained for compatibility with the earlier version.

All observations flagged by the four checks are removed in this version.

**Note:** This is a coarse-cleaning procedure rather than a fully validated meteorological quality-control algorithm. The selected thresholds are project-specific choices.

---

## Notes for the Next Stages: Missing Data

This script performs **coarse cleaning only**.

It removes observations that fail the four checks described above, but it **does not fill in missing observations or reconstruct incomplete time series**.

### 1. Missing Data in the Historical Records

One major issue that remains after coarse cleaning is the significant amount of **missing data** in the historical temperature records.

Missing observations can occur at different time scales, including:

- **Individual measurements:** Missing observations at particular sampling times or hours.
- **Daily gaps:** Entire days with no available measurements.
- **Monthly gaps:** Missing observations over extended periods, potentially covering entire months.
- **Yearly gaps:** Longer interruptions that may extend over entire years.

Such gaps are already present in the original historical datasets.

In addition, the coarse-cleaning procedure may introduce a small number of additional gaps when abnormal observations are removed.

It is also important to note that **the sampling frequency has changed throughout the historical records**.

Therefore, a long interval between two measurements does not necessarily indicate missing data. Some intervals may simply reflect the historical sampling schedule.

This should be considered when identifying genuinely missing observations.

### 2. Potential Effects on Data Analysis

Missing data may affect subsequent data analysis, especially when calculating:

- Daily average temperatures
- Monthly average temperatures
- Annual average temperatures
- Long-term temperature trends
- Comparisons between different historical periods

For example, if only a small number of temperature measurements are available for a particular day, their arithmetic average may not accurately represent the actual daily mean temperature.

Similarly, if measurements are missing unevenly across different times of day or seasons, the calculated averages may be biased.

Therefore, the **number and distribution of available measurements** should be considered when interpreting the results.

### 3. Suggestions for Further Data Processing

The next stage of data processing and optimization may need to address these missing-data issues.

Possible improvements include:

- Identifying genuinely missing observations based on the historical sampling schedules.
- Evaluating the completeness of data within each day, month, or year.
- Establishing minimum data-coverage requirements for statistical analysis.
- Applying appropriate interpolation or gap-filling methods when scientifically justified.
- Taking the distribution of available measurements into account when calculating averages.

These issues are not handled in the current coarse-cleaning script.

Instead, they are left for the subsequent **Data Completion & Optimization** and **Data Analysis** stages.

The purpose of the current stage is simply to remove observations that violate the four predefined checks while preserving the remaining original measurements without additional modifications.

