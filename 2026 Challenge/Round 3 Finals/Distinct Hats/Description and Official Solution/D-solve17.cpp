#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void solve_n_eq_m(int n, int m) {
    long long total_sum = 1LL * n * (n + 1) / 2;
    vector<int> cur;
    vector<bool> used(m + 1, false);
    long long cur_sum = 0;
    bool first = true;

    auto gen = [&](auto& self, int depth) -> void {
        if (depth == n - 1) {
            int missing = total_sum - cur_sum;
            if (!first) cout << ' ';
            cout << missing;
            first = false;
            return;
        }
        for (int i = 1; i <= m; ++i) {
            if (!used[i]) {
                used[i] = true;
                cur_sum += i;
                self(self, depth + 1);
                cur_sum -= i;
                used[i] = false;
            }
        }
    };
    gen(gen, 0);
    cout << '\n';
}

void solve_n3_odd(int m) {
    bool first = true;
    for (int h1 = 1; h1 <= m; ++h1) {
        for (int h2 = 1; h2 <= m; ++h2) {
            if (h1 == h2) continue;
            int x = h1 - 1;
            int y = h2 - 1;
            int v = 1 + ((2 * x - y) % m + m) % m;
            if (!first) cout << ' ';
            cout << v;
            first = false;
        }
    }
    cout << '\n';
}

void solve_n3_even(int m) {
    int q = m - 1;
    int h = (q + 1) / 2;
    bool first = true;
    for (int h1 = 1; h1 <= m; ++h1) {
        for (int h2 = 1; h2 <= m; ++h2) {
            if (h1 == h2) continue;
            int x = h1 - 1;
            int y = h2 - 1;
            int v = 0;
            if (x < q && y < q) {
                if (y == (x + 1) % q) {
                    v = q;
                } else {
                    v = (1LL * (x + y) * h) % q;
                }
            } else if (x < q && y == q) {
                v = (x + h) % q;
            } else if (x == q && y < q) {
                v = ((y - h) % q + q) % q;
            } else {
                v = q;
            }
            if (!first) cout << ' ';
            cout << v + 1;
            first = false;
        }
    }
    cout << '\n';
}

void solve_2sat(int n, int m) {
    vector<vector<int>> tuples;
    vector<int> cur;
    vector<bool> used(m + 1, false);
    auto gen = [&](auto& self, int depth) -> void {
        if (depth == n - 1) {
            tuples.push_back(cur);
            return;
        }
        for (int i = 1; i <= m; ++i) {
            if (!used[i]) {
                used[i] = true;
                cur.push_back(i);
                self(self, depth + 1);
                cur.pop_back();
                used[i] = false;
            }
        }
    };
    gen(gen, 0);

    int num_vars = tuples.size();
    vector<pair<int, int>> choices(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        vector<bool> has(m + 1, false);
        for (int x : tuples[i]) has[x] = true;
        vector<int> rem;
        for (int x = 1; x <= m; ++x) {
            if (!has[x]) rem.push_back(x);
        }
        choices[i] = {rem[0], rem[1]};
    }

    vector<vector<int>> adj(2 * num_vars);
    auto add_clause = [&](int lit1, int lit2) {
        adj[lit1 ^ 1].push_back(lit2);
        adj[lit2 ^ 1].push_back(lit1);
    };

    for (int i = 0; i < num_vars; ++i) {
        for (int j = i + 1; j < num_vars; ++j) {
            int diff = 0;
            for (int k = 0; k < n - 1; ++k) {
                if (tuples[i][k] != tuples[j][k]) diff++;
            }
            if (diff == 1) {
                int c_i[2] = {choices[i].first, choices[i].second};
                int c_j[2] = {choices[j].first, choices[j].second};
                for (int b_i = 0; b_i < 2; ++b_i) {
                    for (int b_j = 0; b_j < 2; ++b_j) {
                        if (c_i[b_i] == c_j[b_j]) {
                            int lit_i = 2 * i + (1 - b_i);
                            int lit_j = 2 * j + (1 - b_j);
                            add_clause(lit_i ^ 1, lit_j ^ 1);
                        }
                    }
                }
            }
        }
    }

    int timer = 0, scc_count = 0;
    vector<int> tin(2 * num_vars, -1), low(2 * num_vars, -1), scc(2 * num_vars, -1);
    vector<int> stk;
    vector<bool> on_stk(2 * num_vars, false);

    auto dfs = [&](auto& self, int u) -> void {
        tin[u] = low[u] = ++timer;
        stk.push_back(u);
        on_stk[u] = true;
        for (int v : adj[u]) {
            if (tin[v] == -1) {
                self(self, v);
                low[u] = min(low[u], low[v]);
            } else if (on_stk[v]) {
                low[u] = min(low[u], tin[v]);
            }
        }
        if (low[u] == tin[u]) {
            while (true) {
                int v = stk.back();
                stk.pop_back();
                on_stk[v] = false;
                scc[v] = scc_count;
                if (v == u) break;
            }
            scc_count++;
        }
    };

    for (int i = 0; i < 2 * num_vars; ++i) {
        if (tin[i] == -1) dfs(dfs, i);
    }

    for (int i = 0; i < num_vars; ++i) {
        if (scc[2 * i] == scc[2 * i + 1]) {
            cout << "IMPOSSIBLE\n";
            return;
        }
    }

    for (int i = 0; i < num_vars; ++i) {
        int val = (scc[2 * i] < scc[2 * i + 1]) ? choices[i].second : choices[i].first;
        if (i > 0) cout << ' ';
        cout << val;
    }
    cout << '\n';
}

