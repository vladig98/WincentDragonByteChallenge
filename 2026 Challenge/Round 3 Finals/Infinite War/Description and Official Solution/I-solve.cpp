// Usage: ./solve [seconds] [seed] [outfile] [warm_start_solution] < in
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <queue>
#include <climits>
#include <cstdio>
#include <cstdlib>

using namespace std;

struct Deck {
    vector<unsigned short> buf; int head = 0, sz = 0, cap = 0;
    void init(const vector<int> &v, int c) {
        cap = c; buf.assign(c, 0); head = 0; sz = 0;
        for (int x : v) push(x);
    }
    inline int at(int i) const { int j = head + i; if (j >= cap) j -= cap; return buf[j]; }
    inline void pop() { if (++head == cap) head = 0; sz--; }
    inline void push(int v) { int j = head + sz; if (j >= cap) j -= cap; buf[j] = (unsigned short)v; sz++; }
};

struct Test { vector<int> g, d; int n; };

inline vector<Test> readInput(istream &in) {
    int t; in >> t;
    vector<Test> ts(t);
    for (auto &c : ts) {
        int g, d; in >> g >> d;
        c.g.resize(g); c.d.resize(d);
        for (int &x : c.g) in >> x;
        for (int &x : c.d) in >> x;
        c.n = g + d;
    }
    return ts;
}

inline string toHex(const vector<unsigned char> &bits) {
    string s;
    int lead = (int)bits.size() % 4;
    size_t i = 0;
    if (lead) {
        int v = 0;
        for (int j = 0; j < lead; j++) v = v * 2 + bits[i++];
        s += "0123456789abcdef"[v];
    }
    while (i < bits.size()) {
        int v = 0;
        for (int j = 0; j < 4; j++) v = v * 2 + bits[i++];
        s += "0123456789abcdef"[v];
    }
    size_t p = s.find_first_not_of('0');
    if (p == string::npos) return "0";
    return s.substr(p);
}

inline bool fromHex(long long k, const string &hex, vector<unsigned char> &bits) {
    bits.clear();
    for (char ch : hex) {
        int v;
        if (ch >= '0' && ch <= '9') v = ch - '0';
        else if (ch >= 'a' && ch <= 'f') v = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F') v = ch - 'A' + 10;
        else return false;
        for (int j = 3; j >= 0; j--) bits.push_back((unsigned char)((v >> j) & 1));
    }
    if ((long long)bits.size() < k) bits.insert(bits.begin(), k - bits.size(), 0);
    if ((long long)bits.size() > k) {
        auto end = bits.begin() + (bits.size() - k);
        if (find(bits.begin(), end, 1) != end) return false;
        bits.erase(bits.begin(), end);
    }
    return true;
}

inline void writeOutput(ostream &out, const vector<vector<unsigned char>> &ans) {
    for (const auto &b : ans) out << b.size() << ' ' << toHex(b) << '\n';
}

inline void applyMove(Deck &A, Deck &B, bool aWins, int hi, int lo, int ord) {
    A.pop(); B.pop();
    Deck &w = aWins ? A : B;
    w.push(ord ? hi : lo); w.push(ord ? lo : hi);
}

inline int blockRule(const Deck &win, const Deck &opp, bool winnerIsW, int hi, int lo) {
    int sg = win.sz, sp = opp.sz;
    if (sg >= sp) return 0;
    bool k2 = sg + 1 < sp;
    int u1 = opp.at(sg), u2 = k2 ? opp.at(sg + 1) : -1;
    int bestOrd = -1, bl = 0, bm = 0, bs = 0;
    for (int ord = 0; ord < 2; ord++) {
        int f = ord ? hi : lo, g = ord ? lo : hi;
        int lw = 0, mk = -1, sk = 0;
        if (winnerIsW ? (u1 > f) : (f > u1)) { lw++; mk = max(mk, max(f, u1)); sk += f + u1; }
        if (k2 && (winnerIsW ? (u2 > g) : (g > u2))) { lw++; mk = max(mk, max(g, u2)); sk += g + u2; }
        if (bestOrd < 0 || lw < bl || (lw == bl && (mk < bm || (mk == bm && sk < bs))))
            { bestOrd = ord; bl = lw; bm = mk; bs = sk; }
    }
    return bestOrd;
}

