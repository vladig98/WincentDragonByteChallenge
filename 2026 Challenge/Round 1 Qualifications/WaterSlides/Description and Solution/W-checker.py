from sys import argv, stdin, stderr

def verbose(msg): print(msg, file=stderr)

assert len(argv)==5, f'correct usage: python3 {argv[0]} input contestants_output max_points partial_points'

max_points = int( argv[3] )
partial_points = int( argv[4] )

INPUTS = []
OUTPUTS = []

try:
    infile = open(argv[1],'r')
    try:
        outfile = open(argv[2],'r')
        outlines = outfile.readlines()
    except:
        assert False, 'Failed to parse the output (probably not a valid text file).'

    assert len(outlines) > 0, 'Empty output.'

    T = int( infile.readline() )
    for tc in range(T):
        N, K = [ int(_) for _ in infile.readline().strip().split() ]
        H = [ int(_) for _ in infile.readline().strip().split() ]
        assert len(outlines) >= K, 'Too few lines of output.'
        current, outlines = outlines[:K], outlines[K:]
        try:
            edges = [ [ int(_) for _ in line.split() ] for line in current ]
        except:
            assert False, 'Failed to parse the edges as integer lists.'
        for edge in edges:
            assert len(edge) == 2, 'Each line of output must have two integers.'
        for edge in edges:
            assert all( -1 <= x < N for x in edge ), 'Invalid value in output.'

        has_minus1 = False
        for edge in edges:
            if -1 in edge:
                has_minus1 = True
        assert not has_minus1, 'Incorrect claim whether a test is solvable.'

        INPUTS.append( (N, K, H) )
        OUTPUTS.append( edges )

    assert len(outlines) == 0, 'Too many lines of output.'
except Exception as error_message:
    print(error_message)
    exit(1)

try:
    for inp, edges in zip( INPUTS, OUTPUTS ):
        N, K, H = inp
        sortedH = list(sorted(H))
        best_quality = sum( sortedH[-K:] ) - sum( sortedH[:K] )

        for x, y in edges:
            assert H[x] > H[y], 'A slide is not going downhill.' # catches self-loops as well

        seen = set()
        for x, y in edges:
            assert x not in seen, 'Some slides share an endpoint.'
            assert y not in seen, 'Some slides share an endpoint.'
            seen.add(x)
            seen.add(y)

        events = []
        for eid, edge in enumerate(edges):
            events.append( (edge[0], eid) )
            events.append( (edge[1], eid) )
        events.sort()

        seen = [ False for _ in range(K) ]
        stack = []

        for where, eid in events:
            if seen[eid]:
                # we have already seen the other endpoint, it must be on top of the stack
                assert stack[-1] == eid, 'Some slides must cross.'
                stack.pop()
            else:
                stack.append(eid)
                seen[eid] = True

        their_quality = 0
        for x, y in edges:
            their_quality += H[x] - H[y]

        assert their_quality >= best_quality, 'Solution quality not optimal.'

except Exception as error_message:
    if partial_points > 0:
        print(partial_points)
        print(f'Partially OK ({error_message})')
        exit(0)
    else:
        print(error_message)
        exit(1)

print(max_points)
print('OK')
exit(0)

