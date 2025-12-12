#!/bin/bash
#
# Master script to collect all performance traces for LightFat+ validation
# Runs normal baseline + 3 attack variations with proper monitoring
#
# Usage: bash collect_all_traces.sh
#

set -e  # Exit on error

SCRIPT_DIR="$HOME/lightfat-experiment"
SYRINGE_DIR="$SCRIPT_DIR/syringe_attacks"
MONITOR_SCRIPT="$SCRIPT_DIR/scripts/get_trace_of_arg_jetson.sh"
RESULTS_DIR="$SCRIPT_DIR/results"

# Configuration
NORMAL_RUNS=1500
ATTACK_RUNS=500

echo "=========================================="
echo "LightFat+ Trace Collection Script"
echo "=========================================="
echo "Normal runs: $NORMAL_RUNS"
echo "Attack runs: $ATTACK_RUNS per attack type"
echo "Total runs: $((NORMAL_RUNS + ATTACK_RUNS * 3))"
echo "=========================================="
echo ""

# Check if binaries exist
if [ ! -f "$SYRINGE_DIR/syringe_pump" ]; then
    echo "ERROR: Binaries not found! Please run 'make all' first."
    exit 1
fi

# Check if monitoring script exists
if [ ! -f "$MONITOR_SCRIPT" ]; then
    echo "ERROR: Monitoring script not found at $MONITOR_SCRIPT"
    exit 1
fi

# Create timestamp for this run
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
echo "Run timestamp: $TIMESTAMP"
echo ""

# Function to run with monitoring
run_monitored() {
    local binary=$1
    local run_num=$2
    local label=$3
    
    echo "[$label] Run $run_num..."
    bash "$MONITOR_SCRIPT" "$binary" > /dev/null 2>&1
}

# ===========================================
# Part 1: Normal Baseline (1500 runs)
# ===========================================
echo "=========================================="
echo "Part 1: Collecting Normal Baseline Traces"
echo "=========================================="
START_TIME=$(date +%s)

for i in $(seq 1 $NORMAL_RUNS); do
    if [ $((i % 100)) -eq 0 ]; then
        elapsed=$(($(date +%s) - START_TIME))
        echo "Progress: $i/$NORMAL_RUNS runs (${elapsed}s elapsed)"
    fi
    run_monitored "$SYRINGE_DIR/syringe_pump" "$i" "NORMAL"
done

NORMAL_TIME=$(($(date +%s) - START_TIME))
echo "✓ Normal baseline complete: $NORMAL_RUNS runs in ${NORMAL_TIME}s"
echo ""

# ===========================================
# Part 2: Attack 1 - Arbitrary Execution
# ===========================================
echo "=========================================="
echo "Part 2: Collecting Attack 1 Traces"
echo "=========================================="
START_TIME=$(date +%s)

for i in $(seq 1 $ATTACK_RUNS); do
    if [ $((i % 100)) -eq 0 ]; then
        elapsed=$(($(date +%s) - START_TIME))
        echo "Progress: $i/$ATTACK_RUNS runs (${elapsed}s elapsed)"
    fi
    run_monitored "$SYRINGE_DIR/attack_arbitrary_execution random" "$i" "ATTACK1"
done

ATTACK1_TIME=$(($(date +%s) - START_TIME))
echo "✓ Attack 1 complete: $ATTACK_RUNS runs in ${ATTACK1_TIME}s"
echo ""

# ===========================================
# Part 3: Attack 2 - Node Skipping
# ===========================================
echo "=========================================="
echo "Part 3: Collecting Attack 2 Traces"
echo "=========================================="
START_TIME=$(date +%s)

for i in $(seq 1 $ATTACK_RUNS); do
    if [ $((i % 100)) -eq 0 ]; then
        elapsed=$(($(date +%s) - START_TIME))
        echo "Progress: $i/$ATTACK_RUNS runs (${elapsed}s elapsed)"
    fi
    run_monitored "$SYRINGE_DIR/attack_node_skipping" "$i" "ATTACK2"
done

ATTACK2_TIME=$(($(date +%s) - START_TIME))
echo "✓ Attack 2 complete: $ATTACK_RUNS runs in ${ATTACK2_TIME}s"
echo ""

# ===========================================
# Part 4: Attack 3 - Data Corruption
# ===========================================
echo "=========================================="
echo "Part 4: Collecting Attack 3 Traces"
echo "=========================================="
START_TIME=$(date +%s)

for i in $(seq 1 $ATTACK_RUNS); do
    if [ $((i % 100)) -eq 0 ]; then
        elapsed=$(($(date +%s) - START_TIME))
        echo "Progress: $i/$ATTACK_RUNS runs (${elapsed}s elapsed)"
    fi
    run_monitored "$SYRINGE_DIR/attack_data_corruption" "$i" "ATTACK3"
done

ATTACK3_TIME=$(($(date +%s) - START_TIME))
echo "✓ Attack 3 complete: $ATTACK_RUNS runs in ${ATTACK3_TIME}s"
echo ""

# ===========================================
# Summary
# ===========================================
TOTAL_TIME=$((NORMAL_TIME + ATTACK1_TIME + ATTACK2_TIME + ATTACK3_TIME))

echo "=========================================="
echo "Collection Complete!"
echo "=========================================="
echo "Normal baseline: $NORMAL_RUNS runs in ${NORMAL_TIME}s"
echo "Attack 1:        $ATTACK_RUNS runs in ${ATTACK1_TIME}s"
echo "Attack 2:        $ATTACK_RUNS runs in ${ATTACK2_TIME}s"
echo "Attack 3:        $ATTACK_RUNS runs in ${ATTACK3_TIME}s"
echo "----------------------------------------"
echo "Total:           $((NORMAL_RUNS + ATTACK_RUNS * 3)) runs in ${TOTAL_TIME}s"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Run: bash process_all_traces.sh"
echo "2. Run: bash generate_dataset.sh"
echo "3. Send dataset_combined.csv to Jefferson"
echo ""
