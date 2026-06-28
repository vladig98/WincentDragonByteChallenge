
def solve_one():
    n, k = [ int(_) for _ in input().split() ]
    H = [ int(_) for _ in input().split() ]
    sortedH = list(sorted(H))
    small = set(sortedH[:k])
    large = set(sortedH[-k:])
    edges = []
    stack_small, stack_large = [], []
    for i in range(n):
        is_small, is_large = H[i] in small, H[i] in large
        if (not is_small) and (not is_large): continue
        if is_small:
            if stack_large == []:
                stack_small.append(i)
            else:
                j = stack_large.pop()
                edges.append( (j, i) )
        if is_large:
            if stack_small == []:
                stack_large.append(i)
            else:
                j = stack_small.pop()
                edges.append( (i, j) )
    assert len(edges) == k
    assert stack_small == [] and stack_large == []
    for edge in edges:
        print(*edge)

t = int( input() )
for tc in range(t): solve_one()

