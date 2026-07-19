from sys import argv

assert len(argv)==4, f'correct usage: python3 {argv[0]} input contestants_output max_points'

max_points = int( argv[3] )

POINTS = { 'W':2, 'D':1, 'L':0 }

def max_draws(n):
    k = n // 2
    return (3 * k**2 + k) // 2

INPUTS = []
OUTPUTS = []

try:
    infile = open(argv[1],'r')
    try:
        outfile = open(argv[2],'r')
        outlines = [ line.strip() for line in outfile.readlines() ]
    except:
        assert False, 'Could not parse the submission (not an ASCII text?)'

    while outlines and outlines[-1] == '':
        outlines.pop()

    assert len(outlines) > 0, 'Empty output.'

    T = int( infile.readline() )
    for tc in range(T):
        N, D = [ int(_) for _ in infile.readline().strip().split() ]

        assert len(outlines) >= 1, 'Too few lines of output.'
        verdict, outlines = outlines[0].upper(), outlines[1:]
        assert verdict in [ 'YES', 'NO' ], 'Each test case must start with "YES" or "NO".'
        assert (verdict == 'YES') == (D <= max_draws(N)), 'Incorrect claim whether a test is solvable.'

        if verdict == 'NO':
            continue

        assert len(outlines) >= N, 'Too few lines of output.'
        current, outlines = outlines[:N], outlines[N:]
        for line in current:
            assert len(line) == N, 'Each line of a tournament must have exactly N characters.'
            assert all( ch in 'WDLX' for ch in line ), 'Invalid character in output (must be "W", "D", "L" or "X").'

        INPUTS.append( (N, D) )
        OUTPUTS.append( current )

    assert len(outlines) == 0, 'Too many lines of output.'
except Exception as error_message:
    print(error_message)
    exit(1)

try:
    for inp, table in zip( INPUTS, OUTPUTS ):
        N, D = inp

        for i in range(N):
            for j in range(N):
                if i == j:
                    assert table[i][j] == 'X', 'A player must have no result against themselves.'
                else:
                    assert table[i][j] != 'X', 'A pair of distinct players has no result.'
                    assert table[i][j] + table[j][i] in [ 'WL', 'LW', 'DD' ], 'Two players disagree on the result of their game.'

        draws = sum( table[i][j] == 'D' for i in range(N) for j in range(i) )
        assert draws == D, 'Wrong number of drawn games.'

        scores = [ sum( POINTS[ table[i][j] ] for j in range(N) if j != i ) for i in range(N) ]
        assert len(set(scores)) == N, 'Two players have the same point total.'

except Exception as error_message:
    print(error_message)
    exit(1)

print(max_points)
print('OK')
exit(0)
