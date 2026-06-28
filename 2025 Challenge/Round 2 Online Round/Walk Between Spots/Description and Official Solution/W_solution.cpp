#include <bits/stdc++.h>
using namespace std;

template<class T, class U> struct MincostMaxflow { // {{{
    struct edge { 
        int source, destination;
        T capacity, residue;
        U cost;
        edge(int s, int d, T cap, T res, U cost) 
            : source(s), destination(d), capacity(cap), residue(res), cost(cost) { }
    };

    MincostMaxflow(int N);
    vector< vector<int> > V;
    vector<edge> E;
    void add_arc(int source, int destination, T capacity, U cost);
    pair<T,U> get_flow(int source, int sink);
};

template<class T, class U> 
MincostMaxflow<T,U>::MincostMaxflow(int N) { V.resize(N); }

template<class T, class U> 
void MincostMaxflow<T,U>::add_arc(int source, int destination, T capacity, U cost) {
    int e = E.size();
    V[source].push_back( e );
    V[destination].push_back( e+1 );
    E.push_back( edge( source, destination, capacity, capacity, cost));
    E.push_back( edge( destination, source, capacity, 0, -cost));
}

template<class T, class U>
pair<T,U> MincostMaxflow<T,U>::get_flow(int source, int sink) {
    int N = V.size(), M = E.size();
    T flowSize = 0;
    U flowCost = 0;
    
    // automagically initialize infinities and epsilon
    // explicit initialization for ints: infinity = 1<<29, ignore epsilon (epsilon = 0)
    // explicit initialization for doubles: infinity = 1e30, epsilon = 1e-9
    T Tinfinity = 1; while (2*Tinfinity > Tinfinity) Tinfinity *= 2; 
    U Uinfinity = 1; while (2*Uinfinity > Uinfinity) Uinfinity *= 2;
    U Uepsilon = 1; for (int i=0; i<30; i++) Uepsilon /= 2;

    vector<T> flow(M,0);
    vector<U> potential(N,0);
    while (1) {
        // use dijkstra to find an augmenting path
        vector<int> from(N,-1);
        vector<U> dist(N,Uinfinity);
        
        priority_queue< pair<U,int>, vector<pair<U,int> >, greater<pair<U,int> > > Q;
        Q.push( make_pair(0,source) );
        from[source] = -2; 
        dist[source] = 0;

        while (!Q.empty()) {
            U howFar  = Q.top().first; 
            int where = Q.top().second;
            Q.pop();
            if (dist[where] < howFar) continue;

            for (int i=0; i < int(V[where].size()); i++) {
                if (E[ V[where][i] ].residue == 0) continue;
                int dest = E[ V[where][i] ].destination;
                U cost = E[ V[where][i] ].cost;
                if (dist[dest] > dist[where] + potential[where] - potential[dest] + cost + Uepsilon) {
                    dist[dest] = dist[where] + potential[where] - potential[dest] + cost;
                    from[dest] = V[where][i];
                    Q.push( make_pair( dist[dest],dest ));
                }
            }
        }

        // update vertex potentials
        for (int i=0; i<N; i++) {
            if (dist[i] == Uinfinity) potential[i] = Uinfinity;
            else if (potential[i] < Uinfinity) potential[i] += dist[i];
        }

        // if there is no path, we are done
        if (from[sink] == -1) return make_pair(flowSize,flowCost);

        // construct an augmenting path
        T canPush = Tinfinity;
        int where = sink;

        while (1) { 
            if (from[where] == -2) break;
            canPush = min(canPush, E[ from[where] ].residue );
            where = E[ from[where] ].source;
        }

        // update along the path
        where = sink;
        while (1) { 
            if (from[where] == -2) break;
            E[ from[where] ].residue -= canPush;
            E[ from[where]^1 ].residue += canPush;
            flowCost += canPush * E[ from[where] ].cost;
            where = E[ from[where] ].source;
        }
        flowSize += canPush;
    }
    return make_pair(-1,47);
} // }}}

