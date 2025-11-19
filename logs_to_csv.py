#!/usr/bin/env python3
"""
Merge cache.log and ipc.log into a single CSV file.

Usage:
    python3 logs_to_csv.py results/signal_11_13_14_34_44/
    
Output:
    results/signal_11_13_14_34_44/merged_data.csv
"""

import sys
import os
import csv

def merge_logs_to_csv(result_folder):
    """
    Merge cache.log and ipc.log into a CSV with columns:
    millisecond, cache_accesses, ipc
    """
    cache_file = os.path.join(result_folder, 'cache.log')
    ipc_file = os.path.join(result_folder, 'ipc.log')
    output_file = os.path.join(result_folder, 'merged_data.csv')
    
    # Check if files exist
    if not os.path.exists(cache_file):
        print(f"Error: {cache_file} not found!")
        return False
    if not os.path.exists(ipc_file):
        print(f"Error: {ipc_file} not found!")
        return False
    
    # Read both files
    with open(cache_file, 'r') as f:
        cache_data = [line.strip() for line in f if line.strip()]
    
    with open(ipc_file, 'r') as f:
        ipc_data = [line.strip() for line in f if line.strip()]
    
    # They should have the same length
    if len(cache_data) != len(ipc_data):
        print(f"Warning: cache.log has {len(cache_data)} lines, ipc.log has {len(ipc_data)} lines")
        min_len = min(len(cache_data), len(ipc_data))
    else:
        min_len = len(cache_data)
    
    # Write CSV
    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        # Header
        writer.writerow(['millisecond', 'cache_accesses', 'ipc'])
        
        # Data rows (assuming 1ms sampling period)
        for i in range(min_len):
            millisecond = i + 1  # 1-indexed time
            cache_val = cache_data[i]
            ipc_val = ipc_data[i]
            writer.writerow([millisecond, cache_val, ipc_val])
    
    print(f"✓ Merged data written to: {output_file}")
    print(f"  Total rows: {min_len}")
    return True


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 logs_to_csv.py <results_folder>")
        print("Example: python3 logs_to_csv.py results/signal_11_13_14_34_44/")
        sys.exit(1)
    
    result_folder = sys.argv[1]
    
    if not os.path.isdir(result_folder):
        print(f"Error: {result_folder} is not a directory!")
        sys.exit(1)
    
    success = merge_logs_to_csv(result_folder)
    sys.exit(0 if success else 1)