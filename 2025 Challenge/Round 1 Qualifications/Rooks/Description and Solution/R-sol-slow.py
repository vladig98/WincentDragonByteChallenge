
def generate_moves(d, rooks, color):
    for r, c in rooks:
        if rooks[(r, c)] != color: continue
        for dr, dc in [ (1, 0), (-1, 0), (0, 1), (0, -1) ]:
            for steps in range(1, d):
                nr, nc = r + steps*dr, c + steps*dc
                if not (0 <= nr < d and 0 <= nc < d): break
                if (nr, nc) in rooks and rooks[(nr, nc)] == color: break
                yield (r, c, nr, nc)
                if (nr, nc) in rooks: break

def apply_move(rooks, move):
    pr, pc, nr, nc = move
    color = rooks[(pr, pc)]
    new_rooks = { (r, c): rooks[(r, c)] for r, c in rooks if (r, c) not in [ (pr, pc), (nr, nc) ] }
    new_rooks[ (nr, nc) ] = color
    return new_rooks

def solve():
    D, N = [ int(_) for _ in input().split() ]
    start_rooks = {}
    for n in range(N):
        r, c, f = input().split()
        r, c = int(r), int(c)
        start_rooks[ (r, c) ] = f
    answer = 0
    for m1 in generate_moves(D, start_rooks, 'W'):
        mid_rooks = apply_move(start_rooks, m1)
        for m2 in generate_moves(D, mid_rooks, 'B'):
            answer += 1
    print(answer)

T = int(input())
for t in range(T): solve()


