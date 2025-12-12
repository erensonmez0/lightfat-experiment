#!/bin/bash
#
# Process all collected traces using merge_with_averaging.py
# Converts raw cache.out and ipc.out files into merged_data_averaged.csv
#
# Usage: bash process_all_traces.sh
#

set -e

SCRIPT_DIR="$HOME/lightfat-experiment"
RESULTS_DIR="$SCRIPT_DIR/results"
MERGE_SCRIPT="$SCRIPT_DIR/merge_with_averaging.py"

echo "=========================================="
echo "Processing All Collected Traces"
echo "=========================================="
echo ""

# Check if merge script exists
if [ ! -f "$MERGE_SCRIPT" ]; then
    echo "ERROR: merge_with_averaging.py not found at $MERGE_SCRIPT"
    exit 1
fi

# Check if results directory exists
if [ ! -d "$RESULTS_DIR" ]; then
    echo "ERROR: Results directory not found at $RESULTS_DIR"
    exit 1
fi

# Count total directories to process
TOTAL_DIRS=$(find "$RESULTS_DIR" -maxdepth 1 -type d -name "*_*_*_*_*_*" | wc -l)
echo "Found $TOTAL_DIRS result directories to process"
echo ""

PROCESSED=0
FAILED=0

# Process all result directories
for result_dir in "$RESULTS_DIR"/*_*_*_*_*_*/; do
    if [ -d "$result_dir" ]; then
        dir_name=$(basename "$result_dir")
        
        # Check if already processed
        if [ -f "$result_dir/merged_data_averaged.csv" ]; then
            # echo "Skipping $dir_name (already processed)"
            ((PROCESSED++))
            continue
        fi
        
        # Check if has required input files
        if [ ! -f "$result_dir/cache.out" ] || [ ! -f "$result_dir/ipc.out" ]; then
            echo "WARNING: Skipping $dir_name (missing cache.out or ipc.out)"
            ((FAILED++))
            continue
        fi
        
        # Process this directory
        if [ $((PROCESSED % 100)) -eq 0 ]; then
            echo "Processing: $PROCESSED/$TOTAL_DIRS..."
        fi
        
        python3 "$MERGE_SCRIPT" "$result_dir" > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            ((PROCESSED++))
        else
            echo "ERROR: Failed to process $dir_name"
            ((FAILED++))
        fi
    fi
done

echo ""
echo "=========================================="
echo "Processing Complete!"
echo "=========================================="
echo "Successfully processed: $PROCESSED directories"
echo "Failed: $FAILED directories"
echo "=========================================="
echo ""
echo "Next step: bash generate_dataset.sh"
echo ""
