// Solution for the whole of D8.in = [ (4,10), (5,11), (6,12), (4,14),
//                                     (4,20), (4,40), (4,62) ].
//
// Every case is solved by a Steiner system S(n-1, n, m): for the seen set
// T = {h1, ..., h_{n-1}} announce the unique point of the unique block that
// contains T.  This is a fully order-independent strategy.  Fixing n-2 of the
// coordinates, the blocks through those n-2 points partition the m-n+2
// remaining points into pairs, so f restricted to a line is a fixed-point-free
// involution -- exactly Lemma 1 (i) + (ii).
//
// The seven systems:
//
//   (4,10)  S(3,4,10), the unique Steiner quadruple system on 10 points,
//           hardcoded as 30 quadruples of digits.
//
//   (6,12)  S(5,6,12), the small Witt design: the orbit of {inf} u QR(11) under
//           PSL(2,11) acting on F_11 u {inf}.  The stabiliser of that hexad
//           contains x -> 3x, of order 5, so the orbit has 660/5 = 132 = C(12,5)/6
//           blocks.
//
//   (5,11)  S(4,5,11), the design derived from S(5,6,12) at the point inf: the
//           blocks through inf with inf deleted, 132*6/12 = 66 = C(11,4)/5 of them.
//
//   (4,14)  S(3,4,14).  14 = 2*7 with 7 inadmissible, so doubling does not apply,
//           and no cyclic or 1-rotational S(3,4,14) exists (exhaustive search).
//           Instead 7 base blocks are developed under the order-21 group
//           { (c, r) -> (c, a*r + b) : a in <2>, b in Z_7 } acting on
//           Z_7 x {0,1}, point 7c + r.  Orbits are short where a base block is
//           fixed by a in {2,4}; the 7 orbits together have 91 = C(14,3)/4 blocks.
//
//   (4,20)  S(3,4,20) = Hanani doubling of S(3,4,10).
//   (4,40)  S(3,4,40) = Hanani doubling applied twice.
//           Doubling an S(3,4,v) on {0,...,v-1} gives, on {0,...,2v-1} with x and
//           x+v the two copies of x, the blocks B and B+v for every block B, plus
//           {x, y, z+v, w+v} for every 1-factor F of a 1-factorization of K_v and
//           every ordered pair of (not necessarily distinct) edges {x,y}, {z,w}
//           of F.  Below F runs over the round-robin factorization (point v-1
//           fixed, the others taken mod v-1).  A triple inside one copy lies in a
//           unique block of the first kind; a triple {x, y, z+v} determines the
//           factor F containing {x,y} and then the edge of F through z, and
//           symmetrically for the other 2+1 split.
//           Counts: 2*30 + 9*5*5 = 285 and 2*285 + 19*10*10 = 2470.
//
//   (4,62)  S(3,4,62).  62 = 2*31 with 31 inadmissible, so doubling does not
//           apply.  31 base blocks are developed under the order-305 group
//           { x -> a*x + b : a in <9>, b in F_61 } on F_61 u {inf} (9 has order 5
//           mod 61, inf is fixed).  A nonidentity element fixes at most inf and
//           b/(1-a), so it stabilises no 3- or 4-subset: all orbits have full
//           length 305, giving 124 triple orbits and 9455/305 = 31 block orbits.
//
// The base-block tables for (4,14) and (4,62) were found by exact cover over the
// triple orbits of the respective group.
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>

using namespace std;

typedef vector<int> Block;
typedef vector<Block> Design;

static Design sqs10() {
    static const char T[30][5] = {
        "0123", "0145", "0167", "0189", "0246", "0258", "0279", "0349", "0357", "0368",
        "0478", "0569", "1247", "1259", "1268", "1348", "1356", "1379", "1469", "1578",
        "2345", "2369", "2378", "2489", "2567", "3467", "3589", "4568", "4579", "6789"
    };
    Design S;
    for (int i = 0; i < 30; ++i) {
        Block b(4);
        for (int j = 0; j < 4; ++j) b[j] = T[i][j] - '0';
        S.push_back(b);
    }
    return S;
}

static Design doubled(const Design &S, int v) {
    Design R;
    for (const Block &b : S) {
        R.push_back(b);
        Block c = b;
        for (int &x : c) x += v;
        R.push_back(c);
    }
    int q = v - 1;
    for (int k = 0; k < q; ++k) {
        vector<pair<int, int>> e;
        e.push_back({q, k});
        for (int i = 1; i < v / 2; ++i) e.push_back({(k + i) % q, (k - i + q) % q});
        for (const auto &p1 : e)
            for (const auto &p2 : e)
                R.push_back({p1.first, p1.second, p2.first + v, p2.second + v});
    }
    return R;
}

static Design sqs14() {
    static const int B[7][4] = {
        {0, 1,  2,  4}, {0, 1,  5,  9}, {0,  8,  9, 11}, {7, 8, 9, 12},
        {0, 1,  7,  8}, {0, 1, 10, 11}, {0,  1, 12, 13}
    };
    static const int ALPHA[3] = {1, 2, 4};
    set<Block> R;
    for (int t = 0; t < 3; ++t)
        for (int b = 0; b < 7; ++b)
            for (int i = 0; i < 7; ++i) {
                Block c(4);
                for (int j = 0; j < 4; ++j) {
                    int x = B[i][j];
                    c[j] = 7 * (x / 7) + (ALPHA[t] * (x % 7) + b) % 7;
                }
                sort(c.begin(), c.end());
                R.insert(c);
            }
    return Design(R.begin(), R.end());
}

