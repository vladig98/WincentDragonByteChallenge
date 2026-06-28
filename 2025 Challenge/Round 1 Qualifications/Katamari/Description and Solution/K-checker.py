from math import hypot
from sys import argv, stdin, stderr

MAX_SCORE_PER_TEST = 20.

def verbose(msg):
    print(msg, file=stderr)

def get_max_final_weight(W, orig_items):
    N = len(orig_items)
    items = [ orig_items[n] + [n] for n in range(N) ]
    items.sort(key = lambda x: x[2])
    total = W
    for n in range(N):
        if 2*items[n][2] < total:
            total += items[n][2]
    return total

if __name__ == '__main__':
    assert len(argv)==4, f'correct usage: python3 {argv[0]} input tsp contestants_output'

    infile = open(argv[1],'r')
    tspfile = open(argv[2],'r')
    try:
        outfile = open(argv[3],'r')
        outlines = outfile.readlines()
    except:
        print('Failed to parse the output (probably not a valid text file).')
        exit(1)

    total_score = 0.
    individual_verdicts = []

    TC = int( infile.readline() )
    for tc in range(TC):
        N, W = [ int(_) for _ in infile.readline().split() ]
        Kopt = int( tspfile.readline() )
        Topt = float( tspfile.readline() )
        items = [ [ int(_) for _ in infile.readline().split() ] for n in range(N) ]

        if len(outlines) < 2*tc+2:
            individual_verdicts.append('none')
            verbose(f'test {tc}: not enough lines in the output file')
            continue

        try:
            K = int(outlines[2*tc])
        except:
            individual_verdicts.append('nok')
            verbose(f'test {tc}: failed to parse k')
            continue

        try:
            seq = [ int(_) for _ in outlines[2*tc+1].split() ]
        except:
            individual_verdicts.append('noit')
            verbose(f'test {tc}: failed to parse the sequence of items')
            continue

        if len(seq) != K or min(seq) < 0 or max(seq) >= N or len(seq) != len(set(seq)):
            individual_verdicts.append('badit')
            verbose(f'test {tc}: sequence must contain k distinct valid items')
            continue

        x, y, w = 0, 0, W
        total_dist = 0.
        valid, bad_item, bad_w = True, None, None
        for item in seq:
            ix, iy, iw = items[item]
            total_dist += hypot( x-ix, y-iy )
            if valid and not (w > 2*iw): valid, bad_item, bad_w = False, item, w
            x, y, w = ix, iy, w+iw

        if not valid:
            individual_verdicts.append('toobig')
            verbose(f'test {tc}: item {bad_item} weight {items[bad_item][2]} too heavy for ball weight {bad_w}')
            continue

        optw = get_max_final_weight(W, items)

        if w < optw:
            individual_verdicts.append('left')
            verbose(f'test {tc}: final ball is not as heavy as possible')
            continue

        if w > optw:
            individual_verdicts.append('ERR')
            verbose(f'test {tc}: INTERNAL ERROR final ball is heavier than should have been possible')
            continue

        total_dist += K # time to pick up items

        if total_dist > 4*Topt:
            score = 0.
        else:
            fraction = min(1., (4*Topt - total_dist) / (3*Topt))
            score = MAX_SCORE_PER_TEST * fraction

        individual_verdicts.append(f'{score:.2f}')
        verbose(f'test {tc}: OK {score:.2f} (t_opt {Topt:.2f} your time {total_dist:.2f})')
        total_score += score

    print(f'{total_score:.2f}')
    msgs = '+'.join(individual_verdicts)
    print(f'OK ({msgs})')

