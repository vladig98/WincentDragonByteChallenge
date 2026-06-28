import sympy.ntheory

# Runs for roughly 500s on my machine...
# Well, it's output only and we have 2 days.

def check(n: int, base: int) -> bool:
  while n > 0:
    if n % base > 1:
      return False
    n //= base
  return True

t = int(input())
for ti in range(t):
  n = int(input())
  if n <= 1:
    print(3)
    continue
  if n == 2:
    print(-1)
    continue
  cands = set(sympy.ntheory.divisors(n)).union(sympy.ntheory.divisors(n - 1))
  cands = sorted(list(cands - set([1, 2])))

  result = -1
  for cand in cands:
    if check(n, cand):
      result = cand
      break
  print(result)