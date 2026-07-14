#!/usr/bin/env python3
"""
Plots JSON results from Google Benchmark using GNUPlot
"""

from argparse import ArgumentParser
import json
from operator import itemgetter
from os.path import dirname, join, realpath
import re
import subprocess
import sys
from tempfile import mkstemp

SELF_ROOT = dirname(realpath(__file__))

WELL_KNOWN_COLUMN_LABELS = {
    "perf.branch.miss": "Branch mispredictions",
    "perf.branch.total": "Branches",
    "perf.cycle": "CPU cycles",
    "perf.instr": "Instructions",
    "thru.in.item": "Throughput (items/s)",
    "thru.out.item": "Throughput (items/s)",
    "thru.in.byte": "Throughput (B/s)",
    "thru.in.byte": "Throughput (B/s)",
}


def escape_text(text: str) -> str:
    """Escape a text in a way suitable for a column header"""
    # WARNING: The backslash itself needs to be escaped
    return name.replace("_", "\\\\_")


if __name__ == "__main__":
    # Parse command-line arguments
    arg_parser = ArgumentParser()
    arg_parser.add_argument("-i", "--input_file", type=str, required=True)
    arg_parser.add_argument("-o", "--output_file", type=str, required=True)
    arg_parser.add_argument("-c", "--column_name", type=str, required=True,
        help="Name of the value column in the input file (Y axis)")
    arg_parser.add_argument("-xl", "--x-label", type=str,
        help="Label to use for the X axis")
    arg_parser.add_argument("-yl", "--y-label", type=str,
        help="Label to use for the Y axis")
    arg_parser.add_argument("-f", "--filter", type=str,
        help="Filter test case names using a regular expression")
    arg_parser.add_argument("-xs", "--x-scale", type=int, default=0,
        help="Use a logarithmic scale for the X axis with the specified base")
    arg_parser.add_argument("-ys", "--y-scale", type=int, default=0,
        help="Use a logarithmic scale for the Y axis with the specified base")
    args = arg_parser.parse_args()

    # Read input file
    with open(args.input_file, "r") as f:
        results = json.load(f)

    # Parse optional filter
    filter_regex = re.compile(args.filter, re.IGNORECASE) if args.filter else None

    # Parse JSON results into series of values
    series = {}
    for result in results["benchmarks"]:
        # Apply filter (if any)
        if filter_regex is not None and not filter_regex.search(result["name"]):
            continue

        # Parse result
        name_fields = result["name"].split("/")
        if len(name_fields) < 2:
            continue
        name = name_fields[0]
        x = int(name_fields[1])
        y = result[args.column_name]

        # Add data point to series
        if name not in series:
            series[name] = []
        series[name].append((x, y))

    # Freeze order for predictable output
    ordered_series = list(series.items())

    # Generate data compatible with GNUPlot
    tmp_fd, tmp_path = mkstemp(prefix="gnuplot", suffix=".dat")
    with open(tmp_fd, "w") as f:
        for (name, values) in ordered_series:
            f.write(f"\"{escape_text(name)}\"\n")
            for (x, y) in values:
                f.write(f"{x}\t{y}\n")
            # WARNING: GNUPlot requires *two* blank lines between each block
            f.write("\n\n")

    # Run GNUPlot
    script_path = join(SELF_ROOT, "linespoint.gnuplot")
    subprocess.run([
        "gnuplot",
        "-c", script_path,
        tmp_path,
        args.output_file,
        args.x_label or "",
        args.y_label or WELL_KNOWN_COLUMN_LABELS.get(args.column_name) or args.column_name,
        str(len(ordered_series)),
        str(args.x_scale),
        str(args.y_scale),
    ])
