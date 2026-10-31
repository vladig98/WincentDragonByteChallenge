#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>
#include <cstdint>
#include <omp.h>

using namespace std;

void print128(__int128 x) {
    if (x == 0) { cout << 0; return; }
    string s;
    while (x > 0) {
        s += (char)('0' + (x % 10));
        x /= 10;
    }
    for (int i = (int)s.size() - 1; i >= 0; --i) cout << s[i];
}

long long gcd_ll(long long a, long long b) {
    while (b) {
        a %= b;
        swap(a, b);
    }
    return a;
}

namespace Solver4 {
    int row_mask[4], col_mask[4], box_mask[4], grid[4][4];
    long long count_sol = 0;

    void dfs(int r, int c) {
        if (r == 4) { count_sol++; return; }
        int nr = (c == 3) ? r + 1 : r;
        int nc = (c == 3) ? 0 : c + 1;
        if (grid[r][c] != -1) { dfs(nr, nc); return; }
        int b = (r / 2) * 2 + (c / 2);
        int used = row_mask[r] | col_mask[c] | box_mask[b];
        int avail = 0xf & ~used;
        while (avail) {
            int d = __builtin_ctz(avail);
            avail &= avail - 1;
            grid[r][c] = d;
            row_mask[r] |= (1 << d);
            col_mask[c] |= (1 << d);
            box_mask[b] |= (1 << d);
            dfs(nr, nc);
            grid[r][c] = -1;
            row_mask[r] &= ~(1 << d);
            col_mask[c] &= ~(1 << d);
            box_mask[b] &= ~(1 << d);
        }
    }

    __int128 count_total() {
        count_sol = 0;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                grid[r][c] = -1;
        row_mask[0] = col_mask[0] = box_mask[0] = (1 << 0); grid[0][0] = 0;
        row_mask[0] |= (1 << 1); col_mask[1] |= (1 << 1); box_mask[0] |= (1 << 1); grid[0][1] = 1;
        row_mask[1] |= (1 << 2); col_mask[0] |= (1 << 2); box_mask[0] |= (1 << 2); grid[1][0] = 2;
        row_mask[1] |= (1 << 3); col_mask[1] |= (1 << 3); box_mask[0] |= (1 << 3); grid[1][1] = 3;
        dfs(0, 2);
        return count_sol * 24;
    }
}

namespace Solver6 {
    int row_mask[6], col_mask[6], box_mask[6], grid[6][6];
    long long count_sol = 0;

    void dfs(int r, int c) {
        if (r == 6) { count_sol++; return; }
        int nr = (c == 5) ? r + 1 : r;
        int nc = (c == 5) ? 0 : c + 1;
        if (grid[r][c] != -1) { dfs(nr, nc); return; }
        int b = (r / 2) * 2 + (c / 3);
        int used = row_mask[r] | col_mask[c] | box_mask[b];
        int avail = 0x3f & ~used;
        while (avail) {
            int d = __builtin_ctz(avail);
            avail &= avail - 1;
            grid[r][c] = d;
            row_mask[r] |= (1 << d);
            col_mask[c] |= (1 << d);
            box_mask[b] |= (1 << d);
            dfs(nr, nc);
            grid[r][c] = -1;
            row_mask[r] &= ~(1 << d);
            col_mask[c] &= ~(1 << d);
            box_mask[b] &= ~(1 << d);
        }
    }

    __int128 count_total() {
        count_sol = 0;
        for (int r = 0; r < 6; ++r)
            for (int c = 0; c < 6; ++c)
                grid[r][c] = -1;
        for (int c = 0; c < 3; ++c) {
            grid[0][c] = c;
            row_mask[0] |= (1 << c);
            col_mask[c] |= (1 << c);
            box_mask[0] |= (1 << c);
            grid[1][c] = c + 3;
            row_mask[1] |= (1 << (c + 3));
            col_mask[c] |= (1 << (c + 3));
            box_mask[0] |= (1 << (c + 3));
        }
        dfs(0, 3);
        return count_sol * 720;
    }
}

namespace Solver8 {
    struct Assignment { int val[6]; };
    struct Cell { int r, c, b; };