struct Solver {
    int n; bool AisW; long long HARD, BEST;
    double noiseBits = 6.0;
    mt19937 rng;
    vector<unsigned char> path, bestSeq;

    Solver(unsigned seed = 12345) : rng(seed) {}

    void init(const Test &c) {
        n = c.n;
        AisW = find(c.g.begin(), c.g.end(), n) != c.g.end();
        HARD = 400LL * n + 20000;
        BEST = HARD;
        path.clear(); bestSeq.clear();
    }

    long long playBase(Deck A, Deck B, bool noisy) {
        size_t base = path.size();
        unsigned thr = (unsigned)(min(1.0, noiseBits / (double)max(1LL, BEST)) * 4294967296.0);
        while (A.sz && B.sz && (long long)path.size() < min(HARD, BEST)) {
            int x = A.at(0), y = B.at(0);
            bool aWins = x > y;
            int hi = x > y ? x : y, lo = x > y ? y : x;
            int ord = (noisy && rng() < thr) ? (int)(rng() & 1)
                    : blockRule(aWins ? A : B, aWins ? B : A, aWins == AisW, hi, lo);
            path.push_back((unsigned char)ord);
            applyMove(A, B, aWins, hi, lo, ord);
        }
        long long t = (long long)(path.size() - base);
        bool done = !(A.sz && B.sz);
        if (done && (long long)path.size() < BEST) { BEST = path.size(); bestSeq = path; }
        path.resize(base);
        return done ? t : -1;
    }

    template <class Inner>
    long long improve(Deck A, Deck B, bool noisy, Inner inner) {
        size_t base = path.size();
        while (A.sz && B.sz && (long long)path.size() < min(HARD, BEST)) {
            int x = A.at(0), y = B.at(0);
            bool aWins = x > y;
            int hi = x > y ? x : y, lo = x > y ? y : x;
            long long best = LLONG_MAX; int bestOrd = 0;
            for (int ord = 0; ord < 2; ord++) {
                Deck A2 = A, B2 = B;
                applyMove(A2, B2, aWins, hi, lo, ord);
                path.push_back((unsigned char)ord);
                long long r;
                if (!A2.sz || !B2.sz) {
                    r = 0;
                    if ((long long)path.size() < BEST) { BEST = path.size(); bestSeq = path; }
                } else r = inner(A2, B2, noisy);
                path.pop_back();
                if (r >= 0 && r < best) { best = r; bestOrd = ord; }
            }
            path.push_back((unsigned char)bestOrd);
            applyMove(A, B, aWins, hi, lo, bestOrd);
        }
        long long t = (long long)(path.size() - base);
        bool done = !(A.sz && B.sz);
        if (done && (long long)path.size() < BEST) { BEST = path.size(); bestSeq = path; }
        path.resize(base);
        return done ? t : -1;
    }

    long long roll(Deck A, Deck B, bool noisy) {
        return improve(A, B, noisy, [&](Deck &a, Deck &b, bool nz) { return playBase(a, b, nz); });
    }
    long long nest(Deck A, Deck B, bool noisy) {
        return improve(A, B, noisy, [&](Deck &a, Deck &b, bool nz) { return roll(a, b, nz); });
    }
};

inline void makeDecks(const Test &c, Deck &A, Deck &B) {
    A.init(c.g, c.n + 4);
    B.init(c.d, c.n + 4);
}

