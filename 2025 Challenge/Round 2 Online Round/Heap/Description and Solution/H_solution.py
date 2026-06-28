
def solve_one():
    N = int( input() )
    count_bigger, count_smaller = [0]*N, [0]*N
    for n in range(1, N):
        count_bigger[n] = 1 + count_bigger[ (n-1)//2 ]
    for n in reversed(range(N)):
        if 2*n+1 < N: count_smaller[n] += 1 + count_smaller[2*n+1]
        if 2*n+2 < N: count_smaller[n] += 1 + count_smaller[2*n+2]
    # for each node we now know how many other nodes must be bigger/smaller
    # it can be shown that any other value is acceptable in this node
    delta = [0]*(N+2)
    for n in range(N):
        minv, maxv = 1 + count_smaller[n], N - count_bigger[n]
        delta[minv] += 1
        delta[maxv+1] -= 1
    answer = 0
    allowed_nodes = 0
    for n in range(1, N+1):
        allowed_nodes += delta[n]
        answer += n * allowed_nodes
    print(answer)

if __name__ == '__main__':
    TC = int( input() )
    for tc in range(TC):
        solve_one()

