#!/usr/bin/env python3
"""
Merge cache.out and ipc.out into a single CSV file with dual-core averaging.

This script handles the case where you have two CPU cores (cpu_atom and cpu_core)
reporting metrics for the same timestamp. It averages values with the same timestamp.

Usage:
    python3 merge_with_averaging.py results/x
    
Output:
    results/x/merged_data.csv
"""

import sys
import os
import csv
from collections import defaultdict

def parse_perf_line(line):
    """
    Parse a line from cache.out or ipc.out
    Example: "     0.001057790          3.009.454      cpu_atom/L1-dcache-loads/"
    Returns: (timestamp, value)
    """
    parts = line.strip().split()
    if len(parts) < 2:
        return None, None
    
    try:
        timestamp = float(parts[0])
        # Remove dots from German number format (3.009.454 -> 3009454)
        value_str = parts[1].replace('.', '')
        value = float(value_str)
        return timestamp, value
    except (ValueError, IndexError):
        return None, None


def merge_logs_to_csv_with_averaging(result_folder):
    """
    Merge cache.out and ipc.out into a CSV with columns:
    millisecond, cache_accesses, ipc
    
    Averages entries with the same timestamp (dual-core handling)
    """
    cache_file = os.path.join(result_folder, 'cache.out')
    ipc_file = os.path.join(result_folder, 'ipc.out')
    output_file = os.path.join(result_folder, 'merged_data.csv')
    
    # Check if files exist
    if not os.path.exists(cache_file):
        print(f"Error: {cache_file} not found!")
        return False
    if not os.path.exists(ipc_file):
        print(f"Error: {ipc_file} not found!")
        return False
    
    # Read and group cache data by timestamp
    cache_by_timestamp = defaultdict(list)
    with open(cache_file, 'r') as f:
        for line in f:
            timestamp, value = parse_perf_line(line)
            if timestamp is not None:
                cache_by_timestamp[timestamp].append(value)
    
    # Read and group IPC data by timestamp
    ipc_by_timestamp = defaultdict(list)
    with open(ipc_file, 'r') as f:
        for line in f:
            timestamp, value = parse_perf_line(line)
            if timestamp is not None:
                ipc_by_timestamp[timestamp].append(value)
    
    # Get common timestamps (should be the same for both files)
    timestamps = sorted(set(cache_by_timestamp.keys()) & set(ipc_by_timestamp.keys()))
    
    if not timestamps:
        print("Error: No common timestamps found!")
        return False
    
    # Write CSV with averaged values
    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        # Header
        writer.writerow(['millisecond', 'cache_accesses', 'ipc'])
        
        # Convert timestamps to milliseconds (starting from 1)
        for idx, timestamp in enumerate(timestamps, start=1):
            # Average cache accesses and round to integer
            cache_values = cache_by_timestamp[timestamp]
            avg_cache = round(sum(cache_values) / len(cache_values))
            
            # Average IPC and keep as float
            ipc_values = ipc_by_timestamp[timestamp]
            avg_ipc = sum(ipc_values) / len(ipc_values)
            
            writer.writerow([idx, avg_cache, f"{avg_ipc:.6f}"])
    
    print(f"✓ Merged data written to: {output_file}")
    print(f"  Total rows: {len(timestamps)}")
    print(f"  Averaged {len(cache_by_timestamp[timestamps[0]])} core(s) per timestamp")
    return True


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 merge_with_averaging.py <results_folder>")
        print("Example: python3 merge_with_averaging.py results/VOLREND_11_20_16_14_16/")
        sys.exit(1)
    
    result_folder = sys.argv[1]
    
    if not os.path.isdir(result_folder):
        print(f"Error: {result_folder} is not a directory!")
        sys.exit(1)
    
    success = merge_logs_to_csv_with_averaging(result_folder)
    sys.exit(0 if success else 1)