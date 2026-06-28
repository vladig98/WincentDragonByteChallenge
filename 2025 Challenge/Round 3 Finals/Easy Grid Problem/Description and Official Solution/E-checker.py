from sys import argv, stdin, stderr

DEBUG = True

assert len(argv)==3, f'correct usage: python3 {argv[0]} input contestants_output'

def process_testcase(N):
    global their_output, next_line
    if next_line == len(their_output): 
        if DEBUG: print('Too few lines of output.', file=stderr)
        return False

    verdict, next_line = their_output[next_line].strip(), next_line+1
    if verdict not in ['POSSIBLE', 'IMPOSSIBLE']:
        if DEBUG: print(f'Invalid verdict {verdict}.', file=stderr)
        return False

    correct_verdict = 'POSSIBLE' if N >= 5 else 'IMPOSSIBLE'
    if verdict != correct_verdict:
        if DEBUG: print(f'Incorrect verdict.', file=stderr)
        return False

    if verdict == 'IMPOSSIBLE':
        if DEBUG: print(f'Answer for {N} OK.', file=stderr)
        return True

    if next_line + N > len(their_output):
        if DEBUG: print('Too few lines of output.', file=stderr)
        return False

    A = [ [ x for x in their_output[next_line+i].strip() ] for i in range(N) ]
    next_line += N

    if not all(len(row) == N for row in A):
        if DEBUG: print('Not a square.', file=stderr)
        return False

    if not all( x in '01' for row in A for x in row ):
        if DEBUG: print('Invalid character in matrix.', file=stderr)
        return False

    entries = []

    for row in A:
        entries.append( ''.join(row) )
        if row.count('1') not in [ N//2, (N+1)//2 ]:
            if DEBUG: print('A row is not balanced.', file=stderr)
            return False

    for c in range(N):
        col = [ A[r][c] for r in range(N) ]
        entries.append( ''.join(col) )
        if col.count('1') not in [ N//2, (N+1)//2 ]:
            if DEBUG: print('A column is not balanced.', file=stderr)
            return False

    if any('000' in line for line in entries):
        if DEBUG: print('Three 0s in a row.', file=stderr)
        return False

    if any('111' in line for line in entries):
        if DEBUG: print('Three 1s in a row.', file=stderr)
        return False

    if len(set(entries)) != len(entries):
        if DEBUG: print('Not all lines are distinct.', file=stderr)
        return False

    if DEBUG: print(f'Answer for {N} OK.', file=stderr)
    return True


input_file = open(argv[1],'r')
TC = int( input_file.readline() )
Ns = [ int( input_file.readline() ) for t in range(TC) ]

solved = [ False for t in range(TC) ]

try:
    try:
        their_output = open(argv[2],'r').readlines()
        next_line = 0
    except:
        assert False, 'Failed to parse output (not ASCII text).'

except Exception as error_message:
    print(error_message)
    exit(1)

for t in range(TC):
    solved[t] = process_testcase( Ns[t] )
    #if not solved[t]: break

score = 0
if all( correct for correct, N in zip(solved, Ns) if N == 7 ): score = 9
if all( correct for correct, N in zip(solved, Ns) if N <= 9 ): score = 15
if all( correct for correct, N in zip(solved, Ns) if N <= 13 ): score = 24
if all( correct for correct, N in zip(solved, Ns) if N <= 40 ): score = 62
if all( correct for correct, N in zip(solved, Ns) ): score = 100

if score == 0:
    print('Wrong answer')
    exit(1)

if score < 100:
    print(score)
    print('Partially OK')
    exit(0)

print(score)
print('OK')
exit(0)

