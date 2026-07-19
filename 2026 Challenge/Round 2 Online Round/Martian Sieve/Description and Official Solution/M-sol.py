import bisect

def solve(n, cr, cs):
    # D[t] = largest number of consecutive options we can always resolve in
    # total time at most t. Build it until D[-1] reaches n.
    D = [1]
    while D[-1] < n:
        t = len(D)
        best = D[-1]  # more time never hurts, so D stays non-decreasing
        for r in range(1, 121):
            cost = r * cr + cs
            if cost > t: break  # cost only grows with r, no point continuing
            cand = (r + 1) * D[t - cost]
            if cand > best: best = cand
        D.append(best)

    # D is non-decreasing, so T(m) is the smallest t with D[t] >= m
    def T(m): return bisect.bisect_left(D, m)

    x = T(n)
    best_time, best_r = 10**18, 1
    # r can be at most min(120, n-1): the theorem bounds it by 120, and only
    # n-1 distinct rock sizes (1..n-1) are ever useful
    for r in range(1, min(120, n-1) + 1):
        round_1_time = r * cr + cs
        if round_1_time >= best_time: break
        new_n = (n+r) // (r+1)
        total_time = round_1_time + T(new_n)
        if total_time < best_time:
            best_time, best_r = total_time, r
    r = best_r
    q, rem = divmod(n, r+1)
    parts = [q+1]*rem + [q]*(r+1-rem)
    sizes, acc = [], 0
    for i in range(r):
        acc += parts[i]
        sizes.append(acc)
    return x, sizes

t = int(input())
for tc in range(t):
    n, cr, cs = [ int(_) for _ in input().split() ]
    assert n > 1
    x, rocks = solve(n, cr, cs)
    print(x)
    print(len(rocks))
    print(*rocks)
