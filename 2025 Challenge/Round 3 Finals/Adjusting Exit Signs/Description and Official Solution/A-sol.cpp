#include<bits/stdc++.h>

using namespace std;

#define FOR(i,n)	for(int i=0;i<(int)n;i++)
#define MP(x,y)	make_pair((x),(y))
#define ii pair<int, int>
#define lli long long int
#define lili pair<lli, lli>
#define SIZE(x) int(x.size())


struct real_edge {
    int from;
    int to;
    int cost;
    
    const bool operator<(const real_edge &A) const {
        return cost < A.cost;
    }
};


struct p_queue {
    // supports merge and removing edges by the minimum
    
    int rank;
    int lazy; // edges are subtracted by their minimum
    real_edge edge;
    p_queue *first, *second;
    
    p_queue(int lz, real_edge _edge, p_queue *_first, p_queue *_second) {
        edge = _edge;
        
        if (_first == NULL && _second == NULL) {
            lazy = 0;
        }
        else {
            lazy = lz;
        }
        first = _first, second = _second;
        rank = 1 + min(first == NULL ? 0 : first -> rank, second == NULL ? 0 : second -> rank);
    }
    
    p_queue *add_lazy(int lz) {
        return new p_queue(lz + lazy, {edge.from, edge.to, edge.cost - lz}, first, second);
    }
    
    p_queue *propagate_lazy() {
        return new p_queue(0, edge, (first == NULL ? first : first -> add_lazy(lazy)), second == NULL ? second : second -> add_lazy(lazy));
    }
};

int get_rank(p_queue *Q) {
    return (Q == NULL ? 0 : Q -> rank);
}

p_queue *from_array(vector<real_edge> &edges) {
    sort(edges.begin(), edges.end());
    // can create a list-like pqueue, all operations will still take O(log n)
    p_queue *current = NULL;
    for (int i = SIZE(edges) - 1; i >= 0; i --) {
        p_queue *next = new p_queue(0, edges[i], current, NULL);
        current = next;
    }
    return current;
}

p_queue *merge(p_queue *A, p_queue *B) {
    if (A == NULL) return B;
    if (B == NULL) return A;
    if (A -> edge < B -> edge) swap(A, B);
    
    if (B -> lazy > 0) {
        B = B -> propagate_lazy();
    }
    
    if (get_rank(B -> first) < get_rank(B -> second)) return new p_queue(0, B -> edge, merge(B -> first, A), B -> second);
    return new p_queue(0, B -> edge, B -> first, merge(A, B -> second));
}

pair<p_queue *, real_edge> erase_min(p_queue *A) {
    // assumes A != NULL
    if (A -> lazy) A = A -> propagate_lazy();
    return {merge(A -> first, A -> second), A -> edge};
}

struct answer {
    lli res;
    vector<int> pointing_to;
};

// UF-like structure
int find_target(int v, vector<int> &leads_to) {
    if (leads_to[v] == v) return v;
    leads_to[v] = find_target(leads_to[v], leads_to);
    return leads_to[v];
}

