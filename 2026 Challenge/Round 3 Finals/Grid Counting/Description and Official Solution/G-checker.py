#!/usr/bin/env python3
"""Checker for the grid counting problem.

    checker.py <maxscore> <correct.out> <contestant.out>

The output has two lines, graded separately, each worth half of <maxscore>: the
integer, and the probability written as a reduced fraction numerator/denominator
with both parts positive.  Whitespace around a line and empty lines are ignored;
an output that does not have exactly two non-empty lines scores zero.

Exit code 0 = at least one line correct (stdout: score, then a message),
1 = score zero (stdout: one message), 2 = internal error (jury data broken).
"""
import re
import sys
from math import gcd


def wa(msg):
    print(msg)
    sys.exit(1)


def ie(msg):
    print(msg)
    sys.exit(2)


def read_lines(path, who):
    fail = ie if who == 'jury' else wa
    try:
        with open(path, encoding='utf-8-sig', errors='replace') as f:
            lines = [line.strip() for line in f.read().splitlines()]
    except OSError as e:
        fail(f'Cannot read {who} output: {e}')
    while lines and not lines[-1]:
        lines.pop()
    if who == 'jury':
        if len(lines) != 2:
            fail(f'Expected 2 lines in the jury output, found {len(lines)}.')
    else:
        if len(lines) == 0:
            fail('Empty contestant output.')
        if len(lines) > 2:
            fail(f'Expected at most 2 lines in the contestant output, found {len(lines)}.')
    return lines


def parse_integer(s):
    if not re.fullmatch(r'[0-9]+', s):
        return None
    try:
        return int(s)
    except ValueError:
        return None


def parse_fraction(s):
    """(numerator, denominator) if s is a reduced fraction with positive parts"""
    match = re.fullmatch(r'([0-9]+)/([0-9]+)', s)
    if not match:
        return None
    try:
        p, q = int(match.group(1)), int(match.group(2))
    except ValueError:
        return None
    if p == 0 or q == 0 or gcd(p, q) != 1:
        return None
    return p, q


def main():
    if len(sys.argv) != 4:
        ie(f'usage: {sys.argv[0]} <maxscore> <correct.out> <contestant.out>')
    try:
        maxscore = int(sys.argv[1])
    except ValueError:
        ie(f'Bad maxscore {sys.argv[1]!r}.')
    jury = read_lines(sys.argv[2], 'jury')
    cont = read_lines(sys.argv[3], 'contestant')
    if len(cont) == 1:
        if parse_fraction(cont[0]) is not None:
            cont = ['', cont[0]]
        else:
            cont = [cont[0], '']
    jint, jfrac = parse_integer(jury[0]), parse_fraction(jury[1])
    if jint is None or jfrac is None:
        ie('Malformed jury output.')

    good, msgs = 0, []
    cint = parse_integer(cont[0])
    if cint is None:
        msgs.append('line 1 is not a non-negative integer')
    elif cint == jint:
        good += 1
        msgs.append('line 1 correct')
    else:
        msgs.append('line 1 wrong')
    cfrac = parse_fraction(cont[1])
    if cfrac is None:
        msgs.append('line 2 is not a reduced fraction p/q with p, q > 0')
    elif cfrac == jfrac:
        good += 1
        msgs.append('line 2 correct')
    else:
        msgs.append('line 2 wrong')

    msg = '; '.join(msgs) + '.'
    if good == 0:
        wa(msg)
    points = maxscore * good
    print(points // 2 if points % 2 == 0 else points / 2)
    print(msg)


if __name__ == '__main__':
    main()
