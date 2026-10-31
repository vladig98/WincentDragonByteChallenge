// Checker for the distinct hats problem.
//
//     checker <subtaskid> <input> <correct.out> <contestant.out>
//
// Exit code 0 = OK (stdout: score, then "OK"),
//           1 = WA (stdout: one message, e.g. "Test case X: <reason>"),
//           2 = internal error (stdout: error message).
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

static const long long CAP = 50000000LL;
static const long long MAX_CONTESTANT_FILE_SIZE = 25 * 1024 * 1024;

const map<string, int> POINTS = {
    {"D1", 12}, {"D2", 12}, {"D3", 12}, {"D4", 12},
    {"D5", 12}, {"D6", 12}, {"D7", 12}, {"D8", 16}
};

static void wa(const string &msg) {
    cout << msg << "\n";
    exit(1);
}

static void ie(const string &msg) {
    cout << msg << "\n";
    exit(2);
}

struct Tokenizer {
    string buf;
    size_t bp = 0;

    void load(const string &path, const string &who) {
        if (who == "contestant") {
            struct stat st;
            if (stat(path.c_str(), &st) != 0) {
                wa("Cannot open contestant output: " + path);
            }
            if ((long long)st.st_size > MAX_CONTESTANT_FILE_SIZE) {
                wa("Contestant output file is too large.");
            }
        }
        ifstream f(path, ios::binary);
        if (!f) {
            if (who == "contestant") wa("Cannot open contestant output: " + path);
            else ie("Cannot open " + who + " file: " + path);
        }
        f.seekg(0, ios::end);
        size_t sz = f.tellg();
        f.seekg(0, ios::beg);
        buf.resize(sz);
        if (sz > 0) f.read(&buf[0], sz);
        bp = 0;
    }

    bool next(string &s) {
        while (bp < buf.size() && isspace((unsigned char)buf[bp])) bp++;
        if (bp >= buf.size()) return false;
        size_t st = bp;
        while (bp < buf.size() && !isspace((unsigned char)buf[bp])) bp++;
        s = buf.substr(st, bp - st);
        return true;
    }

    bool line_tokens(long long &cnt) {
        size_t p = bp;
        while (p < buf.size() && isspace((unsigned char)buf[p])) p++;
        if (p >= buf.size()) return false;
        cnt = 0;
        while (p < buf.size()) {
            while (p < buf.size() && !isspace((unsigned char)buf[p])) p++;
            cnt++;
            while (p < buf.size() && buf[p] != '\n' && isspace((unsigned char)buf[p])) p++;
            if (p >= buf.size() || buf[p] == '\n') break;
        }
        return true;
    }
};

