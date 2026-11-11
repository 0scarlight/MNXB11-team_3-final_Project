# Data Analysis Topic 3 — Cold Days in Lund

This analysis asks whether the annual number of days with a mean temperature below 0 °C has changed over time in Lund. It uses the Lund station observations in the project's shared ROOT tree. The historical reference results give a fitted trend of **1.32 fewer cold days per decade**.

## Where this analysis fits

The [project workflow](../README.md#proposed-project-workflow) describes the steps that produce the analysis input. The shared [program entry point](../main.cxx) opens `Climate_data_file.root` and passes its tree to [the Lund analysis](../src/lund_cold_days.cxx). This step turns individual temperature measurements into one count per observed calendar year, then fits a linear trend to those counts.

![Annual cold days in Lund and their linear trend](../plots/lund_cold_days/reference_plot.png)

## How the count works

The program reads the Lund measurements from the ROOT tree in date order. For each day it adds the readings in tenths of a degree. A negative sum means the daily mean was below zero, so the day counts once. A mean of exactly zero does not count.

Each point represents one year with observations. Missing days are skipped, and a year with no observations has no point. Incomplete years can therefore have lower counts. ROOT fits a straight line to the yearly counts with `TGraph::Fit("pol1")`. Multiplying the slope by ten gives the change in days per decade.

The reference results contain no annual points for 1958–1960. Years with only part of the year observed are still plotted. Missing observations during cold months can reduce the annual count, so the fitted slope summarizes the available records and can be affected by observation coverage.

## Run in the MNXB11 container

Follow the [shared build and run instructions](../README.md#data-analysis). Running the shared executable runs all three analyses using `Climate_data_file.root`.

The Lund analysis prints the fitted slope and saves `lund_cold_days.png` and `lund_cold_days.pdf` in the working directory. Yearly counts stay in memory.

## Reference results

The [yearly counts](../plots/lund_cold_days/lund_cold_days.csv) cover 244 years with observations between 1780 and 2026. The preview above and its [PDF version](../plots/lund_cold_days/reference_plot.pdf) were plotted separately from those counts. Running the C++ program draws the data using ROOT's default style.
