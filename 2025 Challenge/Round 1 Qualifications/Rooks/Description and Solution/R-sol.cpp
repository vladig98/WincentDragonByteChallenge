#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

const long long MOD = 1000000007;

struct rook { int r, c; bool white; };
ostream& operator<< (ostream &out, const rook &A) { return out << (A.white?"W@":"B@") << A.r << "," << A.c; }
bool comp_rc(const rook &A, const rook &B) { if (A.r != B.r) return A.r < B.r; return A.c < B.c; }

struct idrook { int r, c, id; bool white; };
bool comp_cr(const idrook &A, const idrook &B) { if (A.c != B.c) return A.c < B.c; return A.r < B.r; }

struct event { int r, r0, type, c, id; };
bool operator<(const event &A, const event &B) { if (A.r != B.r) return A.r < B.r; return A.type < B.type; }

long long D;
int N;
vector<rook> INPUT;

struct Fenwick1D {
    int size;
    vector<long long> T;
    Fenwick1D(int maxval) { size = 1; while (size < maxval) size <<= 1; T.clear(); T.resize(size+1,0); }
    void update(int x, long long delta) { while (x <= size) { T[x] += delta; x += x & -x; } }
    long long sum(int x1, int x2) { long long res=0; --x1; while (x2) { res += T[x2]; x2 -= x2 & -x2; } while (x1) { res -= T[x1]; x1 -= x1 & -x1; } return res; }
};

long long count_white_left_black_horizontal() {
    long long answer = 0;

    // precompute information on neighboring rooks
    sort( INPUT.begin(), INPUT.end(), comp_rc );

    long long all_black_moves = 0;
    vector<long long> my_moves(N);
    vector<bool> black_left(N, false), black_right(N, false);
    for (int n=0; n<N; ++n) {
        int leftmost = 0, rightmost = D-1;
        if (n > 0 && INPUT[n-1].r == INPUT[n].r) {
            leftmost = INPUT[n-1].c + int( INPUT[n-1].white == INPUT[n].white );
            if (INPUT[n].white && !INPUT[n-1].white) black_left[n] = true;
        }
        if (n+1 < N && INPUT[n+1].r == INPUT[n].r) {
            rightmost = INPUT[n+1].c - int( INPUT[n+1].white == INPUT[n].white );
            if (INPUT[n].white && !INPUT[n+1].white) black_right[n] = true;
        }
        if (INPUT[n].white) {
            rightmost = INPUT[n].c - 1;
            my_moves[n] = rightmost - leftmost + 1;
        } else {
            my_moves[n] = rightmost - leftmost;
            all_black_moves += my_moves[n];
        }
    }
    all_black_moves %= MOD;

    // iterate over white rooks and count moves where this one moves left
    for (int n=0; n<N; ++n) if (INPUT[n].white) {
        long long curr_black_moves = all_black_moves;
        if (black_left[n]) curr_black_moves -= my_moves[n-1];
        if (black_right[n]) curr_black_moves -= my_moves[n+1];

        // I move and then an independent black rook moves
        answer += my_moves[n] * curr_black_moves; 
        
        // I move away from the black rook on my right
        if (black_right[n]) {
            answer += my_moves[n+1] * my_moves[n];
            answer += my_moves[n] * (my_moves[n]+1) / 2;
        }
        
        // I move towards the black rook on my left
        if (black_left[n]) {
            answer += my_moves[n-1] * (my_moves[n] - 1);
            answer -= my_moves[n] * (my_moves[n]-1) / 2;
        }
        
        // I actually take the black rook on my left, and then another black rook beyond that takes me
        if (black_left[n] && n-2 >= 0 && INPUT[n-2].r == INPUT[n].r && !INPUT[n-2].white) ++answer;
        answer %= MOD;
    }
    answer += MOD;
    answer %= MOD;
    return answer;
}

