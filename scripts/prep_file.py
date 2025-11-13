#!/usr/bin/env python3
import sys, re

args = sys.argv[1:]
if len(args) < 1:
    print("Usage: prep_file.py <infile> [outfile]", file=sys.stderr)
    sys.exit(1)

in_path = args[0]
out_path = args[1] if len(args) > 1 else None

# Detect file type from filename
is_cache = 'cache' in in_path.lower()
is_ipc = 'ipc' in in_path.lower()

def parse_line(line, is_cache, is_ipc):
    """Extract the right metric from each line"""
    s = line.strip()
    
    # Skip header/footer lines
    if (not s or s.startswith('#') or 
        'Performance counter stats' in s or
        'seconds time elapsed' in s or
        'task-clock' in s):
        return None
    
    # Remove thousand separators (both German dots and commas)
    # Replace German decimal commas with dots
    s = s.replace(',', '.')  # German decimal comma → dot
    
    if is_ipc:
        # For IPC: Find "X.XX insn per cycle" pattern
        match = re.search(r'#\s*([0-9]+\.?[0-9]*)\s+insn per cycle', s)
        if match:
            return float(match.group(1))
    
    elif is_cache:
        # For cache: Extract the large number (2nd column after timestamp)
        # Pattern: timestamp  BIG_NUMBER  event_name
        parts = s.split()
        if len(parts) >= 2:
            # Second element should be the cache access count
            num_str = parts[1].replace('.', '')  # Remove thousand separators
            try:
                return int(num_str)
            except ValueError:
                pass
    
    return None

# Process file
results = []
with open(in_path, 'r') as f:
    for line in f:
        value = parse_line(line, is_cache, is_ipc)
        if value is not None:
            results.append(value)

# Write output
if out_path:
    with open(out_path, 'w') as f:
        for val in results:
            f.write(f"{val}\n")
else:
    for val in results:
        print(val)
