from collections import deque

def solve_one():
    mod = 10**9 + 7
    n, m = [ int(_) for _ in input().split() ]
    G = [ [] for _ in range(n) ]
    for _ in range(m):
        x, y = [ int(_) for _ in input().split() ]
        G[x].append(y)
        G[y].append(x)

    for u in range(n): G[u].sort()

    # run an ordinary BFS first

    dist, par = [-1]*n, [-1]*n
    dist[0] = 0
    order = [0]
    Q = deque([0])
    while Q:
        u = Q.popleft()
        for v in G[u]:
            if dist[v] == -1:
                dist[v] = dist[u] + 1
                par[v] = u
                order.append(v)
                Q.append(v)

    D = max(dist)
    if D == 0: return 0

    pos = [-1]*n
    for i, v in enumerate(order): pos[v] = i
    v_last = max((par[t] for t in range(n) if dist[t] == D), key=lambda u: pos[u])

    w = []
    cur = v_last
    while cur != -1:
        w.append(cur)
        cur = par[cur]
    w.reverse()

    y = [1]*n
    total = 0
    f = 0
    for k in range(D-1):
        total += y[0]
        i = D-2-k
        # count walks that share the first i steps but then continue to a smaller-numbered neighbor
        f += sum( y[u] for u in G[w[i]] if u < w[i+1] )
        # extend path counts by one more step
        y = [ sum( y[v] for v in G[u] ) % mod for u in range(n) ]

    return (total + f + 1) % mod

t = int( input() )
for tc in range(t): print( solve_one() )
