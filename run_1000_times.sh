#!/bin/bash
# Run VOLREND 1000 times and collect averaged metrics
# Each run produces one row with: average_cache_accesses, average_ipc

# Configuration
NUM_RUNS=1000
APP_NAME="VOLREND"
SAMPLING_PERIOD=1  # milliseconds
DURATION=0.5       # seconds
OUTPUT_CSV="volrend_1000_runs.csv"

# Paths (adjust if needed)
MONITOR_SCRIPT="monitor.py"
VOLREND_EXEC="./Splash-3/codes/apps/volrend/VOLREND"
VOLREND_ARGS="1 ./Splash-3/codes/apps/volrend/inputs/head 8"
MERGE_SCRIPT="merge_with_averaging.py"

# Create CSV header
echo "run_id,avg_cache_accesses,avg_ipc" > $OUTPUT_CSV
echo "Created output file: $OUTPUT_CSV"

# Main loop
for run_id in $(seq 1 $NUM_RUNS)
do
    echo "=========================================="
    echo "Run $run_id / $NUM_RUNS"
    echo "=========================================="
    
    # 1. Start the monitor in background
    python3 $MONITOR_SCRIPT -a $APP_NAME -s $SAMPLING_PERIOD -d $DURATION &
    MONITOR_PID=$!
    
    # Give monitor time to start
    sleep 1
    
    # 2. Run VOLREND (this triggers the monitoring via signal)
    $VOLREND_EXEC $VOLREND_ARGS
    
    # 3. Wait for monitor to finish
    wait $MONITOR_PID
    
    # 4. Find the most recent results folder
    LATEST_RESULT=$(ls -td results/${APP_NAME}_* | head -1)
    
    if [ ! -d "$LATEST_RESULT" ]; then
        echo "ERROR: No results folder found for run $run_id"
        continue
    fi
    
    echo "Processing results from: $LATEST_RESULT"
    
    # 5. Merge cache.out and ipc.out with averaging
    python3 $MERGE_SCRIPT $LATEST_RESULT
    
    # 6. Calculate averages from merged_data.csv
    MERGED_CSV="$LATEST_RESULT/merged_data.csv"
    
    if [ ! -f "$MERGED_CSV" ]; then
        echo "ERROR: merged_data.csv not found"
        continue
    fi
    
    # Calculate averages (skip header, average columns 2 and 3)
    AVG_CACHE=$(awk -F',' 'NR>1 {sum+=$2; count++} END {if(count>0) print int(sum/count+0.5); else print 0}' $MERGED_CSV)
    AVG_IPC=$(awk -F',' 'NR>1 {sum+=$3; count++} END {if(count>0) printf "%.6f", sum/count; else print 0}' $MERGED_CSV)
    
    # 7. Append to final CSV
    echo "$run_id,$AVG_CACHE,$AVG_IPC" >> $OUTPUT_CSV
    
    echo "✓ Run $run_id complete: avg_cache=$AVG_CACHE, avg_ipc=$AVG_IPC"
    
    # Optional: Clean up individual results to save space
    # Uncomment the next line if you want to delete intermediate files
    # rm -rf $LATEST_RESULT
    
    # Small delay between runs
    sleep 1
done

echo ""
echo "=========================================="
echo "ALL RUNS COMPLETE!"
echo "=========================================="
echo "Results saved to: $OUTPUT_CSV"
echo "Total runs: $(wc -l < $OUTPUT_CSV | xargs echo $(($(cat) - 1)))"
echo ""
echo "You can now analyze the data with:"
echo "  - Excel/Google Sheets"
echo "  - Python pandas: pd.read_csv('$OUTPUT_CSV')"
echo "  - Plot distributions, etc."