static Design sqs62() {
    static const int B[31][4] = {
        { 0,  1,  5, 61}, { 0,  2, 10, 61}, { 0,  1,  2,  4}, { 0,  1,  6,  7},
        { 0,  1,  8, 41}, { 0,  1, 11, 18}, { 0,  1,  9, 10}, { 0,  1, 12, 20},
        { 0,  1, 13, 24}, { 0,  1, 15, 37}, { 0,  1, 16, 23}, { 0,  1, 19, 39},
        { 0,  1, 14, 40}, { 0,  1, 32, 43}, { 0,  1, 22, 44}, { 0,  1, 46, 51},
        { 0,  1, 33, 45}, { 0,  1, 17, 48}, { 0,  1, 26, 29}, { 0,  1, 31, 49},
        { 0,  1, 36, 47}, { 0,  2,  7, 47}, { 0,  2, 12, 25}, { 0,  2,  6, 50},
        { 0,  2, 13, 37}, { 0,  2, 35, 51}, { 0,  2, 16, 30}, { 0,  2, 24, 31},
        { 0,  2, 33, 38}, { 0,  2, 14, 49}, { 0,  4, 30, 53}
    };
    const int P = 61, INF = 61;
    set<Block> R;
    int a = 1;
    for (int k = 0; k < 5; ++k) {
        for (int b = 0; b < P; ++b)
            for (int i = 0; i < 31; ++i) {
                Block c(4);
                for (int j = 0; j < 4; ++j) {
                    int x = B[i][j];
                    c[j] = (x == INF) ? INF : (a * x + b) % P;
                }
                sort(c.begin(), c.end());
                R.insert(c);
            }
        a = a * 9 % P;
    }
    return Design(R.begin(), R.end());
}

static Design witt12() {
    const int P = 11, INF = 11;
    auto mob = [&](int a, int b, int c, int d) {
        vector<int> pi(P + 1);
        for (int x = 0; x <= P; ++x) {
            int num, den;
            if (x == INF) { num = a; den = c; }
            else { num = (a * x + b) % P; den = (c * x + d) % P; }
            if (den % P == 0) pi[x] = INF;
            else {
                int iv = 1;
                while (den * iv % P != 1) ++iv;
                pi[x] = num * iv % P;
            }
        }
        return pi;
    };
    vector<vector<int>> gens = {mob(1, 1, 0, 1), mob(3, 0, 0, 1), mob(0, P - 1, 1, 0)};

    Block start = {1, 3, 4, 5, 9, INF};          // {inf} u QR(11)
    sort(start.begin(), start.end());
    set<Block> R{start};
    vector<Block> stack{start};
    while (!stack.empty()) {
        Block b = stack.back();
        stack.pop_back();
        for (const auto &g : gens) {
            Block c(b.size());
            for (size_t j = 0; j < b.size(); ++j) c[j] = g[b[j]];
            sort(c.begin(), c.end());
            if (R.insert(c).second) stack.push_back(c);
        }
    }
    return Design(R.begin(), R.end());
}

static Design derived(const Design &S, int p) {
    Design R;
    for (const Block &b : S) {
        if (find(b.begin(), b.end(), p) == b.end()) continue;
        Block c;
        for (int x : b) if (x != p) c.push_back(x);
        R.push_back(c);
    }
    return R;
}

static void emit(int n, int m, const Design &S) {
    unordered_map<unsigned long long, int> comp;
    comp.reserve(S.size() * n * 2);
    for (const Block &b : S) {
        unsigned long long full = 0;
        for (int x : b) full |= 1ULL << x;
        for (int x : b) comp[full ^ (1ULL << x)] = x;
    }

    vector<char> used(m, 0);
    bool first = true;
    auto gen = [&](auto &self, int depth, unsigned long long mask) -> void {
        if (depth == n - 1) {
            if (!first) cout << ' ';
            cout << comp[mask] + 1;
            first = false;
            return;
        }
        for (int v = 0; v < m; ++v) {
            if (used[v]) continue;
            used[v] = 1;
            self(self, depth + 1, mask | (1ULL << v));
            used[v] = 0;
        }
    };
    gen(gen, 0, 0);
    cout << '\n';
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n, m;
        cin >> n >> m;
        Design S;
        if (n == 4 && m == 10) S = sqs10();
        else if (n == 5 && m == 11) S = derived(witt12(), 11);
        else if (n == 6 && m == 12) S = witt12();
        else if (n == 4 && m == 14) S = sqs14();
        else if (n == 4 && m == 20) S = doubled(sqs10(), 10);
        else if (n == 4 && m == 40) S = doubled(doubled(sqs10(), 10), 20);
        else if (n == 4 && m == 62) S = sqs62();
        else {
            cerr << "D8: (n, m) = (" << n << ", " << m << ") is not a D8 case\n";
            return 1;
        }
        emit(n, m, S);
    }
    return 0;
}