    long long dfs(int grid[8][8], int row_mask[8], int col_mask[8], int box_mask[8], Cell rem_cells[64], int rem_cnt) {
        if (rem_cnt == 0) return 1;

        int min_cands = 9;
        int best_idx = -1;
        int best_avail = 0;

        for (int i = 0; i < rem_cnt; ++i) {
            int r = rem_cells[i].r;
            int c = rem_cells[i].c;
            int b = rem_cells[i].b;
            int used = row_mask[r] | col_mask[c] | box_mask[b];
            int avail = 0xff & ~used;
            int cands = __builtin_popcount(avail);
            if (cands == 0) return 0;
            if (cands < min_cands) {
                min_cands = cands;
                best_idx = i;
                best_avail = avail;
                if (min_cands == 1) break;
            }
        }

        Cell chosen = rem_cells[best_idx];
        rem_cells[best_idx] = rem_cells[rem_cnt - 1];

        int r = chosen.r, c = chosen.c, b = chosen.b;
        int avail = best_avail;
        long long total = 0;

        while (avail) {
            int d = __builtin_ctz(avail);
            avail &= avail - 1;
            grid[r][c] = d;
            row_mask[r] |= (1 << d);
            col_mask[c] |= (1 << d);
            box_mask[b] |= (1 << d);
            total += dfs(grid, row_mask, col_mask, box_mask, rem_cells, rem_cnt - 1);
            grid[r][c] = -1;
            row_mask[r] &= ~(1 << d);
            col_mask[c] &= ~(1 << d);
            box_mask[b] &= ~(1 << d);
        }

        rem_cells[best_idx] = chosen;
        return total;
    }

    __int128 count_total() {
        vector<int> rem = {1, 2, 3, 5, 6, 7};
        vector<Assignment> assigns;
        do {
            int r2 = rem[0], r3 = rem[1];
            int r4 = rem[2], r5 = rem[3];
            int r6 = rem[4], r7 = rem[5];
            if (r2 < r3 && r4 < r5 && r6 < r7 && r2 < r4 && r4 < r6) {
                assigns.push_back({r2, r3, r4, r5, r6, r7});
            }
        } while (next_permutation(rem.begin(), rem.end()));

        long long total_count = 0;

        #pragma omp parallel for reduction(+:total_count) schedule(dynamic)
        for (int i = 0; i < (int)assigns.size(); ++i) {
            int grid[8][8];
            int row_mask[8] = {0}, col_mask[8] = {0}, box_mask[8] = {0};
            for (int r = 0; r < 8; ++r)
                for (int c = 0; c < 8; ++c)
                    grid[r][c] = -1;

            grid[0][0] = 0; grid[0][1] = 1; grid[0][2] = 2; grid[0][3] = 3;
            grid[1][0] = 4; grid[1][1] = 5; grid[1][2] = 6; grid[1][3] = 7;

            grid[0][4] = 4; grid[0][5] = 5; grid[0][6] = 6; grid[0][7] = 7;

            for (int r = 2; r < 8; ++r) {
                grid[r][0] = assigns[i].val[r - 2];
            }

            Cell rem_cells[64];
            int rem_cnt = 0;

            for (int r = 0; r < 8; ++r) {
                for (int c = 0; c < 8; ++c) {
                    if (grid[r][c] != -1) {
                        int d = grid[r][c];
                        row_mask[r] |= (1 << d);
                        col_mask[c] |= (1 << d);
                        box_mask[(r / 2) * 2 + (c / 4)] |= (1 << d);
                    } else {
                        rem_cells[rem_cnt++] = {r, c, (r / 2) * 2 + (c / 4)};
                    }
                }
            }

            total_count += dfs(grid, row_mask, col_mask, box_mask, rem_cells, rem_cnt);
        }

        __int128 fact8 = 40320;
        __int128 fact4 = 24;
        __int128 sym_group = 48;
        return (__int128)total_count * fact8 * fact4 * sym_group;
    }
}

namespace Solver9 {
    struct Pair { int m3, m4; };
    vector<Pair> all_pairs;
    int pair_idx[512][512];
    int match_table[1680][1680];
    vector<int> valid_k1_for_k0[1680];