int N;
string S;

pair<int, int> get_yz(int x, int r) {
    int z = (-x) % (r+1);
    if (z < 0) z += (r+1);
    assert( (x - z*r) % (r+1) == 0 );
    int y = (x - z*r) / (r+1);
    return {y, z};
}

int find_shortest_period(vector<int> &X, vector<int> &Y, vector<int> &Z) {
    X.clear(); 
    X.resize(256, 0);
    for (int n=0; n<N; ++n) ++X[S[n]];
    
    vector<int> nonzeroX;
    for (int x : X) if (x > 0) nonzeroX.push_back(x);

    for (int p=1; p<=N; ++p) {
        int r = N / p, s = N % p;
        
        bool all_ok = true;
        int sum_y = 0, sum_z = 0;
        for (int x : nonzeroX) {
            auto [y, z] = get_yz(x, r);
            if (y < 0) all_ok = false;
            sum_y += y;
            sum_z += z;
        }
        if (!all_ok) continue;
        if (sum_y != s || sum_z != p-s) continue;
        Y.clear();
        Y.resize(256, 0);
        Z.clear();
        Z.resize(256, 0);
        for (int i=0; i<256; ++i) if (X[i] > 0) {
            auto [y, z] = get_yz(X[i], r);
            Y[i] = y;
            Z[i] = z;
        }
        return p;
    }
    assert(false);
}

long long sum_distances(const vector<int> &left, const vector<int> &right) {
    assert( left.size() == right.size() );
    int Z = left.size();
    long long answer = 0;
    for (int z=0; z<Z; ++z) answer += abs( left[z] - right[z] );
    return answer;
}

long long match(const vector<vector<int> > &left, const vector<vector<int> > &right) {
    assert( left.size() == right.size() );
    int Z = left.size();
    MincostMaxflow<int, long long> MCMF(2*Z+2);
    for (int a=0; a<Z; ++a) for (int b=0; b<Z; ++b) MCMF.add_arc(a, Z+b, 1, sum_distances( left[a], right[b] ) );
    for (int a=0; a<Z; ++a) MCMF.add_arc( 2*Z, a, 1, 0 );
    for (int a=0; a<Z; ++a) MCMF.add_arc( Z+a, 2*Z+1, 1, 0 );
    auto [cap, cost] = MCMF.get_flow(2*Z, 2*Z+1);
    assert(cap == Z);
    return cost;
}

void solve_one() {
    cin >> N >> S;
    assert( int(S.size()) == N );
    int p;
    vector<int> X, Y, Z;
    p = find_shortest_period(X, Y, Z);
    int s = N%p;

    vector< vector<int> > letter_positions(256);
    for (int n=0; n<N; ++n) letter_positions[ S[n] ].push_back(n);
    vector< vector<int> > light_red, light_blue;
    for (int i=0; i<256; ++i) if (!letter_positions[i].empty()) {
        int y = Y[i], z = Z[i];
        vector< vector<int> > reds(y), blues(z);
        for (unsigned j=0; j<letter_positions[i].size(); ++j) {
            int k = j%(y+z);
            if (k < y) reds[k].push_back( letter_positions[i][j] ); else blues[k-y].push_back( letter_positions[i][j] );
        }
        for (auto r : reds) light_red.push_back(r);
        for (auto b : blues) light_blue.push_back(b);
    }

    long long best_cost = 1LL << 62;

    for (int offset=0; offset<=0; ++offset) {
        vector< vector<int> > dark_red(s), dark_blue(p-s);
        for (int n=0; n<N; ++n) {
            int i = n%p;
            if (i < s) dark_red[i].push_back( offset+n ); else dark_blue[i-s].push_back( offset+n );
        }
        long long cost = match(dark_red, light_red) + match(dark_blue, light_blue);
        best_cost = min( best_cost, cost );
    }
    cout << p << " " << best_cost << endl;
}

int main() {
    int TC; cin >> TC;
    while (TC--) solve_one();
}