long long count_white_left_black_up() {
    long long answer = 0;
    
    // precompute information on neighboring rooks and intervals that correspond to vertical movement of black rooks
    sort( INPUT.begin(), INPUT.end(), comp_rc );

    vector<idrook> IDINPUT;
    for (int n=0; n<N; ++n) IDINPUT.push_back( { INPUT[n].r, INPUT[n].c, n, INPUT[n].white } );
    sort( IDINPUT.begin(), IDINPUT.end(), comp_cr );

    vector<int> good_columns;
    vector<bool> another_black_under(N, false);
    vector<event> events;
    long long all_black_moves = 0;
    vector<long long> my_moves(N, 0);
    vector<long long> black_extra(N, 0);
    for (int n=0; n<N; ++n) {
        if (good_columns.empty() || good_columns.back() < IDINPUT[n].c) good_columns.push_back( IDINPUT[n].c );
            
        if (n+1 < N && IDINPUT[n].c == IDINPUT[n+1].c) {
            if (!IDINPUT[n].white && !IDINPUT[n+1].white) another_black_under[ IDINPUT[n].id ] = true;
            if (IDINPUT[n].white && !IDINPUT[n+1].white) {
                int lo = 0;
                if (n-1 >= 0 && IDINPUT[n-1].c == IDINPUT[n].c) {
                    lo = IDINPUT[n-1].r;
                    if (!IDINPUT[n-1].white) ++lo;
                }
                black_extra[ IDINPUT[n].id ] = IDINPUT[n].r - lo;
            }
        }

        if (!IDINPUT[n].white) {
            int lo = 0;
            if (n-1 >= 0 && IDINPUT[n-1].c == IDINPUT[n].c) { lo = IDINPUT[n-1].r; if (!IDINPUT[n-1].white) ++lo; }
            if (lo < IDINPUT[n].r) {
                int mm = IDINPUT[n].r - lo;
                my_moves[ IDINPUT[n].id ] = mm;
                all_black_moves += mm;
                events.push_back( {lo, lo, +1, (int)good_columns.size(), IDINPUT[n].id } );
                events.push_back( {IDINPUT[n].r, lo, -1, (int)good_columns.size(), IDINPUT[n].id } );
            }
        }
    }
    all_black_moves %= MOD;
    
    // inititalize trees to count how many alive rooks we have and to sum the top cells they can reach
    Fenwick1D count_alive(N+7), sum_top(N+7);

    // sort all events that correspond to black rooks
    sort( events.begin(), events.end() );
    events.push_back( {(int)D, (int)D, 0, -1, -1} );
    int e = 0;

    // iterate over white rooks from top to bottom, count what happens when this white rook goes left and some black rook goes up
    for (int n=0; n<N; ++n) if (INPUT[n].white) {
        // process all events until the current row
        while (events[e].r <= INPUT[n].r) {
            int cid = events[e].c;
            count_alive.update(cid,events[e].type);
            sum_top.update(cid,events[e].type*events[e].r0);
            ++e;
        }

        // determine how far left this white rook goes
        int lo = 0;
        if (n > 0 && INPUT[n-1].r == INPUT[n].r) lo = INPUT[n-1].c + 1;
        long long wmoves = INPUT[n].c - lo;
        bool can_eat = (n > 0 && INPUT[n-1].r == INPUT[n].r && !INPUT[n-1].white);
        
        // this white rook might take a black rook, if it does, that rook cannot move
        if (can_eat) {
            answer += all_black_moves - my_moves[n-1];
            answer += black_extra[n];
            if (another_black_under[n-1]) ++answer; // another black rook may now take
        }

        // find out how many conflicting columns we have and what the conflicts look like
        int clo = lower_bound( good_columns.begin(), good_columns.end(), lo ) - good_columns.begin();
        int chi = lower_bound( good_columns.begin(), good_columns.end(), INPUT[n].c ) - good_columns.begin();
        long long conflicts = count_alive.sum(clo+1, chi);
        long long tops = sum_top.sum(clo+1, chi) % MOD;
        
        // count all moves that would be there without interference
        answer += wmoves * all_black_moves;
        
        // subtract the impossible moves due to white rook being in the way
        answer -= conflicts * INPUT[n].r - tops;

        // add the extra moves gained by a black rook below our white, if any
        answer += wmoves * black_extra[n];
        answer %= MOD;
    }
    answer %= MOD;
    return answer;
}

void solve() {
    cin >> D >> N;
    INPUT.clear(); INPUT.resize(N);
    string tmp;
    for (int n=0; n<N; ++n) { cin >> INPUT[n].r >> INPUT[n].c >> tmp; INPUT[n].white = (tmp[0] == 'W'); }

    // iterate over all symmetries of the board and count
    long long answer = 0;
    for (int transpose=0; transpose<2; ++transpose) {
        for (int flipv=0; flipv<2; ++flipv) {
            answer += count_white_left_black_horizontal();
            for (int fliph=0; fliph<2; ++fliph) {
                answer += count_white_left_black_up();
                for (int n=0; n<N; ++n) INPUT[n].r = D-1-INPUT[n].r;
            }
            for (int n=0; n<N; ++n) INPUT[n].c = D-1-INPUT[n].c;
        }
        for (int n=0; n<N; ++n) swap( INPUT[n].r, INPUT[n].c );
    }
    answer %= MOD; answer += MOD; answer %= MOD;
    cout << answer << endl;
}

int main() { 
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    int T; cin >> T; while (T--) solve();
}
