#!/bin/bash
#
# Generate final dataset CSV from all processed traces
# Creates dataset_combined.csv with labels for ML training
#
# Usage: bash generate_dataset.sh
#

set -e

SCRIPT_DIR="$HOME/lightfat-experiment"
RESULTS_DIR="$SCRIPT_DIR/results"
OUTPUT_FILE="$SCRIPT_DIR/dataset_combined.csv"

echo "=========================================="
echo "Generating Final Dataset CSV"
echo "=========================================="
echo ""

# Check if results directory exists
if [ ! -d "$RESULTS_DIR" ]; then
    echo "ERROR: Results directory not found at $RESULTS_DIR"
    exit 1
fi

# Create CSV header
echo "label,timestamp,cache_loads,ipc" > "$OUTPUT_FILE"
echo "Created output file: $OUTPUT_FILE"
echo ""

# Counters
NORMAL_COUNT=0
ATTACK1_COUNT=0
ATTACK2_COUNT=0
ATTACK3_COUNT=0

# Function to add data with label
add_to_dataset() {
    local pattern=$1
    local label=$2
    local count_var=$3
    
    echo "Processing $label traces..."
    
    for result_dir in "$RESULTS_DIR"/$pattern/; do
        if [ -d "$result_dir" ] && [ -f "$result_dir/merged_data_averaged.csv" ]; then
            # Skip header line and add label column
            tail -n +2 "$result_dir/merged_data_averaged.csv" | \
                awk -v label="$label" '{print label "," $0}' >> "$OUTPUT_FILE"
            
            case $count_var in
                "normal") ((NORMAL_COUNT++)) ;;
                "attack1") ((ATTACK1_COUNT++)) ;;
                "attack2") ((ATTACK2_COUNT++)) ;;
                "attack3") ((ATTACK3_COUNT++)) ;;
            esac
        fi
    done
    
    echo "  → Added traces from ${!count_var} runs"
}

# Add normal baseline traces
add_to_dataset "syringe_pump_*" "normal" "normal"

# Add attack 1 traces (arbitrary execution)
add_to_dataset "attack_arbitrary_execution_*" "attack1" "attack1"

# Add attack 2 traces (node skipping)
add_to_dataset "attack_node_skipping_*" "attack2" "attack2"

# Add attack 3 traces (data corruption)
add_to_dataset "attack_data_corruption_*" "attack3" "attack3"

# Count total lines (excluding header)
TOTAL_LINES=$(($(wc -l < "$OUTPUT_FILE") - 1))

echo ""
echo "=========================================="
echo "Dataset Generation Complete!"
echo "=========================================="
echo "Output file: $OUTPUT_FILE"
echo ""
echo "Dataset Summary:"
echo "  Normal baseline: $NORMAL_COUNT runs"
echo "  Attack 1 (arbitrary exec): $ATTACK1_COUNT runs"
echo "  Attack 2 (node skipping): $ATTACK2_COUNT runs"
echo "  Attack 3 (data corruption): $ATTACK3_COUNT runs"
echo "  ----------------------------------------"
echo "  Total data points: $TOTAL_LINES"
echo "=========================================="
echo ""
echo "Preview (first 10 lines):"
head -10 "$OUTPUT_FILE"
echo ""
echo "File ready to send to Jefferson!"
echo "Download with: scp jetson@jetson19:$OUTPUT_FILE ~/Desktop/"
echo ""
