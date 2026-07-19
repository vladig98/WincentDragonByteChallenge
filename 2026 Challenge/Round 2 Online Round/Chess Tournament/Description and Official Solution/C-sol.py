import sys

def max_draws(n):
    # works also for even n
    min_non_draws = sum( abs(x-n//2) for x in range(n) )

    k = n // 2
    return (3 * k**2 + k) // 2

def set_result(M, u, v, res):
    M[u][v] = res
    M[v][u] = {'W': 'L', 'L': 'W', 'D': 'D'}[res]

def rotate_triangle(M, b):
    # find and rotate one triangle among the first b players
    for u in range(b - 1, -1, -1):
        for v in range(u - 1, -1, -1):
            if M[u][v] != 'W':
                continue
            for z in range(v + 1, u):
                if M[u][z] == 'D' and M[z][v] == 'D':
                    set_result(M, u, v, 'D')
                    set_result(M, u, z, 'W')
                    set_result(M, z, v, 'W')
                    return
    assert False

def build(n, d):
    if d > max_draws(n): return None
    b = 1
    while True:
        M = [['X'] * n for _ in range(n)]
        c = (b + 1) // 2
        for u in range(n):
            for v in range(u):
                set_result(M, u, v, 'W' if u >= b or u - v >= c else 'D')
        draws = sum( M[u][v] == 'D' for u in range(n) for v in range(n) ) // 2
        if draws >= d: break
        b += 1
    for _ in range(draws-d): rotate_triangle(M, b)
    return M

def solve_one():
    n, d = [ int(_) for _ in input().split() ]
    M = build(n, d)
    if M:
        print('YES')
        for row in M: print(''.join(row))
    else:
        print('NO')

t = int( input() )
for tc in range(t): solve_one()