int main(int argc, char **argv) {
    if (argc != 5) {
        ie("usage: checker <subtaskid> <input> <correct.out> <contestant.out>");
    }

    string subtask_id = argv[1];
    string input_path = argv[2];
    string jury_path = argv[3];
    string cont_path = argv[4];

    if (!POINTS.count(subtask_id)) {
        ie("Unknown subtask id: " + subtask_id);
    }
    int score = POINTS.at(subtask_id);

    ifstream fin(input_path);
    if (!fin) ie("Cannot open input file: " + input_path);
    int t;
    if (!(fin >> t)) ie("Malformed input file: " + input_path);
    vector<pair<int, int>> cases(t);
    for (int i = 0; i < t; i++) {
        if (!(fin >> cases[i].first >> cases[i].second)) {
            ie("Malformed test case in input file: " + input_path);
        }
    }

    Tokenizer cont_tok;
    cont_tok.load(cont_path, "contestant");

    Tokenizer jury_tok;
    jury_tok.load(jury_path, "jury");

    for (int c = 0; c < t; c++) {
        int n = cases[c].first;
        int m = cases[c].second;
        string where = "Test case " + to_string(c + 1) + ": ";

        bool expect_impossible = false;
        string jtok;
        if (!jury_tok.next(jtok)) ie("Jury output ended early at test case " + to_string(c + 1));
        if (jtok == "IMPOSSIBLE") {
            expect_impossible = true;
        } else {
            expect_impossible = false;
            long long num_tup = 1;
            for (int x = m - n + 2; x <= m; x++) num_tup *= x;
            for (long long k = 1; k < num_tup; k++) {
                if (!jury_tok.next(jtok)) {
                    ie("Jury output ended early inside test case " + to_string(c + 1));
                }
            }
        }

        string tok;
        long long line_cnt = 0;
        if (!cont_tok.line_tokens(line_cnt)) wa(where + "output ended early.");
        if (!cont_tok.next(tok)) wa(where + "output ended early.");

        if (tok == "IMPOSSIBLE") {
            if (expect_impossible) {
                if (line_cnt != 1) {
                    wa(where + "expected 1 token on the line, found "
                       + to_string(line_cnt) + ".");
                }
                continue;
            } else {
                wa("Wrong answer.");
            }
        } else {
            if (expect_impossible) {
                wa("Wrong answer.");
            }
        }

        long long total_cells = 1;
        for (int i = 0; i < n - 1; i++) {
            total_cells *= m;
            if (total_cells > CAP) {
                ie(where + "requires more than " + to_string(CAP) + " cells for verification grid.");
            }
        }

        long long expect_cnt = 1;
        for (int x = m - n + 2; x <= m; x++) expect_cnt *= x;
        if (line_cnt != expect_cnt) {
            wa(where + "expected " + to_string(expect_cnt) + " values on the line, found "
               + to_string(line_cnt) + ".");
        }

        vector<int> g(total_cells, 0), pw(n - 1);
        pw[n - 2] = 1;
        for (int i = n - 3; i >= 0; i--) pw[i] = pw[i + 1] * m;

        vector<char> in(m + 1, 0);
        vector<int> cur_tuple(n - 1, 1);
        bool first_tok = true;

        for (long long i = 0; i < total_cells; i++) {
            bool distinct = true;
            for (int j = 0; j < n - 1; j++) {
                if (in[cur_tuple[j]]) distinct = false;
                in[cur_tuple[j]] = 1;
            }
            if (distinct) {
                if (!first_tok) {
                    if (!cont_tok.next(tok)) wa(where + "output ended early.");
                }
                first_tok = false;

                int val = 0;
                try {
                    size_t pos;
                    val = stoi(tok, &pos);
                    if (pos != tok.size()) throw 0;
                } catch (...) {
                    wa(where + "token is not a valid integer.");
                }
                if (val < 1 || val > m) {
                    wa(where + "value " + to_string(val) + " out of range [1, " + to_string(m) + "].");
                }
                if (in[val]) {
                    wa(where + "the first person says a number they can see.");
                }
                g[i] = val;
            }
            for (int j = 0; j < n - 1; j++) in[cur_tuple[j]] = 0;

            if (i + 1 < total_cells) {
                for (int p = n - 2; p >= 0; p--) {
                    if (++cur_tuple[p] <= m) break;
                    cur_tuple[p] = 1;
                }
            }
        }

        // Line injectivity check (Derangement condition on every fiber)
        vector<int> stamp(m + 1, -1);
        int mark = 0;
        for (int i = 0; i < n - 1; i++) {
            long long rests = 1;
            for (int j = 0; j < n - 2; j++) rests *= m;
            vector<int> r(max(n - 2, 1), 1);
            for (long long c = 0; c < rests; c++) {
                bool distinct = true;
                for (int j = 0; j < n - 2; j++) {
                    if (in[r[j]]) distinct = false;
                    in[r[j]] = 1;
                }
                if (distinct) {
                    long long base = 0;
                    for (int j = 0, k = 0; j < n - 1; j++) {
                        if (j != i) base += (long long)(r[k++] - 1) * pw[j];
                    }
                    mark++;
                    for (int v = 1; v <= m; v++) {
                        if (in[v]) continue;
                        int val = g[base + (long long)(v - 1) * pw[i]];
                        if (stamp[val] == mark) {
                            wa("Wrong answer.");
                        }
                        stamp[val] = mark;
                    }
                }
                for (int j = 0; j < n - 2; j++) in[r[j]] = 0;

                if (c + 1 < rests) {
                    for (int p = n - 3; p >= 0; p--) {
                        if (++r[p] <= m) break;
                        r[p] = 1;
                    }
                }
            }
        }
    }

    string extra;
    if (cont_tok.next(extra)) {
        wa("Extra tokens beyond the expected end of the output.");
    }

    printf("%d\n", score);
    printf("OK\n");
    return 0;
}
