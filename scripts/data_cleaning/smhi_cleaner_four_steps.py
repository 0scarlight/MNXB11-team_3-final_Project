"""SMHI coarse cleaning: four checks + four interactive RAW-data plots.

Install: pip install pandas matplotlib numpy
Run: python smhi_cleaner_four_steps.py
The original CSV is never overwritten. Close the plot windows to continue cleaning.
"""

import csv
import math
import os
import re
from collections import Counter
from datetime import datetime

import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import pandas as pd

# ==========================================================
# 0. CONFIGURATION: EDIT ONLY THE FILE PATH AS NEEDED
# ==========================================================
file_path = r"c:\Users\o-sca\Desktop\Stuff\LU\MNXB11-team_3-final_Project\datasets\smhi-opendata_1_52230_20261007_165001_Falsterbo.csv"
# file_path = "smhi-opendata_1_53430_20261007_164857_Lund.csv"
# file_path = "smhi-opendata_1_62410_20261007_164836_Halmstad_flygplats.csv"
# file_path = "smhi-opendata_1_72450_20261007_164940_Borås.csv"
# file_path = "smhi-opendata_1_78400_20261007_165022_Visby_Flygplats.csv"

# file_path = "smhi-opendata_1_93220_20261007_164752_Karlstad_Flygplats.csv"
# file_path = "smhi-opendata_1_97530_20261007_165105_Uppsala_Flygplats.csv"
# file_path = "smhi-opendata_1_99450_20261007_164731_Söderarm_A.csv"
# file_path = "smhi-opendata_1_105370_20261007_165044_Falun-Lugnet.csv"
# file_path = "smhi-opendata_1_140480_20261007_164814_Umeå_Flygplats.csv"
# file_path = "smhi-opendata_1_162860_20261007_164918_Luleå-Kallax_Flygplats.csv"


MIN_TEMP = -45.0
MAX_TEMP = 35.0
MAX_RATE = 3.0  # Absolute temperature change, degrees C per hour
POINT_SIZE = 3
POINT_ALPHA = 0.6
SAVE_FIGURES = False

# Exact SMHI column format: YYYY-MM-DD;HH:MM:SS;temperature;quality
# No slashes, changed hyphens, extra spaces, or comma decimals.
DATE_PATTERN = re.compile(r"\d{4}-\d{2}-\d{2}\Z")
TIME_PATTERN = re.compile(r"\d{2}:\d{2}:\d{2}\Z")
TEMP_PATTERN = re.compile(r"[+-]?(?:\d+(?:\.\d+)?|\.\d+)\Z")
STATION_PATTERN = re.compile(
    r"^smhi-opendata_1_\d+_\d{8}_\d{6}_(.+?)(?:\(\d+\))?\.csv$", re.I
)
ERRORS = ("INVALID_FORMAT", "TIME_ORDER", "TEMPERATURE_RATE", "TEMPERATURE_RANGE")


def read_original(path):
    # Keep every original line (metadata, header, measurements, footer) unchanged.
    with open(path, "r", encoding="utf-8-sig", newline="") as f:
        lines = f.readlines()

    header_idx = None
    for i, line in enumerate(lines):
        cols = next(csv.reader([line.rstrip("\r\n")], delimiter=";"))
        if "Datum" in cols and "Tid (UTC)" in cols and "Lufttemperatur" in cols:
            header_idx = i
            columns = cols
            break
    if header_idx is None:
        raise ValueError("SMHI header with Datum / Tid (UTC) / Lufttemperatur not found")

    date_col = columns.index("Datum")
    time_col = columns.index("Tid (UTC)")
    temp_col = columns.index("Lufttemperatur")
    need_columns = max(date_col, time_col, temp_col) + 1
    records = []
    started = False

    for i in range(header_idx + 1, len(lines)):
        raw = lines[i].rstrip("\r\n")
        if not raw.strip():
            if started:
                # Preserve blank lines, but do not assume this is the end of the data.
                continue
            continue
        fields = next(csv.reader([raw], delimiter=";"))
        # A non-table line marks the beginning of the SMHI footer.
        if len(fields) < need_columns and ";" not in raw:
            if started:
                break
            continue
        started = True
        date_s = fields[date_col] if len(fields) > date_col else ""
        time_s = fields[time_col] if len(fields) > time_col else ""
        temp_s = fields[temp_col] if len(fields) > temp_col else ""
        errors = []
        stamp = None
        temp = None

        # STEP 1: Validate the exact strings BEFORE parsing or normalizing.
        if len(fields) < need_columns or not DATE_PATTERN.fullmatch(date_s) or not TIME_PATTERN.fullmatch(time_s):
            errors.append("INVALID_FORMAT")
        else:
            try:
                stamp = datetime.strptime(date_s + " " + time_s, "%Y-%m-%d %H:%M:%S")
            except ValueError:  # impossible calendar dates, 25:00:00 etc.
                errors.append("INVALID_FORMAT")
        if not TEMP_PATTERN.fullmatch(temp_s):
            if "INVALID_FORMAT" not in errors:
                errors.append("INVALID_FORMAT")
        else:
            try:
                temp = float(temp_s)
                if not math.isfinite(temp):
                    raise ValueError("nonfinite")
            except ValueError:
                temp = None
                if "INVALID_FORMAT" not in errors:
                    errors.append("INVALID_FORMAT")

        records.append({
            "line": i + 1, "raw": raw, "timestamp": stamp, "temperature": temp,
            "errors": errors, "dt_hours": np.nan, "delta_C": np.nan,
            "rate_C_per_hour": np.nan,
        })

    if not records:
        raise ValueError("No measurement records found")
    return lines, records


