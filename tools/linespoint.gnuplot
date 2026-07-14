# GNUPlot script to plot multiple series as lines on the same graph

# Alias command-line arguments for better readability
INPUT_FILE_PATH = ARG1
OUTPUT_FILE_PATH = ARG2
X_LABEL = ARG3
Y_LABEL = ARG4
SERIES_COUNT = ARG5
X_LOG_SCALE = ARG6
Y_LOG_SCALE = ARG7

# Customize graph style
set terminal svg size 600, 400
set output OUTPUT_FILE_PATH
set key bmargin center
set bmargin (SERIES_COUNT + 4)
if (X_LOG_SCALE != 0) { set logscale x X_LOG_SCALE }
if (Y_LOG_SCALE != 0) { set logscale y Y_LOG_SCALE }
set format x "%.0f"
set format y "%.3g"
set xlabel X_LABEL
set ylabel Y_LABEL
set grid
set pointsize 0.5

# Plot all series on the same graph
plot for[in=0:SERIES_COUNT - 1] INPUT_FILE_PATH \
    index in \
    using 1:2 \
    with linespoint \
    title columnheader(1) \
    pointtype 7
