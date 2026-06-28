from math import isfinite
from sys import argv, stdin, stderr

VERBOSE = False
STRICT = False # len pre interne testy

if __name__ == '__main__':
    assert len(argv)==5, f'correct usage: python3 {argv[0]} max_score input correct_output contestants_output'

    max_score = int(argv[1])

    infile = open(argv[2],'r')
    T = int( infile.readline() )
    TESTS = []
    for t in range(T):
        a, b = [ int(_) for _ in infile.readline().split() ]
        TESTS.append( (a, b) )
    
    corrfile = open(argv[3],'r')
    corrlines = corrfile.readlines()
    CORRECT_AREAS = [ int( corrlines[t] ) for t in range(T) ]

    try:
        outfile = open(argv[4],'r')
        outlines = outfile.readlines()
    except:
        print('Failed to parse the output (probably not a valid text file).')
        exit(1)

    try:
        assert len(outlines) == T, 'Output must have exactly one line per test case.'
        try:
            their_areas = [ int(outlines[t]) for t in range(T) ]
        except:
            assert False, 'Failed to parse x.'

        for our, their in zip(CORRECT_AREAS, their_areas):
            if their != our:
                assert False, 'Incorrect x.'

    except Exception as error_message:
        print(error_message)
        exit(1)

    print(max_score)
    print('OK')
    exit(0)

