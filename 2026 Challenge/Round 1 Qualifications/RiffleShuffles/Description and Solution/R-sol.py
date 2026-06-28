import string
import random
NEW_DECK = string.ascii_uppercase + string.ascii_lowercase

T = int( input() )
for tc in range(T):
    decks = [ input() for _ in range(1000) ]
    count_sx_after_x = 0
    for deck in decks:
        for i in range(len(deck)-1):
            x = deck.index( NEW_DECK[i] )
            y = deck.index( NEW_DECK[i+1] )
            if x < y:
                count_sx_after_x += 1
    if count_sx_after_x < 26400:
        print('uniform')
    else:
        print('riffle')

