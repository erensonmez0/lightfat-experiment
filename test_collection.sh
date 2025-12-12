#!/bin/bash
#
# Test script - runs only 10 of each to verify setup
#
# Usage: bash test_collection.sh
#

set -e

SCRIPT_DIR="$HOME/lightfat-experiment"
SYRINGE_DIR="$SCRIPT_DIR/syringe_attacks"
MONITOR_SCRIPT="$SCRIPT_DIR/scripts/get_trace_of_arg_jetson.sh"

echo "=========================================="
echo "TEST: Running 10 samples of each type"
echo "=========================================="

# Normal
echo "Testing normal..."
for i in {1..10}; do
    bash "$MONITOR_SCRIPT" "$SYRINGE_DIR/syringe_pump" > /dev/null 2>&1
    echo "  Normal $i/10"
done

# Attack 1
echo "Testing attack 1..."
for i in {1..10}; do
    bash "$MONITOR_SCRIPT" "$SYRINGE_DIR/attack_arbitrary_execution random" > /dev/null 2>&1
    echo "  Attack 1 $i/10"
done

# Attack 2
echo "Testing attack 2..."
for i in {1..10}; do
    bash "$MONITOR_SCRIPT" "$SYRINGE_DIR/attack_node_skipping" > /dev/null 2>&1
    echo "  Attack 2 $i/10"
done

# Attack 3
echo "Testing attack 3..."
for i in {1..10}; do
    bash "$MONITOR_SCRIPT" "$SYRINGE_DIR/attack_data_corruption" > /dev/null 2>&1
    echo "  Attack 3 $i/10"
done

echo ""
echo "Test complete! Check results/ directory"
echo "If everything looks good, run: bash collect_all_traces.sh"
