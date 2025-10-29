#!/usr/bin/env python3
import sys, re

# Flexible IO:
# - 2 args: in_file -> out_file
# - 1 arg:  in_file -> stdout
# - 0 args: stdin   -> stdout
args = sys.argv[1:]

def parse_stream(fin, fout):
    num_re = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?')
    for line in fin:
        s = line.strip()
        if (not s or s.startswith('#') or
            'Performance counter stats' in s or
            'seconds time elapsed' in s or
            'task-clock' in s):
            continue
        s = s.replace(',', '')
        m = num_re.search(s)
        if not m:
            continue
        try:
            v = float(m.group(0))
        except ValueError:
            continue
        fout.write(f"{v}\n")

if len(args) == 2:
    in_path, out_path = args
    with open(in_path, 'r') as fin, open(out_path, 'w') as fout:
        parse_stream(fin, fout)
elif len(args) == 1:
    in_path = args[0]
    with open(in_path, 'r') as fin:
        parse_stream(fin, sys.stdout)
elif len(args) == 0:
    parse_stream(sys.stdin, sys.stdout)
else:
    print("usage: prep_file.py <infile> <outfile>", file=sys.stderr)
    sys.exit(1)