void solve_parity(int n, int m) {
    bool first = true;
    auto gen = [&](auto& self, int depth, unsigned long long used_mask, int cur_inv) -> void {
        if (depth == n - 1) {
            unsigned long long unused = (((1ULL << (m + 1)) - 2) ^ used_mask);
            int a = __builtin_ctzll(unused);
            unused ^= (1ULL << a);
            int b = __builtin_ctzll(unused);

            int inv_P = (cur_inv + (a - 1) + (m - b)) & 1;
            int ans = (inv_P == 0 ? b : a);

            if (!first) cout << ' ';
            cout << ans;
            first = false;
            return;
        }
        for (int v = 1; v <= m; ++v) {
            if (!(used_mask & (1ULL << v))) {
                int add_inv = __builtin_popcountll(used_mask >> (v + 1));
                self(self, depth + 1, used_mask | (1ULL << v), cur_inv + add_inv);
            }
        }
    };
    gen(gen, 0, 0, 0);
    cout << '\n';
}

void solve_mrv(int n, int m) {
    vector<vector<int>> tuples;
    vector<int> cur;
    vector<bool> used(m + 1, false);
    auto gen = [&](auto& self, int depth) -> void {
        if (depth == n - 1) {
            tuples.push_back(cur);
            return;
        }
        for (int i = 1; i <= m; ++i) {
            if (!used[i]) {
                used[i] = true;
                cur.push_back(i);
                self(self, depth + 1);
                cur.pop_back();
                used[i] = false;
            }
        }
    };
    gen(gen, 0);

    int num_vars = tuples.size();
    vector<int> assignment(num_vars, -1);
    vector<int> domains(num_vars, 0);

    for (int i = 0; i < num_vars; ++i) {
        int mask = ((1 << (m + 1)) - 1) & ~1;
        for (int x : tuples[i]) mask &= ~(1 << x);
        domains[i] = mask;
    }

    vector<vector<int>> adj(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        for (int j = i + 1; j < num_vars; ++j) {
            int diff = 0;
            for (int k = 0; k < n - 1; ++k) {
                if (tuples[i][k] != tuples[j][k]) diff++;
            }
            if (diff == 1) {
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    }

    auto backtrack = [&](auto& self, int assigned) -> bool {
        if (assigned == num_vars) return true;

        int best_var = -1, min_dom = 999;
        for (int i = 0; i < num_vars; ++i) {
            if (assignment[i] == -1) {
                int sz = __builtin_popcount(domains[i]);
                if (sz < min_dom) {
                    min_dom = sz;
                    best_var = i;
                    if (sz <= 1) break;
                }
            }
        }
        if (min_dom == 0) return false;

        int d = domains[best_var];
        while (d > 0) {
            int v = __builtin_ctz(d);
            int val_bit = 1 << v;
            d &= ~val_bit;

            vector<int> affected;
            bool ok = true;
            for (int neighbor : adj[best_var]) {
                if (assignment[neighbor] == -1 && (domains[neighbor] & val_bit)) {
                    affected.push_back(neighbor);
                    domains[neighbor] &= ~val_bit;
                    if (domains[neighbor] == 0) ok = false;
                }
            }

            assignment[best_var] = v;
            if (ok && self(self, assigned + 1)) return true;

            for (int neighbor : affected) {
                domains[neighbor] |= val_bit;
            }
            assignment[best_var] = -1;
        }
        return false;
    };

    if (backtrack(backtrack, 0)) {
        for (int i = 0; i < num_vars; ++i) {
            if (i > 0) cout << ' ';
            cout << assignment[i];
        }
        cout << '\n';
    } else {
        cout << "IMPOSSIBLE\n";
    }
}

void solve_n4_pow2(int m) {
    bool first = true;
    for (int h1 = 1; h1 <= m; ++h1) {
        for (int h2 = 1; h2 <= m; ++h2) {
            if (h2 == h1) continue;
            for (int h3 = 1; h3 <= m; ++h3) {
                if (h3 == h1 || h3 == h2) continue;
                int v = 1 + (((h1 - 1) ^ (h2 - 1) ^ (h3 - 1)));
                if (!first) cout << ' ';
                cout << v;
                first = false;
            }
        }
    }
    cout << '\n';
}

bool solve_steiner(int n, int m) {
    int t_sub = n - 1;
    int k_block = n;
    int v_elem = m;

    vector<vector<int>> subs;
    vector<int> cur;
    auto gen_subs = [&](auto& self, int start, int depth) -> void {
        if (depth == t_sub) {
            subs.push_back(cur);
            return;
        }
        for (int i = start; i <= v_elem; ++i) {
            cur.push_back(i);
            self(self, i + 1, depth + 1);
            cur.pop_back();
        }
    };
    gen_subs(gen_subs, 1, 0);

    vector<vector<int>> blocks;
    auto gen_blocks = [&](auto& self, int start, int depth) -> void {
        if (depth == k_block) {
            blocks.push_back(cur);
            return;
        }
        for (int i = start; i <= v_elem; ++i) {
            cur.push_back(i);
            self(self, i + 1, depth + 1);
            cur.pop_back();
        }
    };
    gen_blocks(gen_blocks, 1, 0);

    int num_subs = subs.size();
    int num_blocks = blocks.size();

    auto sub_to_id = [&](const vector<int>& s) {
        return lower_bound(subs.begin(), subs.end(), s) - subs.begin();
    };

    vector<vector<int>> b_subs(num_blocks);
    vector<vector<int>> s_blocks(num_subs);

    for (int bi = 0; bi < num_blocks; ++bi) {
        vector<int> c;
        auto gen_b_subs = [&](auto& self, int start, int depth) -> void {
            if (depth == t_sub) {
                int sid = sub_to_id(c);
                b_subs[bi].push_back(sid);
                s_blocks[sid].push_back(bi);
                return;
            }
            for (int i = start; i < k_block; ++i) {
                c.push_back(blocks[bi][i]);
                self(self, i + 1, depth + 1);
                c.pop_back();
            }
        };
        gen_b_subs(gen_b_subs, 0, 0);
    }

    vector<bool> covered(num_subs, false);
    vector<int> chosen;

    auto search = [&](auto& self) -> bool {
        int best_s = -1;
        int min_c = 9999;
        for (int sid = 0; sid < num_subs; ++sid) {
            if (!covered[sid]) {
                int c = 0;
                for (int bi : s_blocks[sid]) {
                    bool ok = true;
                    for (int osid : b_subs[bi]) {
                        if (osid != sid && covered[osid]) {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) c++;
                }
                if (c < min_c) {
                    min_c = c;
                    best_s = sid;
                    if (c <= 1) break;
                }
            }
        }
        if (best_s == -1) return true;
        if (min_c == 0) return false;

        for (int bi : s_blocks[best_s]) {
            bool ok = true;
            for (int osid : b_subs[bi]) {
                if (osid != best_s && covered[osid]) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;

            for (int osid : b_subs[bi]) covered[osid] = true;
            chosen.push_back(bi);

            if (self(self)) return true;

            chosen.pop_back();
            for (int osid : b_subs[bi]) covered[osid] = false;
        }
        return false;
    };

    if (!search(search)) return false;

    vector<int> ans_map(1 << (v_elem + 1), 0);
    for (int bi : chosen) {
        int mask = 0;
        for (int x : blocks[bi]) mask |= (1 << x);
        for (int x : blocks[bi]) {
            int sub_mask = mask ^ (1 << x);
            ans_map[sub_mask] = x;
        }
    }

    vector<bool> used(m + 1, false);
    int cur_mask = 0;
    bool first = true;

    auto gen_out = [&](auto& self, int depth) -> void {
        if (depth == n - 1) {
            int v = ans_map[cur_mask];
            if (!first) cout << ' ';
            cout << v;
            first = false;
            return;
        }
        for (int i = 1; i <= m; ++i) {
            if (!used[i]) {
                used[i] = true;
                cur_mask |= (1 << i);
                self(self, depth + 1);
                cur_mask ^= (1 << i);
                used[i] = false;
            }
        }
    };
    gen_out(gen_out, 0);
    cout << '\n';
    return true;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int t;
    if (!(cin >> t)) return 0;
    while (t--) {
        int n, m;
        cin >> n >> m;
        if (m == n) {
            solve_n_eq_m(n, m);
        } else if (n == 3) {
            if (m % 2 == 1) solve_n3_odd(m);
            else solve_n3_even(m);
        } else if (m == n + 1) {
            solve_parity(n, m);
        } else if (n == 4 && (m & (m - 1)) == 0) {
            solve_n4_pow2(m);
        } else if (m - n == 2 && n >= 7) {
            cout << "IMPOSSIBLE\n";
        } else if (m - n == 3 && n >= 4) {
            cout << "IMPOSSIBLE\n";
        } else if ((n == 4 && (m % 6 == 2 || m % 6 == 4)) || (n == 5 && m == 11) || (n == 6 && m == 12)) {
            if (!solve_steiner(n, m)) {
                solve_mrv(n, m);
            }
        } else {
            solve_mrv(n, m);
        }
    }
    return 0;
}