int main(int argc, char **argv) {
    double budget = argc > 1 ? atof(argv[1]) : 600.0;
    unsigned seed = argc > 2 ? atoi(argv[2]) : 12345;
    string outPath = argc > 3 ? argv[3] : "";
    const char *warmPath = argc > 4 ? argv[4] : nullptr;
    auto t0 = chrono::steady_clock::now();
    auto elapsed = [&] { return chrono::duration<double>(chrono::steady_clock::now() - t0).count(); };

    vector<Test> ts = readInput(cin);
    Solver S(seed);
    int m = (int)ts.size();
    vector<vector<unsigned char>> ans(m);
    vector<int> att(m, 0);

    int cur_test = -1;
    if (warmPath) {
        ifstream warm(warmPath);
        long long k; string hex;
        for (int i = 0; i < m && (warm >> k >> hex); i++)
            if (!fromHex(k, hex, ans[i])) { fprintf(stderr, "bad warm start at test %d\n", i); return 1; }
        for (int i = 0; i < m; i++)
            if (ans[i].empty()) { fprintf(stderr, "warm start covers only %d tests\n", i); return 1; }
    } else {
        size_t cur_max = 0;
        for (int i = 0; i < m; i++) {
            cur_test = i;
            printf("switching to test %d\n", i);
            fflush(stdout);
            Deck A, B; makeDecks(ts[i], A, B);
            S.init(ts[i]);
            S.roll(A, B, false);
            ans[i] = S.bestSeq;
            cur_max = max(cur_max, ans[i].size());
            printf("test %d: improvement found: initial solution length %zu, max k = %zu\n", i, ans[i].size(), cur_max);
            fflush(stdout);
        }
    }
    fprintf(stderr, "pass 1 done, max k = %zu, %.1fs\n",
            max_element(ans.begin(), ans.end(),
                        [](const vector<unsigned char> &p, const vector<unsigned char> &q)
                        { return p.size() < q.size(); })->size(), elapsed());
    fflush(stderr);

    auto dump = [&] {
        if (outPath.empty()) return;
        ofstream f(outPath + ".tmp");
        writeOutput(f, ans);
        f.close();
        rename((outPath + ".tmp").c_str(), outPath.c_str());
    };
    dump();
    double lastDump = elapsed();

    priority_queue<pair<long long, int>> pq;
    for (int i = 0; i < m; i++) pq.push({(long long)ans[i].size(), i});
    long long improved = 0, tries = 0;
    while (!pq.empty() && elapsed() < budget) {
        auto [k, i] = pq.top(); pq.pop();
        if ((long long)ans[i].size() != k) continue;

        if (i != cur_test) {
            cur_test = i;
            printf("switching to test %d\n", i);
            fflush(stdout);
        }

        while (elapsed() < budget) {
            att[i]++; tries++;
            Deck A, B; makeDecks(ts[i], A, B);
            S.init(ts[i]);
            S.BEST = k;
            S.bestSeq = ans[i];
            bool deep = (double)k * k * k < 5e11;
            if (deep && (att[i] & 3) == 1) S.nest(A, B, true);
            else                           S.roll(A, B, true);
            if (S.BEST < k) {
                ans[i] = S.bestSeq;
                improved++;
                size_t cur_max = 0;
                for (const auto &b : ans) cur_max = max(cur_max, b.size());
                printf("test %d: improvement found: %lld -> %zu, max k = %zu\n", i, k, ans[i].size(), cur_max);
                fflush(stdout);
                break;
            }

            if (elapsed() - lastDump > 30.0) {
                dump(); lastDump = elapsed();
                size_t cur = 0;
                for (auto &b : ans) cur = max(cur, b.size());
                fprintf(stderr, "  %.0fs  tries=%lld improved=%lld  max k = %zu\n",
                        elapsed(), tries, improved, cur);
                fflush(stderr);
            }
        }
        pq.push({(long long)ans[i].size(), i});

        if (elapsed() - lastDump > 30.0) {
            dump(); lastDump = elapsed();
            size_t cur = 0;
            for (auto &b : ans) cur = max(cur, b.size());
            fprintf(stderr, "  %.0fs  tries=%lld improved=%lld  max k = %zu\n",
                    elapsed(), tries, improved, cur);
            fflush(stderr);
        }
    }
    size_t mx = 0;
    for (auto &b : ans) mx = max(mx, b.size());
    fprintf(stderr, "refined: %lld tries, %lld improvements, max k = %zu, %.1fs\n",
            tries, improved, mx, elapsed());
    if (outPath.empty()) writeOutput(cout, ans); else dump();
    return 0;
}
