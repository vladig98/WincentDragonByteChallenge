#!/usr/bin/python3
import sys

MAX_DIGITS = 50

def dcnt(a):
    return sum([int(c) for c in str(a)])

# you can use skap/cskap or assert
def skap(msg: str, exit_code: int = 1):
    print(msg)
    exit(exit_code)


def cskap(cond: bool, msg: str, exit_code: int = 1):
    if not cond:
        skap(msg, exit_code)

cskap(
    len(sys.argv) == 4,
    "Arguments should be <input> <target_output> <contestant_output>",
    2,
)
_, fn_inp, fn_target, fn_contestant = sys.argv

def parse_solution(fn: str, t : int):
    res = []
    try:
        with open(fn) as f:
            lines = f.readlines()
            cskap(len(lines) == t, "Solution output has {} lines but {} were expected".format(len(lines), t))
            for i in range(t):
                res.append(lines[i].split())
    except Exception:
        skap("Invalid solution format")
    return res

with open(fn_inp) as f:
    t = int(f.readline())
    sol_target = parse_solution(fn_target, t)
    sol_contestant = parse_solution(fn_contestant, t)
    for i in range(t):
        d = int(f.readline())
        cskap(len(sol_target[i]) == len(sol_contestant[i]), "test #{}: wrong solution".format(i))
        if sol_target[i][0] != "NONE":
            try:
                cskap(max(len(sol_contestant[i][0]), len(sol_contestant[i][1])) <= MAX_DIGITS, "test #{}: too long numbers".format(i))
                a, b = map(int, sol_contestant[i])
                cskap(dcnt(a) == dcnt(b), "test #{}: not the same sum of digits".format(i))
                cskap(abs(a - b) == d, "test #{}: not the target difference".format(i))
            except Exception:
                skap("test #{}: Invalid solution format: not a number".format(i))
        else:
            cskap(sol_contestant[i][0] == "NONE", "test #{}: wrong solution".format(i))
        
skap("OK", 0)
