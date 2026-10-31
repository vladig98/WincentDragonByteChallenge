#!/usr/bin/env python3
"""Checker for the cranes problem.

    checker.py <subtaskid> <input> <correct.out> <contestant.out>

Exit code 0 = OK (stdout: score, then "OK"), 1 = WA (stdout: one message),
2 = internal error (jury data broken).
"""
import sys

POINTS = {'P1': 14, 'P2': 20, 'P3': 33, 'P4': 33}


def block_sizes(C):
    out, s = [], 1
    for c in C:
        if c == 'C':
            s += 1
        else:
            out.append(s)
            s = 1
    out.append(s)
    return out


def value(C, n, m):
    if m == 1:
        return 0
    if m == 2:
        return sum(s * (s - 1) // 2 for s in block_sizes(C))
    return sum(5 * (s * (s - 1) * (s - 2) // 6) + s * (s - 1) // 2 * (n - s)
               for s in block_sizes(C))


def wa(msg):
    print(msg)
    sys.exit(1)


def ie(msg):
    print(msg)
    sys.exit(2)


def read_cases(path):
    try:
        with open(path, encoding='ascii') as f:
            content = f.read()
        if '\x00' in content:
            raise UnicodeDecodeError('ascii', b'', 0, 1, 'null byte in text')
        data = content.split()
    except UnicodeDecodeError:
        ie(f'Cannot read {path}: not valid ASCII text.')
    except OSError as e:
        ie(f'Cannot read {path}: {e}')
    try:
        t = int(data[0])
        pos, cases = 1, []
        for _ in range(t):
            n, b, m = int(data[pos]), int(data[pos + 1]), int(data[pos + 2])
            C = data[pos + 3]
            pos += 4
            cases.append((n, b, m, C))
    except (IndexError, ValueError) as e:
        ie(f'Malformed input file {path}: {e}')
    return cases


def read_answer(path, t, who):
    try:
        with open(path, encoding='ascii') as f:
            content = f.read()
        if '\x00' in content:
            raise UnicodeDecodeError('ascii', b'', 0, 1, 'null byte in text')
        lines = content.split()
    except UnicodeDecodeError:
        (ie if who == 'jury' else wa)(f'Cannot read {who} output: not valid ASCII text.')
    except OSError as e:
        (ie if who == 'jury' else wa)(f'Cannot read {who} output: {e}')
    if len(lines) != t:
        (ie if who == 'jury' else wa)(
            f'Expected {t} lines in the {who} output, found {len(lines)}.')
    return lines


def check_one(idx, n, b, m, C, ans, who):
    fail = ie if who == 'jury' else wa
    where = f'Test case {idx + 1}: '
    if len(ans) != n - 1:
        fail(where + f'expected a string of length {n - 1}, got {len(ans)}.')
    if any(c not in 'C-' for c in ans):
        fail(where + 'the configuration may only contain the characters C and -.')
    added = 0
    for i in range(n - 1):
        if C[i] == 'C' and ans[i] != 'C':
            fail(where + f'crane at position {i} was removed.')
        if C[i] == '-' and ans[i] == 'C':
            added += 1
    if added > b:
        fail(where + f'{added} cranes installed, the budget is {b}.')


def main():
    if len(sys.argv) != 5:
        ie(f'usage: {sys.argv[0]} <subtaskid> <input> <correct.out> <contestant.out>')
    sub, inp, jury_path, cont_path = sys.argv[1:5]
    if sub not in POINTS:
        ie(f'Unknown subtask id {sub!r}.')
    cases = read_cases(inp)
    jury = read_answer(jury_path, len(cases), 'jury')
    cont = read_answer(cont_path, len(cases), 'contestant')

    for i, (n, b, m, C) in enumerate(cases):
        check_one(i, n, b, m, C, jury[i], 'jury')
        check_one(i, n, b, m, C, cont[i], 'contestant')
        vj, vc = value(jury[i], n, m), value(cont[i], n, m)
        if vc > vj:
            ie(f'Test case {i + 1}: internal error, please contact organizers.')
        if vc < vj:
            #wa(f'Test case {i + 1}: solution not optimal.')
            wa(f'Wrong answer.')

    print(POINTS[sub])
    print('OK')


if __name__ == '__main__':
    main()