def inspect_original(records):
    # No sorting, interpolation or deleting before any checks / plots.
    for i, rec in enumerate(records):
        if i > 0:
            prev = records[i - 1]
            if rec["timestamp"] is not None and prev["timestamp"] is not None:
                hours = (rec["timestamp"] - prev["timestamp"]).total_seconds() / 3600
                rec["dt_hours"] = hours
                if hours <= 0:
                    rec["errors"].append("TIME_ORDER")
            if rec["temperature"] is not None and prev["temperature"] is not None:
                change = rec["temperature"] - prev["temperature"]
                rec["delta_C"] = change
                if pd.notna(rec["dt_hours"]) and rec["dt_hours"] > 0:
                    rate = change / rec["dt_hours"]
                    rec["rate_C_per_hour"] = rate
                    if abs(rate) > MAX_RATE:
                        rec["errors"].append("TEMPERATURE_RATE")
        if rec["temperature"] is not None and not MIN_TEMP <= rec["temperature"] <= MAX_TEMP:
            rec["errors"].append("TEMPERATURE_RANGE")


def plot_one(df, column, title, ylabel, color, filename):
    shown = df.loc[df["timestamp"].notna() & df[column].notna()].reset_index(drop=True)
    fig, ax = plt.subplots(figsize=(15, 5))
    scatter = ax.scatter(shown["timestamp"], shown[column], s=POINT_SIZE,
                         alpha=POINT_ALPHA, c=color, picker=7, rasterized=True)
    ax.set(title=title, xlabel="Original observation timestamp (UTC)", ylabel=ylabel)
    locator = mdates.AutoDateLocator(minticks=5, maxticks=12)
    ax.xaxis.set_major_locator(locator)
    ax.xaxis.set_major_formatter(mdates.ConciseDateFormatter(locator))
    ax.grid(alpha=0.3)
    marker, = ax.plot([], [], "o", markersize=12, markerfacecolor="none",
                      markeredgecolor="red", markeredgewidth=2, visible=False)
    annotation = ax.annotate("", xy=(0, 0), xytext=(18, 18), textcoords="offset points",
                              bbox=dict(boxstyle="round", fc="white", alpha=0.95),
                              arrowprops=dict(arrowstyle="->"), fontsize=9, visible=False)
    coords = scatter.get_offsets()

    def fmt(value, unit=""):
        return "N/A" if pd.isna(value) else f"{value:.6g}{unit}"

    def on_pick(event):
        if event.artist is not scatter or event.mouseevent.button != 1:
            return
        candidates = np.asarray(event.ind, dtype=int)
        if not len(candidates):
            return
        pixel = ax.transData.transform(coords[candidates])
        mouse = np.array([event.mouseevent.x, event.mouseevent.y])
        idx = int(candidates[np.argmin(np.sum((pixel - mouse) ** 2, axis=1))])
        row = shown.iloc[idx]
        x, y = coords[idx]
        marker.set_data([x], [y])
        marker.set_visible(True)
        annotation.xy = (x, y)
        annotation.set_text("\n".join([
            f"Original CSV line: {int(row['line'])}",
            f"UTC: {row['timestamp']:%Y-%m-%d %H:%M:%S}",
            f"Temperature: {fmt(row['temperature'], ' C')}",
            f"Time difference: {fmt(row['dt_hours'], ' h')}",
            f"Temperature difference: {fmt(row['delta_C'], ' C')}",
            f"Average hourly change: {fmt(row['rate_C_per_hour'], ' C/h')}",
            f"Errors: {row['error_text'] or 'None'}",
        ]))
        annotation.set_position(((-18 if event.mouseevent.x > fig.bbox.width * 0.6 else 18),
                                 (-18 if event.mouseevent.y > fig.bbox.height * 0.6 else 18)))
        annotation.set_ha("right" if event.mouseevent.x > fig.bbox.width * 0.6 else "left")
        annotation.set_va("top" if event.mouseevent.y > fig.bbox.height * 0.6 else "bottom")
        annotation.set_visible(True)
        fig.canvas.draw_idle()
        print(f"Clicked CSV line {int(row['line'])}: {row['timestamp']} | {column}={y:.5g}")

    fig.canvas.mpl_connect("pick_event", on_pick)
    fig.tight_layout()
    if SAVE_FIGURES:
        fig.savefig(filename, dpi=160)