    void init_pairs() {
        memset(pair_idx, -1, sizeof(pair_idx));
        for (int m3 = 0; m3 < 512; ++m3) {
            if (__builtin_popcount(m3) != 3) continue;
            for (int m4 = 0; m4 < 512; ++m4) {
                if (__builtin_popcount(m4) != 3) continue;
                if (m3 & m4) continue;
                pair_idx[m3][m4] = all_pairs.size();
                all_pairs.push_back({m3, m4});
            }
        }
        for (int i = 0; i < 1680; ++i) {
            for (int j = 0; j < 1680; ++j) {
                if ((all_pairs[i].m3 & all_pairs[j].m3) == 0 &&
                    (all_pairs[i].m4 & all_pairs[j].m4) == 0) {
                    int req_m3 = 0x1ff ^ (all_pairs[i].m3 | all_pairs[j].m3);
                    int req_m4 = 0x1ff ^ (all_pairs[i].m4 | all_pairs[j].m4);
                    if ((req_m3 & req_m4) == 0) {
                        match_table[i][j] = pair_idx[req_m3][req_m4];
                        valid_k1_for_k0[i].push_back(j);
                    } else {
                        match_table[i][j] = -1;
                    }
                } else {
                    match_table[i][j] = -1;
                }
            }
        }
    }

    struct SymMap { int s_map[9]; };
    vector<SymMap> sym_maps;

    void init_sym_maps() {
        vector<vector<int>> p3;
        vector<int> p = {0, 1, 2};
        do { p3.push_back(p); } while (next_permutation(p.begin(), p.end()));

        for (auto &pr : p3) {
            for (auto &pc : p3) {
                SymMap sm;
                for (int r = 0; r < 3; ++r) {
                    for (int c = 0; c < 3; ++c) {
                        sm.s_map[3 * r + c] = 3 * pr[r] + pc[c];
                    }
                }
                sym_maps.push_back(sm);
            }
        }
    }

    uint64_t get_canon(int C3, int C4, int C5, int C6, int C7, int C8) {
        uint64_t best = ~0ULL;
        int orig[6] = {C3, C4, C5, C6, C7, C8};

        for (const auto &sm : sym_maps) {
            int m[6];
            for (int i = 0; i < 6; ++i) {
                int mask = orig[i];
                int nmask = 0;
                for (int d = 0; d < 9; ++d) {
                    if ((mask >> d) & 1) nmask |= (1 << sm.s_map[d]);
                }
                m[i] = nmask;
            }
            if (m[0] > m[1]) swap(m[0], m[1]);
            if (m[1] > m[2]) swap(m[1], m[2]);
            if (m[0] > m[1]) swap(m[0], m[1]);

            if (m[3] > m[4]) swap(m[3], m[4]);
            if (m[4] > m[5]) swap(m[4], m[5]);
            if (m[3] > m[4]) swap(m[3], m[4]);

            bool swap_boxes = false;
            for (int i = 0; i < 3; ++i) {
                if (m[i] != m[3 + i]) {
                    if (m[i] > m[3 + i]) swap_boxes = true;
                    break;
                }
            }
            if (swap_boxes) {
                swap(m[0], m[3]); swap(m[1], m[4]); swap(m[2], m[5]);
            }

            uint64_t key = 0;
            for (int i = 0; i < 6; ++i) key = (key << 9) | m[i];
            if (key < best) best = key;
        }
        return best;
    }

    struct Triple56 {
        int s0[3], rem0[3];
        int s1[3], rem1[3];
        int s2[3], rem2[3];
    };

    struct Triple { int s[3]; };

    vector<Triple> get_triples(int c0, int c1, int c2) {
        int A0 = 0x1ff ^ c0, A1 = 0x1ff ^ c1, A2 = 0x1ff ^ c2;
        vector<Triple> res;
        vector<int> sub0, sub1;
        for (int m = 0; m < 512; ++m) {
            if (__builtin_popcount(m) == 3) {
                if ((m & ~A0) == 0) sub0.push_back(m);
                if ((m & ~A1) == 0) sub1.push_back(m);
            }
        }
        for (int m0 : sub0) {
            for (int m1 : sub1) {
                if (m0 & m1) continue;
                int m2 = 0x1ff ^ (m0 | m1);
                if ((m2 & ~A2) == 0) res.push_back({m0, m1, m2});
            }
        }
        return res;
    }

