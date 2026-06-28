def solve():
    d = int(input())
    if d % 9 == 0:
        k = d // 9
        print(k, 10 * k)
    else:
        print("NONE")

T = int(input())
for i in range(T):
    solve()
