from itertools import combinations

def binomial(n, k):
    answer = 1
    for i in range(1, k+1): answer = (answer * (n+1-i)) // i
    return answer

def solve_one():
    n, k, p = [ int(_) for _ in input().split() ]
    if 2*k <= n:
        d = binomial(n-1, k-1)
        print(d)
        if p:
            for opt in combinations(range(n-1), k-1):
                print(*opt, n-1)
    else:
        d = binomial(n, k)
        print(d)
        if p:
            for opt in combinations(range(n), k):
                print(*opt)

t = int(input())
for tc in range(t): solve_one()