    vector<int> get_pairs(Triple t) {
        vector<int> c0, c1, c2;
        for (int d = 0; d < 9; ++d) {
            if ((t.s[0] >> d) & 1) c0.push_back(d);
            if ((t.s[1] >> d) & 1) c1.push_back(d);
            if ((t.s[2] >> d) & 1) c2.push_back(d);
        }
        vector<int> res;
        for (int x : c0) {
            for (int y : c1) {
                for (int z : c2) {
                    int m3 = (1 << x) | (1 << y) | (1 << z);
                    vector<int> r0, r1, r2;
                    for (int a : c0) if (a != x) r0.push_back(a);
                    for (int a : c1) if (a != y) r1.push_back(a);
                    for (int a : c2) if (a != z) r2.push_back(a);
                    for (int x4 : r0) {
                        for (int y4 : r1) {
                            for (int z4 : r2) {
                                int m4 = (1 << x4) | (1 << y4) | (1 << z4);
                                res.push_back(pair_idx[m3][m4]);
                            }
                        }
                    }
                }
            }
        }
        return res;
    }

    struct Band1Class {
        int C[9];
        long long weight;
    };

    __int128 count_total() {
        init_pairs();
        init_sym_maps();

        int U0 = 0x1f8, U1 = 0x1c7, U2 = 0x03f;
        vector<Triple56> trips;

        for (int m0 = 0; m0 < 512; ++m0) {
            if (__builtin_popcount(m0) != 3 || (m0 & ~U0)) continue;
            int rem_U1 = U1 & ~m0;
            for (int m1 = 0; m1 < 512; ++m1) {
                if (__builtin_popcount(m1) != 3 || (m1 & ~rem_U1)) continue;
                int m2 = 0x1ff ^ (m0 | m1);
                if ((m2 & ~U2) == 0) {
                    Triple56 tr;
                    int idx0 = 0, idx_r0 = 0;
                    for (int d = 0; d < 9; ++d) {
                        if ((m0 >> d) & 1) tr.s0[idx0++] = d;
                        else if ((U0 >> d) & 1) tr.rem0[idx_r0++] = d;
                    }
                    int idx1 = 0, idx_r1 = 0;
                    for (int d = 0; d < 9; ++d) {
                        if ((m1 >> d) & 1) tr.s1[idx1++] = d;
                        else if ((U1 >> d) & 1) tr.rem1[idx_r1++] = d;
                    }
                    int idx2 = 0, idx_r2 = 0;
                    for (int d = 0; d < 9; ++d) {
                        if ((m2 >> d) & 1) tr.s2[idx2++] = d;
                        else if ((U2 >> d) & 1) tr.rem2[idx_r2++] = d;
                    }
                    trips.push_back(tr);
                }
            }
        }

        vector<vector<int>> p3;
        vector<int> p = {0, 1, 2};
        do { p3.push_back(p); } while (next_permutation(p.begin(), p.end()));

        map<uint64_t, long long> class_weights;

        for (const auto &tr : trips) {
            for (const auto &p1 : p3) {
                for (const auto &p2 : p3) {
                    for (const auto &q1 : p3) {
                        for (const auto &q2 : p3) {
                            int c3 = (1 << tr.s0[0]) | (1 << tr.s1[p1[0]]) | (1 << tr.s2[p2[0]]);
                            int c4 = (1 << tr.s0[1]) | (1 << tr.s1[p1[1]]) | (1 << tr.s2[p2[1]]);
                            int c5 = (1 << tr.s0[2]) | (1 << tr.s1[p1[2]]) | (1 << tr.s2[p2[2]]);

                            int c6 = (1 << tr.rem0[0]) | (1 << tr.rem1[q1[0]]) | (1 << tr.rem2[q2[0]]);
                            int c7 = (1 << tr.rem0[1]) | (1 << tr.rem1[q1[1]]) | (1 << tr.rem2[q2[1]]);
                            int c8 = (1 << tr.rem0[2]) | (1 << tr.rem1[q1[2]]) | (1 << tr.rem2[q2[2]]);

                            uint64_t cf = get_canon(c3, c4, c5, c6, c7, c8);
                            class_weights[cf] += 36;
                        }
                    }
                }
            }
        }

        vector<Band1Class> classes;
        for (auto &kv : class_weights) {
            Band1Class bc;
            bc.weight = kv.second;
            bc.C[0] = 73; bc.C[1] = 146; bc.C[2] = 292;
            uint64_t key = kv.first;
            for (int i = 8; i >= 3; --i) {
                bc.C[i] = key & 0x1ff;
                key >>= 9;
            }
            classes.push_back(bc);
        }

        int num_classes = classes.size();
        __int128 sum_completions = 0;

        #pragma omp parallel reduction(+:sum_completions)
        {
            int freq2[1680];
            int freq3[1680];
            bool in_p1[1680];
            bool in_p1_R[1680];
            memset(in_p1, 0, sizeof(in_p1));
            memset(in_p1_R, 0, sizeof(in_p1_R));

            #pragma omp for schedule(dynamic)
            for (int c_idx = 0; c_idx < num_classes; ++c_idx) {
                const auto &bc = classes[c_idx];
                auto T0 = get_triples(bc.C[0], bc.C[1], bc.C[2]);
                auto T1 = get_triples(bc.C[3], bc.C[4], bc.C[5]);
                auto T2 = get_triples(bc.C[6], bc.C[7], bc.C[8]);

                int sz0 = T0.size(), sz1 = T1.size(), sz2 = T2.size();
                vector<vector<int>> pairs0(sz0), pairs1(sz1), pairs2(sz2);
                vector<vector<int>> pairs0_R(sz0), pairs1_R(sz1), pairs2_R(sz2);

                for (int i = 0; i < sz0; ++i) {
                    pairs0[i] = get_pairs(T0[i]);
                    Triple t_R = {(0x1ff^bc.C[0])^T0[i].s[0], (0x1ff^bc.C[1])^T0[i].s[1], (0x1ff^bc.C[2])^T0[i].s[2]};
                    pairs0_R[i] = get_pairs(t_R);
                }
                for (int i = 0; i < sz1; ++i) {
                    pairs1[i] = get_pairs(T1[i]);
                    Triple t_R = {(0x1ff^bc.C[3])^T1[i].s[0], (0x1ff^bc.C[4])^T1[i].s[1], (0x1ff^bc.C[5])^T1[i].s[2]};
                    pairs1_R[i] = get_pairs(t_R);
                }
                for (int i = 0; i < sz2; ++i) {
                    pairs2[i] = get_pairs(T2[i]);
                    Triple t_R = {(0x1ff^bc.C[6])^T2[i].s[0], (0x1ff^bc.C[7])^T2[i].s[1], (0x1ff^bc.C[8])^T2[i].s[2]};
                    pairs2_R[i] = get_pairs(t_R);
                }

                long long cur_comp = 0;

                for (int i0 = 0; i0 < sz0; ++i0) {
                    for (int i1 = 0; i1 < sz1; ++i1) {
                        memset(freq2, 0, sizeof(freq2));
                        memset(freq3, 0, sizeof(freq3));

                        for (int k1 : pairs1[i1]) in_p1[k1] = true;
                        for (int k0 : pairs0[i0]) {
                            for (int k1 : valid_k1_for_k0[k0]) {
                                if (in_p1[k1]) freq2[match_table[k0][k1]]++;
                            }
                        }
                        for (int k1 : pairs1[i1]) in_p1[k1] = false;

                        for (int l1 : pairs1_R[i1]) in_p1_R[l1] = true;
                        for (int l0 : pairs0_R[i0]) {
                            for (int l1 : valid_k1_for_k0[l0]) {
                                if (in_p1_R[l1]) freq3[match_table[l0][l1]]++;
                            }
                        }
                        for (int l1 : pairs1_R[i1]) in_p1_R[l1] = false;

                        for (int i2 = 0; i2 < sz2; ++i2) {
                            int w2 = 0, w3 = 0;
                            for (int k2 : pairs2[i2]) w2 += freq2[k2];
                            if (w2 == 0) continue;
                            for (int l2 : pairs2_R[i2]) w3 += freq3[l2];
                            cur_comp += 1LL * w2 * w3;
                        }
                    }
                }

                sum_completions += ((__int128)cur_comp) * bc.weight;
            }
        }

        __int128 fact9 = 362880;
        return sum_completions * fact9;
    }
}

void solve_for_n(int n) {
    __int128 N_total = 0;
    if (n == 4) {
        N_total = Solver4::count_total();
    } else if (n == 6) {
        N_total = Solver6::count_total();
    } else if (n == 8) {
        N_total = Solver8::count_total();
    } else if (n == 9) {
        N_total = Solver9::count_total();
    }

    long long num = 4LL * n - 2LL;
    long long den = 1LL * n * n;
    long long g = gcd_ll(num, den);
    num /= g;
    den /= g;

    __int128 N_corner = (N_total * num) / den;

    print128(N_corner);
    cout << "\n";
    cout << num << "/" << den << "\n";
}

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n = 0;
    if (argc > 1) {
        n = atoi(argv[1]);
    } else if (!(cin >> n)) {
        n = 4;
    }

    solve_for_n(n);
    return 0;
}