answer solve_absorbence(int n, int s, vector<vector<ii> > &E) {
    int N = n;
    lli res = 0;
    
    queue<int> to_process;
    vector<int> leads_to, in_cycle;
    vector<p_queue *> Qs(n, NULL);
    FOR(i, n) {
        leads_to.push_back(i);
        in_cycle.push_back(i);
        if (i != s) {
            to_process.push(i);
            vector<real_edge> edges;
            for (ii w : E[i]) {
                edges.push_back({i, w.first, w.second});
            }
            
            Qs[i] = from_array(edges);
        }
    }
    
    
    vector<pair<int, real_edge> > points_to(n, {-1, {-1, -1, -1}});
    vector<vector<int> > cycle_contents(n);
    vector<int> in_cycle_direct(n, -1);
    FOR(i, n) cycle_contents[i] = {i};
    
    // while to_process non-empty; otherwise we reached selected_edges being an s-absorbence
    while (!to_process.empty()) {
        int v = to_process.front();
        to_process.pop();
        
        // two possibilities
        // We strip all the in-cycle edges previously
        
        auto popped = erase_min(Qs[v]);
        Qs[v] = popped.first;
        real_edge e = popped.second;
        // either this is a valid edge, or otherwise this is an obsolete edge
        if (find_target(e.to, in_cycle) == find_target(e.from, in_cycle)) {
            popped = erase_min(Qs[v]);
            Qs[v] = popped.first;
            e = popped.second;
        }
        // adjust 
        res += (lli)e.cost;
        Qs[v] = (Qs[v] == NULL ? Qs[v] : Qs[v] -> add_lazy(e.cost)); 
        
        // we now (re)set the e.to -> e.from in the result
        points_to[v] = {find_target(e.to, in_cycle), e};
        
        // we have: from should not lead anywhere, o/w we would not re-processed it, it is going from the cycle
        int real_from = find_target(e.from, in_cycle);
        int real_to = find_target(e.to, leads_to); // we really care about where to leads to
        // Case 1: we did not create a cycle
        if (real_to != real_from) {
            leads_to[real_from] = real_to; // re-attach UF-style
        }
        else { // Case 2: we created a cycle; interesting things happen
            // first we need to collect all vertices on the cycle
            vector<int> cycle_detected;
            cycle_detected.push_back(v);
            while (cycle_detected[0] != find_target(points_to[cycle_detected.back()].first, in_cycle)) {
                int next = find_target(points_to[cycle_detected.back()].first, in_cycle);
                cycle_detected.push_back(next);
            }
            
            points_to.push_back({-1, {-1,-1,-1}});
            Qs.push_back(NULL);
            in_cycle.push_back(N);
            leads_to.push_back(N);
            in_cycle_direct.push_back(-1);
            cycle_contents.push_back(cycle_detected);
            for (int w : cycle_detected) {
                in_cycle[w] = N;
                // by the choice of w, should have w = find_target(w, in_cycle);
                in_cycle_direct[w] = N;
                leads_to[w] = N;
                Qs[N] = merge(Qs[N], Qs[w]);
            }
            to_process.push(N);
            N ++;
        }
    }
   
    // next need to process which edges are selected
    vector<int> selected_edges(n, -1);
    
    vector<int> already_taken(N, -1);
    
    for (int i = N - 1; i >= 0; i --) {
        if (i == s) continue;
        // we now unravel the cycles from here
     
        // if already_taken[i] == -1, then we are not on cycle-edge; If i a cycle, we unravel it
        if (already_taken[i] == -1) {
            selected_edges[points_to[i].second.from] = points_to[i].second.to;
            
            // now we go from points_to[i].second.from up until we reach i, this vertex is an offender
            int v_taken = points_to[i].second.from;
            already_taken[v_taken] = v_taken;
            while (in_cycle_direct[v_taken] != -1 && in_cycle_direct[v_taken] != i) {
                already_taken[in_cycle_direct[v_taken]] = v_taken;
                v_taken = in_cycle_direct[v_taken];
            }
            
            // now process this cycle
            if (cycle_contents[i].size() > 1) { // must be a cycle to unravel
                for (int v : cycle_contents[i]) {
                    if (v == v_taken) continue;
                    selected_edges[points_to[v].second.from] = points_to[v].second.to;
                    int w_taken = points_to[v].second.from; // found a direct offender, will mark all where it takes an edge
                    already_taken[w_taken] = w_taken; // do not process this vertex again
                    while (v != w_taken && in_cycle_direct[w_taken] != -1 && in_cycle_direct[w_taken] != v) {
                        already_taken[in_cycle_direct[w_taken]] = w_taken;
                        w_taken = in_cycle_direct[w_taken];
                    }
                    already_taken[v] = w_taken;
                }
            }
        }
        else { // now can process the ones marked in above process
            if (cycle_contents[i].size() > 1) {
                for (int v : cycle_contents[i]) {
                    if (v == already_taken[i]) continue;
                    selected_edges[points_to[v].second.from] = points_to[v].second.to;
                    int w_taken = points_to[v].second.from; // found a direct offender, will mark all where it takes an edge
                    already_taken[w_taken] = w_taken; // do not process this vertex again
                    while (v != w_taken && in_cycle_direct[w_taken] != -1 && in_cycle_direct[w_taken] != v) {
                        already_taken[in_cycle_direct[w_taken]] = w_taken;
                        w_taken = in_cycle_direct[w_taken];
                    }
                }
            }
        }
    }
    
    return {res, selected_edges};
}

bool in_bounds(int x, int y, int n, int m) {
    return (x >= 0 && x < n && y >= 0 && y < m);
}

ii next(int x, int y, char d) {
    if (d == '<') return {x, y - 1};
    if (d == '>') return {x, y + 1};
    if (d == '^') return {x - 1, y};
    return {x + 1, y};
}   

void solve() {
    int n, m;
    cin >> n >> m;
    vector<string> pointers(n);
    vector<vector<bool> > moveable(n);
    vector<vector<int> > ID(n);
    
    FOR(i, n) {
        cin >> pointers[i];
    }
    
    FOR(i, n) {
        FOR(j, m) {
            int a;
            cin >> a;
            moveable[i].push_back(a);
        }
    }
    FOR(i, n) FOR(j, m) ID[i].push_back(i * m + j);
    int N = n * m + 1;
    vector<vector<ii> > edges(N);
    vector<unordered_map<int, char> > reedge(N);
    
    string dirs = "<>^v";
    
    FOR(i, n) {
        FOR(j, m) {
            if (moveable[i][j]) {
                for (char c : dirs) {
                    ii p = next(i, j, c);
                    if (in_bounds(p.first, p.second, n, m)) {
                        edges[ID[i][j]].push_back({ID[p.first][p.second], c != pointers[i][j]});
                        reedge[ID[i][j]][ID[p.first][p.second]] = c;
                    }
                    else {
                        edges[ID[i][j]].push_back({N - 1, c != pointers[i][j]});
                        if (c == pointers[i][j] || reedge[ID[i][j]].count(N - 1) == 0) reedge[ID[i][j]][N - 1] = c;
                    }
                }
            }
            else {
                ii p = next(i, j, pointers[i][j]);
                if (in_bounds(p.first, p.second, n, m)) edges[ID[i][j]].push_back({ID[p.first][p.second], 0});
                else edges[ID[i][j]].push_back({N - 1, 0});
            }
        }
    }
    
    
    auto answer = solve_absorbence(N, N - 1, edges);
    cout << answer.res << endl;
    vector<vector<char> > orient(n, vector<char>(m, '-'));
    
    FOR(i, n) {
        FOR(j, m) {
            char c = (moveable[i][j] ? reedge[ID[i][j]][answer.pointing_to[ID[i][j]]] : pointers[i][j]);
            cout << c;
        }
        cout << "\n";
    }
    
}

int main() {
    cin.sync_with_stdio(false);
    cout.sync_with_stdio(false);
    
    int t;
    cin >> t;
    FOR(i, t) solve();
    
}
