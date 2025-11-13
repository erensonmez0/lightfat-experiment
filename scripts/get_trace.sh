#!/bin/bash

## This script periodically gets traces from a target process (by name) from a certain duration using perf.

## First we get the pid of target process in argument
pid=`pidof $1`
echo "Getting trace for $1, with pid $pid"


# Just some preparation for the output files
if [ ! -d "results" ] 
then
    mkdir results
fi
now=$(date +"%m_%d_%H_%M_%S")
prefix="$1_$now"
mkdir results/$prefix

# set the sample period in ms
sampling_period=$2
# set window duration in seconds 
duration=$3

# call perf to get samples
perf stat -B -I $sampling_period -e cycles,instructions,L1-dcache-loads  -p $pid  -o results/tmp.log  sleep $duration

#create sub-files
# add sleep maybe (5 or 3)
sleep 3  # Wait for file sync

cat results/tmp.log | grep "cache" > results/cache_raw.log; cat results/cache_raw.log | grep -v not > results/$prefix/cache.out
cat results/tmp.log | grep "ins" > results/ipc_raw.log; cat results/ipc_raw.log | grep -v not > results/$prefix/ipc.out
rm -rf results/*.log

# call script to fix file format
python3 scripts/prep_file.py  results/$prefix/cache.out
python3 scripts/prep_file.py  results/$prefix/ipc.out
rm -rf results/$prefix/*.out
