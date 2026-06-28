from math import isfinite
from sys import argv, stdin, stderr

VERBOSE = False
STRICT = False # len pre interne testy

MAX_Y = 500
MAX_DIGITS = 50

def is_reshuffle(former, latter):
    df = [ x for x in str(former) if x != '0' ]
    dl = [ x for x in str(latter) if x != '0' ]
    return list(sorted(df)) == list(sorted(dl))

def get_area(seq):
    if len(seq) <= 1: return 0
    length, width = 0, 0
    for i in range(len(seq)-1):
        if is_reshuffle( seq[i], seq[i+1] ): continue
        length += 1
        width = max(width, abs( seq[i] - seq[i+1] ))
    if VERBOSE: print(f's = {s} length = {length} width = {width}')
    return length * width

if __name__ == '__main__':
    assert len(argv)==5, f'correct usage: python3 {argv[0]} max_score input correct_output contestants_output'

    max_score = int(argv[1])

    infile = open(argv[2],'r')
    T = int( infile.readline() )
    TESTS = []
    for t in range(T):
        a, b = [ int(_) for _ in infile.readline().split() ]
        TESTS.append( (a, b, 1) )
    
    corrfile = open(argv[3],'r')
    corrlines = corrfile.readlines()
    CORRECT_AREAS = [ int( corrlines[3*t] ) for t in range(T) ]

    try:
        outfile = open(argv[4],'r')
        outlines = outfile.readlines()
    except:
        print('Failed to parse the output (probably not a valid text file).')
        exit(1)

    partial = False

    try:
        assert len(outlines) == 3*T, 'Output must have exactly three lines per test case.'
        try:
            their_areas = [ int(outlines[3*t]) for t in range(T) ]
        except:
            assert False, 'Failed to parse x.'

        for our, their in zip(CORRECT_AREAS, their_areas):
            if their > our:
                assert False, 'Incorrect x.'

        if all( our == their for our, their in zip(CORRECT_AREAS, their_areas) ):
            partial = True

        for t in range(T):
            a, b, p = TESTS[t]
            if p == 0:
                assert CORRECT_AREAS[t] == their_areas[t], 'Incorrect x.'
                continue
            
            try:
                y = int( outlines[3*t+1] )
            except:
                assert False, 'Failed to parse y.'
            assert 0 <= y <= MAX_Y, 'Value of y outside allowed range.'

            s = outlines[3*t+2].split()
            assert len(s) == y+1, 'Third line must have exactly y+1 tokens.'
            try:
                s = [ int(_) for _ in s ]
            except:
                assert False, 'All elements of s must be integers.'
            assert all( ss > 0 for ss in s ), 'All elements of s must be positive integers.'
            assert all( ss < 10**MAX_DIGITS for ss in s ), f'All elements of s must have at most {MAX_DIGITS} digits.'
            assert a == s[0], 's[0] does not equal a'
            assert b == s[y], 's[y] does not equal b'
            assert their_areas[t] == get_area(s), 'Area of s[] does not match the declared x.'
            
            assert CORRECT_AREAS[t] == their_areas[t], 'Internal error, contact organizers.'

    except Exception as error_message:
        if partial:
            print( max_score // 2 )
            print(f'All areas OK, then WA: {error_message}')
            if STRICT: exit(1)
            exit(0)
        else:
            print(error_message)
            exit(1)

    print(max_score)
    print('OK')
    exit(0)

