import sys, re

if len(sys.argv) < 3:
    print("usage: prep_file.py <infile> <outfile>")
    sys.exit(1)

in_path, out_path = sys.argv[1], sys.argv[2]
saved = []

# match integers / floats / scientific notation
num_re = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?')

with open(in_path, 'r') as f:
    for line in f:
        s = line.strip()
        # skip empty, comments, or perf headers/footers
        if (not s or s.startswith('#') or
            'Performance counter stats' in s or
            'seconds time elapsed' in s or
            'task-clock' in s or
            'cycles' in s and 'events' in s and 'raw' not in s):
            continue

        # remove thousands separators (locale)
        s = s.replace(',', '')

        m = num_re.search(s)
        if not m:
            continue
        try:
            saved.append(float(m.group(0)))
        except ValueError:
            continue

with open(out_path, 'w') as g:
    for v in saved:
        g.write(f"{v}\n")
