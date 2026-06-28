import random
import sys
sys.set_int_max_str_digits(10**5)

MOD = 10**9 + 7

cached_n = 10**19999 - 1
cached_answer = None

def count_steps_faster(n):
    global cached_n, cached_answer
    answer = 0
    while True:
        strn = str(n)
        d = len(strn)
        if d == 19999 and cached_n == n and cached_answer is not None: return (answer + cached_answer) % MOD
        if d == 1: return (answer + 1) % MOD
        f = int( strn[0] )
        threshold = f * 10**(d-1)
        steps = ((n - threshold) // f) + 1
        n -= steps * f
        answer += steps
        answer %= MOD

t = int( input() )
for tc in range(t):
    n = int( input() )
    if n > cached_n and cached_answer is None:
        cached_answer = count_steps_faster(cached_n)
    print( count_steps_faster(n) )

