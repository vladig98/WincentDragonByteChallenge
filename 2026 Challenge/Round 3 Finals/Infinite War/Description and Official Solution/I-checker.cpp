// Validator and scorer.   ./checker I1.in out.txt
//
// Verifies that every answered test really ends in exactly the declared number of
// turns, then reports x (the largest k) and the score.
//
// Exit code 0 = accepted (possibly with a partial score): line 1 is the score, line 2 a
// human-readable summary.  Exit code 1 = wrong answer: a single line saying why.
// Exit code 2 = internal error (bad arguments, unreadable files, an output file over the
// size limit -- such a submission must never reach the checker).  Everything goes to stdout.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

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

inline void applyMove(Deck &A, Deck &B, bool aWins, int hi, int lo, int ord) {
    A.pop(); B.pop();
    Deck &w = aWins ? A : B;
    w.push(ord ? hi : lo); w.push(ord ? lo : hi);
}

static double X100 = 7500.0, X0 = 175000.0;
static const long long PREFIX_PENALTY = 25000000LL;
static const long long SIZE_LIMIT = 5 * 1024 * 1024;
static const long long MAX_K = 25000000LL;

static double scoreOf(double x) {
    double s = 100.0 * log(X0 / x) / log(X0 / X100);
    s = max(0.0, min(100.0, s));
    return floor(s * 100.0 + 0.5) / 100.0;
}

int main(int argc, char **argv) {
    if (argc < 3) { printf("usage: checker input output [X100] [X0]\n"); return 2; }
    if (argc > 3) X100 = atof(argv[3]);
    if (argc > 4) X0 = atof(argv[4]);
    ifstream fin(argv[1]);
    if (!fin) { printf("cannot open the input file %s\n", argv[1]); return 2; }
    vector<Test> ts = readInput(fin);
    if (!fin) { printf("the input file %s is malformed or truncated\n", argv[1]); return 2; }
    int t = (int)ts.size();

    struct stat st;
    if (stat(argv[2], &st) != 0) { printf("cannot stat the output file %s\n", argv[2]); return 2; }
    long long fsize = (long long)st.st_size;
    if (fsize > SIZE_LIMIT) {
        printf("output is %lld bytes, over the %lld byte limit\n", fsize, SIZE_LIMIT);
        return 2;
    }

    ifstream fout(argv[2]);
    if (!fout) { printf("cannot open the output file %s\n", argv[2]); return 2; }
    long long x = 0;
    int solved = 0;
    string ktok, hex;
    while (fout >> ktok) {
        if (solved == t) { printf("malformed output: too many lines of output\n"); return 1; }
        if (!(fout >> hex)) { printf("test %d: malformed output: failed to read the hex string\n", solved); return 1; }
        if (ktok.size() > 18 || ktok.find_first_not_of("0123456789") != string::npos) {
            printf("test %d: malformed output: first token is not a valid number of turns\n", solved);
            return 1;
        }
        long long k = stoll(ktok);
        const Test &c = ts[solved];
        if (k < 1) { printf("test %d: k = %lld is not positive\n", solved, k); return 1; }
        if (k > MAX_K) { printf("test %d: k = %lld is over the checker's limit of %lld turns\n", solved, k, MAX_K); return 1; }
        vector<unsigned char> bits;
        if (!fromHex(k, hex, bits)) {
            printf("test %d: failed to parse the hex string\n", solved);
            return 1;
        }
        Deck A, B; A.init(c.g, c.n + 4); B.init(c.d, c.n + 4);
        for (long long i = 0; i < k; i++) {
            if (!A.sz || !B.sz) { printf("test %d: game already over after %lld turns, k = %lld\n", solved, i, k); return 1; }
            int p = A.at(0), q = B.at(0);
            applyMove(A, B, p > q, max(p, q), min(p, q), bits[i]);
        }
        if (A.sz && B.sz) { printf("test %d: game not over after k = %lld turns\n", solved, k); return 1; }
        x = max(x, k);
        solved++;
    }
    if (solved == 0) { printf("the output answers no test case: it is empty or malformed\n"); return 1; }
    double xEff = (double)x;
    if (solved < t) xEff = max(xEff, (double)PREFIX_PENALTY / solved);
    printf("%.2f\n", scoreOf(xEff));
    printf("solved %d/%d; max k = %lld; x = %.2f; size = %lld B\n", solved, t, x, xEff, fsize);
    return 0;
}
