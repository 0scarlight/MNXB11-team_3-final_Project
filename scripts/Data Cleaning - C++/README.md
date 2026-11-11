# SMHI Temperature Data Cleaning in C++

## Run the batch cleaner

Run these commands from the repository root inside the MNXB11 container. The archive lives at `datasets/datasets.tgz`. The `-C datasets` option extracts the station CSVs into `datasets/`. Skip the extraction command if the CSVs are already there. The cleaner searches this directory recursively and reads the extracted files.

```bash
tar -xzf datasets/datasets.tgz -C datasets
make smhi_cleaner
./smhi_cleaner
```

The repository’s existing `Makefile` has a `smhi_cleaner` target that builds `./smhi_cleaner`. The default `make` target still builds the analysis program `./main`.

The standalone C++17 cleaner needs a C++ compiler and the standard library. It searches `datasets/` and its subdirectories for standard SMHI station files named `smhi-opendata_1_<station-id>_<date>_<time>_<station>.csv`. The bundled archive has 11 matching station CSVs. The filename rule also accepts downloaded copies with a trailing `(number)` in the filename. Supplemental datasets in other formats are outside this batch. Supply only one raw file per station name.

Raw files are opened for reading. The default output directories are:

| Directory | File produced for each station |
| --- | --- |
| `results/data_cleaning/dataset_clean/` | `<station>_Clean.csv`, with flagged rows removed and the original metadata, header, footer, and retained rows preserved. |
| `results/data_cleaning/removed_records/` | `<station>_removed_records.csv`, with original CSV line number, date, UTC time, temperature, all applicable error codes, and the complete original record. A station with no deletions gets a header-only log. |

Both implementations check the SMHI field format, strictly increasing timestamps, absolute temperature changes greater than 3 °C/hour, and temperatures outside −45 °C to +35 °C. The time-order and rate checks compare adjacent **original** observations, even if the preceding observation is also flagged. The C++ batch cleaner produces two files per station and reports failures on standard error.

You can also specify the input directory and output root. Relative paths are resolved from the directory where you run the command. Choose an output root outside the raw-data directory.

```bash
./smhi_cleaner datasets /path/to/cleaning-output
```

To use the cleaned CSVs in the ROOT conversion:

```bash
CLEAN_DATA_PATH=results/data_cleaning/dataset_clean bash scripts/csv_to_ttree/process_files.sh
```

The [interactive Python guide](../Data%20Cleaning%20-%20py/README.md) describes the same checks in detail.
