#!/usr/bin/env python3
import sys, bisect

def build(n, cr, cs):
    D = [1]
    while D[-1] < n:
        t = len(D)
        best = D[-1]
        for r in range(1, 121):
            cost = r * cr + cs
            if cost > t: break
            cand = (r + 1) * D[t - cost]
            if cand > best: best = cand
        D.append(best)
    return D

def die(code, msg):
    print(msg)
    sys.exit(code)

def main():
    if len(sys.argv) != 3:
        die(2, "usage: checker.py <input_file> <output_file>")
    try:
        with open(sys.argv[1]) as f: inp = f.read().split()
    except OSError as e:
        die(2, f"cannot read input file {sys.argv[1]}: {e}")
    try:
        with open(sys.argv[2]) as f: raw = f.read()
    except OSError as e:
        die(1, f"cannot read output file {sys.argv[2]}: {e}")

    it = iter(inp)
    def need(what):
        try: return int(next(it))
        except StopIteration: die(2, f"input file ended early while reading {what}")
        except ValueError: die(2, f"input file has a non-integer token while reading {what}")
    t = need("t")
    tests = [(need("n"), need("cr"), need("cs")) for _ in range(t)]

    lines = raw.replace("\r\n", "\n").replace("\r", "\n").split("\n")
    if lines and lines[-1] == "": lines.pop()
    pos = 0
    def line(what):
        nonlocal pos
        if pos >= len(lines): die(1, f"output ended early, expected a line with {what}")
        pos += 1
        return lines[pos - 1]

    for tc, (n, cr, cs) in enumerate(tests, 1):
        tag = f"test case {tc}"
        try: x = int(line(f"x ({tag})").strip())
        except ValueError: die(1, f"{tag}: expected an integer x")
        try: r = int(line(f"r ({tag})").strip())
        except ValueError: die(1, f"{tag}: expected an integer r")
        raw_sizes = line(f"rock sizes ({tag})").split()
        if x < 0: die(1, f"{tag}: x must be non-negative, got {x}")
        if r <= 0: die(1, f"{tag}: r must be positive, got {r}")
        if len(raw_sizes) != r: die(1, f"{tag}: declared r={r} but the sizes line has {len(raw_sizes)} number(s)")
        try: sizes = [int(v) for v in raw_sizes]
        except ValueError: die(1, f"{tag}: rock sizes must be integers")

        for s in sizes:
            if not 1 <= s <= n: die(1, f"{tag}: rock size {s} is out of range [1,{n}]")
        for i in range(r - 1):
            if sizes[i] >= sizes[i + 1]: die(1, f"{tag}: rock sizes must be strictly increasing")

        D = build(n, cr, cs)
        T = lambda m: bisect.bisect_left(D, m)
        bound = [0] + sizes + [n]
        achieved = r * cr + cs + max(T(bound[i + 1] - bound[i]) for i in range(len(bound) - 1))
        if x != T(n): die(1, "wrong answer given for some test case")
        if achieved > x: die(1, "wrong answer given for some test case")
        if achieved < x: die(2, "wrong answer given for some test case")

    if pos != len(lines):
        die(1, f"output has {len(lines) - pos} unexpected extra line(s) at the end")
    print("OK")

try:
    main()
except SystemExit:
    raise
except Exception:
    print("Could not parse the submission (not an ASCII text?)")
    sys.exit(1)