def show_original_plots(records):
    # Four independent charts use the ORIGINAL uncleaned data.
    df = pd.DataFrame(records)
    df["error_text"] = df["errors"].apply(lambda x: ", ".join(x))
    # plot_one(df, "dt_hours", "1. Original sampling time difference", "Hours", "royalblue", "raw_time_difference.png")
    # plot_one(df, "delta_C", "2. Original temperature difference", "Temperature change (C)", "darkorange", "raw_temperature_difference.png")
    # plot_one(df, "rate_C_per_hour", "3. Original average hourly temperature change", "Temperature change (C/hour)", "crimson", "raw_hourly_change.png")
    plot_one(df, "temperature", "4. Original temperature", "Temperature (C)", "forestgreen", "raw_temperature.png")
    print("\nFour RAW-DATA plots opened. Zoom/pan using toolbar; click points for original row details.")
    print("Close ALL plot windows to continue to the deletion and file output stage.")
    plt.show(block=True)


def print_report(records):
    totals = Counter(error for rec in records for error in rec["errors"])
    problem_rows = 0
    print("\n" + "=" * 75)
    print("COARSE CLEANING: ERROR REPORT (original order, 2 before + 2 after)")
    print("=" * 75)
    for i, rec in enumerate(records):
        if not rec["errors"]:
            continue
        problem_rows += 1
        print(f"\n[CSV line {rec['line']}] ERROR: {', '.join(rec['errors'])}")
        for neighbor in records[max(0, i - 2): min(len(records), i + 3)]:
            mark = "  <-- DELETE" if neighbor is rec else ""
            print(f"  {neighbor['line']}: {neighbor['raw']}{mark}")
    print("\n" + "=" * 75)
    print("ERROR COUNTS (a row can have more than one type)")
    for code in ERRORS:
        print(f"  {code:<24} {totals[code]}")
    print(f"  Total rows with errors:  {problem_rows}")
    print(f"  Original rows:           {len(records)}")
    print(f"  Rows kept:               {len(records) - problem_rows}")
    print("=" * 75)


def save_cleaned(lines, records, source):
    # Remove every row with ANY error, preserving original lines, metadata and footer.
    station_match = STATION_PATTERN.match(os.path.basename(source))
    station = station_match.group(1) if station_match else os.path.splitext(os.path.basename(source))[0]
    directory = os.path.dirname(source) or "."
    cleaned_path = os.path.join(directory, f"{station}_Clean.csv")
    removed_path = os.path.join(directory, f"{station}_removed_records.csv")
    suspicious_path = os.path.join(directory, f"{station}_suspicious_records.csv")
    removed_lines = {rec["line"] for rec in records if rec["errors"]}
    with open(cleaned_path, "w", encoding="utf-8-sig", newline="") as out:
        for line_number, line in enumerate(lines, start=1):
            if line_number not in removed_lines:
                out.write(line)
    with open(removed_path, "w", encoding="utf-8-sig", newline="") as out:
        writer = csv.writer(out)
        writer.writerow(["csv_line", "date", "time_utc", "temperature", "errors", "original_record"])
        for rec in records:
            if rec["errors"]:
                fields = next(csv.reader([rec["raw"]], delimiter=";"))
                writer.writerow([rec["line"], fields[0] if len(fields) > 0 else "",
                                 fields[1] if len(fields) > 1 else "",
                                 fields[2] if len(fields) > 2 else "",
                                 " | ".join(rec["errors"]), rec["raw"]])
    # Keep same three output filenames as the compact cleaner.
    # No suspicious records remain: under coarse cleaning, ALL flagged rows are deleted.
    with open(suspicious_path, "w", encoding="utf-8-sig", newline="") as out:
        csv.writer(out).writerow(["csv_line", "date", "time_utc", "temperature", "reason", "original_record"])
    print("\nOUTPUT FILES:")
    print("  Cleaned: ", cleaned_path)
    print("  Removed: ", removed_path)
    print("  Suspicious (empty by design): ", suspicious_path)


def main():
    lines, records = read_original(file_path)
    inspect_original(records)
    show_original_plots(records)  # First inspect and plot RAW records; no rows have been removed.
    print_report(records)
    save_cleaned(lines, records, file_path)


if __name__ == "__main__":
    main()
