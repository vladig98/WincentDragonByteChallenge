from sys import argv

try:
    assert len(argv)==4, 'Internal error: provide arguments [subtask] [correct.out] [their.out]'
    subtask = argv[1]
    assert subtask in [ 'R1', 'R2' ]
    our_tokens = open(argv[2]).read().split()
    try:
        their_tokens = open(argv[3]).read().split()
    except:
        assert False, 'Failed to parse your output (not a valid text file?)'
    assert len(our_tokens) == len(their_tokens), 'Wrong number of tokens in your output file.'
    assert all( x in ['riffle','uniform'] for x in their_tokens ), 'Wrong token (must be "riffle" or "uniform").'
except AssertionError as error_message:
    print(error_message)
    exit(1)

good = sum( x==y for x,y in zip(our_tokens,their_tokens) )

if subtask == 'R1' and good < 99:
    print('Too few correct answers.')
    exit(1)

if subtask == 'R2' and good < 195:
    print('Too few correct answers.')
    exit(1)

print(50)
print('OK')
exit(0)

