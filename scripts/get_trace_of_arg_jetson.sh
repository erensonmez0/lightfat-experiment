#!/bin/bash

## getting the pid of target process in argument
echo "Getting trace for $1, until finished"
# Just some preparation for the output files
if [ ! -d "results" ]
then
    mkdir results
fi
now=$(date +"%m_%d_%H_%M_%S")
prefix="${1##*/}_$now"
mkdir results/$prefix
# set the sample period in ms
sampling_period=1

# call perf to get samples
#echo "perf stat -B -I $sampling_period -e cycles,instructions,L1-dcache-loads  -o tmp.log  $1 $2"
perf stat -B -I $sampling_period -e cycles,instructions,L1-dcache-loads  -o results/tmp.log  $1 $2

#create sub-files
cat results/tmp.log | grep "cache" > results/cache_raw.log; cat results/cache_raw.log | grep -v not > results/$prefix/cache.out
cat results/tmp.log | grep "ins" > results/ipc_raw.log; cat results/ipc_raw.log | grep -v not > results/$prefix/ipc.out
# rm -rf results/*.log

python3 scripts/prep_file.py  results/$prefix/cache.out
python3 scripts/prep_file.py  results/$prefix/ipc.out
# rm -rf results/$prefix/*.out