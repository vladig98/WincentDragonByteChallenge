#include <bits/stdc++.h>
#include <gurobi_c++.h> 

using namespace std;

const int MAXDIM = 205;
const int MULT1 = 3, MULT0 = 1;

struct instruction {
    int r0, r1, c0, c1;
    string ins;
    bool operator<(const instruction& o) const {
        return tie(r0, r1, c0, c1, ins) < tie(o.r0, o.r1, o.c0, o.c1, o.ins);
    }
};

int R, C;
int must[2*MAXDIM+1][2*MAXDIM+1];
int cannot[2*MAXDIM+1][2*MAXDIM+1];
int pmust[2*MAXDIM+2][2*MAXDIM+2];
int pcannot[2*MAXDIM+2][2*MAXDIM+2];

void rebuild_prefix_sums() {
    memset(pmust, 0, sizeof(pmust));
    memset(pcannot, 0, sizeof(pcannot));
    for (int r=0; r<=2*R; ++r) {
        for (int c=0; c<=2*C; ++c) {
            pmust[r+1][c+1] = pmust[r+1][c] + pmust[r][c+1] - pmust[r][c] + must[r][c];
            pcannot[r+1][c+1] = pcannot[r+1][c] + pcannot[r][c+1] - pcannot[r][c] + cannot[r][c];
        }
    }
}

inline int pmust_query(int r0, int r1, int c0, int c1) {
    return pmust[r1+1][c1+1] - pmust[r1+1][c0] - pmust[r0][c1+1] + pmust[r0][c0];
}

inline int pcannot_query(int r0, int r1, int c0, int c1) {
    return pcannot[r1+1][c1+1] - pcannot[r1+1][c0] - pcannot[r0][c1+1] + pcannot[r0][c0];
}

void update(int &bscore, int &br0, int &br1, int &bc0, int &bc1, string &bcmds, 
            int score, int r0, int r1, int c0, int c1, string cmds) {
    if (score <= bscore) return;
    bscore = score; br0 = r0; br1 = r1; bc0 = c0; bc1 = c1; bcmds = cmds;
}

// Normalizes single-edge commands so duplicate geometric instructions are pruned
instruction normalize(instruction I) {
    sort(I.ins.begin(), I.ins.end());
    if (I.ins == "T") I.r1 = I.r0;
    if (I.ins == "B") I.r0 = I.r1;
    if (I.ins == "L") I.c1 = I.c0;
    if (I.ins == "R") I.c0 = I.c1;
    return I;
}

// Custom Callback to report chronological outputs seamlessly
class PeriodicOutputCallback : public GRBCallback {
private:
    double interval, last_output_time, best_obj;
    bool has_incumbent;
    vector<GRBVar> vars;
    vector<instruction> lp_ops, greedy_ops;
    vector<double> best_sol;

public:
    PeriodicOutputCallback(const vector<GRBVar>& _vars, const vector<instruction>& _lp, 
                           const vector<instruction>& _greedy, double _interval)
        : vars(_vars), lp_ops(_lp), greedy_ops(_greedy), interval(_interval), 
          last_output_time(0.0), has_incumbent(false), best_obj(-1.0) {
        best_sol.resize(vars.size(), 0.0);
    }

    void callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                if (!vars.empty()) {
                    double* vals = getSolution(vars.data(), vars.size());
                    for (size_t i = 0; i < vars.size(); i++) best_sol[i] = vals[i];
                    delete[] vals;
                }
                best_obj = getDoubleInfo(GRB_CB_MIPSOL_OBJ);
                has_incumbent = true;
            }
            if (where == GRB_CB_MIP || where == GRB_CB_MIPSOL || where == GRB_CB_MIPNODE) {
                double current_time = getDoubleInfo(GRB_CB_RUNTIME);
                if (current_time - last_output_time >= interval) {
                    if (has_incumbent) {
                        int total = greedy_ops.size();
                        for (double v : best_sol) if (v > 0.5) total++;

                        cout << "\n=== Best Solution so far (Time: " << current_time << "s, Ops: " << total << ") ===\n";
                        cout << total << "\n";
                        for (size_t i = 0; i < vars.size(); ++i) {
                            if (best_sol[i] > 0.5) {
                                auto& op = lp_ops[i];
                                cout << op.r0 << " " << op.r1 << " " << op.c0 << " " << op.c1 << " " << op.ins << "\n";
                            }
                        }
                        for (auto it = greedy_ops.rbegin(); it != greedy_ops.rend(); ++it) {
                            cout << it->r0 << " " << it->r1 << " " << it->c0 << " " << it->c1 << " " << it->ins << "\n";
                        }
                        cout << "=================================================\n" << endl;
                    }
                    last_output_time = current_time;
                }
            }
        } catch (...) {}
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    memset(must, 0, sizeof(must));
    memset(cannot, 0, sizeof(cannot));
    vector<instruction> greedy_ops;

    string infile;
    if (!(cin >> infile)) return 0;
    ifstream in(infile);
    if (!in.is_open()) return 0;

    in >> R >> C;
    for (int r = 0; r <= 2 * R; ++r) {
        string line;
        in >> line;
        for (int c = 0; c <= 2 * C; ++c) {
            if (r % 2 + c % 2 == 1) {
                if (line[c] == '.') cannot[r][c] = 1; else must[r][c] = 1;
            }
        }
    }

    // --- GREEDY SOLVER PHASE ---
    const int MIN_GREEDY_SCORE = 180; 
    while (true) {
        rebuild_prefix_sums();
        if (pmust[2 * R + 1][2 * C + 1] == 0) break;
        cerr << "remaining 1s: " << pmust[2*R+1][2*C+1] << endl;
        int bscore = -1, br0 = -1, br1 = -1, bc0 = -1, bc1 = -1; string bcmds;

        for (int r0=0; r0<R; ++r0) for (int r1=r0; r1<R; ++r1) for (int c0=0; c0<C; ++c0) for (int c1=c0; c1<C; ++c1) {
            int amust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c1+2), acannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c1+2);
            if (amust > 0 && acannot > 0) break; 
            if (acannot == 0) update(bscore,br0,br1,bc0,bc1,bcmds,MULT1*amust,r0,r1,c0,c1,"A");
            if (amust == 0) update(bscore,br0,br1,bc0,bc1,bcmds,MULT0*acannot,r0,r1,c0,c1,"E");
        }

        // T, B, L, R combinations
        for (int r0=0; r0<R; ++r0) for (int c0=0; c0<C; ++c0) {
            for (int c1=c0; c1<C; ++c1) {
                int tmust = pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2), tcannot = pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2);
                if (tmust == 0) continue;
                if (tcannot > 0) break;
                for (int r1=r0; r1<R; ++r1) {
                    int bmust = pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2), bcannot = pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2);
                    int lmust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0),      lcannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0);
                    int rmust = pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2),  rcannot = pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2);
                    string cmds = ""; int affected = 0;
                    if (tcannot == 0) { cmds += 'T'; affected += tmust; }
                    if (bcannot == 0) { cmds += 'B'; affected += bmust; }
                    if (lcannot == 0) { cmds += 'L'; affected += lmust; }
                    if (rcannot == 0) { cmds += 'R'; affected += rmust; }
                    if (cmds != "") update(bscore,br0,br1,bc0,bc1,bcmds,MULT1*affected,r0,r1,c0,c1,cmds);
                }
            }
        }
        for (int r1=0; r1<R; ++r1) for (int c0=0; c0<C; ++c0) {
            for (int c1=c0; c1<C; ++c1) {
                int bmust = pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2), bcannot = pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2);
                if (bmust == 0) continue;
                if (bcannot > 0) break;
                for (int r0=r1; r0>=0; --r0) {
                    int tmust = pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2),     tcannot = pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2);
                    int lmust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0),     lcannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0);
                    int rmust = pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2), rcannot = pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2);
                    string cmds = ""; int affected = 0;
                    if (tcannot == 0) { cmds += 'T'; affected += tmust; }
                    if (bcannot == 0) { cmds += 'B'; affected += bmust; }
                    if (lcannot == 0) { cmds += 'L'; affected += lmust; }
                    if (rcannot == 0) { cmds += 'R'; affected += rmust; }
                    if (cmds != "") update(bscore,br0,br1,bc0,bc1,bcmds,MULT1*affected,r0,r1,c0,c1,cmds);
                }
            }
        }
        for (int r0=0; r0<R; ++r0) for (int c0=0; c0<C; ++c0) {
            for (int r1=r0; r1<R; ++r1) {
                int lmust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0), lcannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0);
                if (lmust == 0) continue;
                if (lcannot > 0) break;
                for (int c1=c0; c1<C; ++c1) {
                    int tmust = pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2),     tcannot = pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2);
                    int bmust = pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2), bcannot = pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2);
                    int rmust = pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2), rcannot = pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2);
                    string cmds = ""; int affected = 0;
                    if (tcannot == 0) { cmds += 'T'; affected += tmust; }
                    if (bcannot == 0) { cmds += 'B'; affected += bmust; }
                    if (lcannot == 0) { cmds += 'L'; affected += lmust; }
                    if (rcannot == 0) { cmds += 'R'; affected += rmust; }
                    if (cmds != "") update(bscore,br0,br1,bc0,bc1,bcmds,MULT1*affected,r0,r1,c0,c1,cmds);
                }
            }
        }
        for (int r0=0; r0<R; ++r0) for (int c1=0; c1<C; ++c1) {
            for (int r1=r0; r1<R; ++r1) {
                int rmust = pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2), rcannot = pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2);
                if (rmust == 0) continue;
                if (rcannot > 0) break;
                for (int c0=c1; c0>=0; --c0) {
                    int tmust = pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2),     tcannot = pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2);
                    int bmust = pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2), bcannot = pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2);
                    int lmust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0),     lcannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0);
                    string cmds = ""; int affected = 0;
                    if (tcannot == 0) { cmds += 'T'; affected += tmust; }
                    if (bcannot == 0) { cmds += 'B'; affected += bmust; }
                    if (lcannot == 0) { cmds += 'L'; affected += lmust; }
                    if (rcannot == 0) { cmds += 'R'; affected += rmust; }
                    if (cmds != "") update(bscore,br0,br1,bc0,bc1,bcmds,MULT1*affected,r0,r1,c0,c1,cmds);
                }
            }
        }

        // STOP greedy parsing if best impact is trivial. Handover to LP.
        if (bscore < MIN_GREEDY_SCORE) break;

        greedy_ops.push_back({br0, br1, bc0, bc1, bcmds});
        if (bcmds == "A") {
            for (int r=2*br0; r<=2*br1+2; ++r) for (int c=2*bc0; c<=2*bc1+2; ++c) must[r][c] = 0;
        } else if (bcmds == "E") {
            for (int r=2*br0; r<=2*br1+2; ++r) for (int c=2*bc0; c<=2*bc1+2; ++c) cannot[r][c] = 0;
        } else {
            for (char cmd : bcmds) {
                if (cmd == 'T') for (int c=2*bc0; c<=2*bc1+2; ++c) must[2*br0][c] = 0;
                if (cmd == 'B') for (int c=2*bc0; c<=2*bc1+2; ++c) must[2*br1+2][c] = 0;
                if (cmd == 'L') for (int r=2*br0; r<=2*br1+2; ++r) must[r][2*bc0] = 0;
                if (cmd == 'R') for (int r=2*br0; r<=2*br1+2; ++r) must[r][2*bc1+2] = 0;
            }
        }
    }


    // --- LP GENERATION PHASE ---
    rebuild_prefix_sums();
    set<instruction> lp_candidates;

    for (int r0=0; r0<R; ++r0) for (int c0=0; c0<C; ++c0) for (int r1=r0; r1<R; ++r1) for (int c1=c0; c1<C; ++c1) {
        if (pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c1+2) > 0) break; 
        if (pmust_query(2*r0, 2*r1+2, 2*c0, 2*c1+2) > 0) lp_candidates.insert(normalize({r0, r1, c0, c1, "A"}));
    }

    auto generate_borders = [&](int r0, int r1, int c0, int c1) {
        int tmust = pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2),     tcannot = pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2);
        int bmust = pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2), bcannot = pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2);
        int lmust = pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0),     lcannot = pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0);
        int rmust = pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2), rcannot = pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2);
        string cmds = "";
        if (tcannot == 0 && tmust > 0) cmds += "T";
        if (bcannot == 0 && bmust > 0) cmds += "B";
        if (lcannot == 0 && lmust > 0) cmds += "L";
        if (rcannot == 0 && rmust > 0) cmds += "R";
        if (!cmds.empty()) lp_candidates.insert(normalize({r0, r1, c0, c1, cmds}));
    };

    for(int r0=0; r0<R; ++r0) for(int c0=0; c0<C; ++c0) {
        for(int c1=c0; c1<C; ++c1) {
            if(pcannot_query(2*r0, 2*r0, 2*c0, 2*c1+2) > 0) break;
            if(pmust_query(2*r0, 2*r0, 2*c0, 2*c1+2) == 0) continue;
            for(int r1=r0; r1<R; ++r1) generate_borders(r0, r1, c0, c1);
        }
    }
    for(int r1=0; r1<R; ++r1) for(int c0=0; c0<C; ++c0) {
        for(int c1=c0; c1<C; ++c1) {
            if(pcannot_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2) > 0) break;
            if(pmust_query(2*r1+2, 2*r1+2, 2*c0, 2*c1+2) == 0) continue;
            for(int r0=r1; r0>=0; --r0) generate_borders(r0, r1, c0, c1);
        }
    }
    for(int r0=0; r0<R; ++r0) for(int c0=0; c0<C; ++c0) {
        for(int r1=r0; r1<R; ++r1) {
            if(pcannot_query(2*r0, 2*r1+2, 2*c0, 2*c0) > 0) break;
            if(pmust_query(2*r0, 2*r1+2, 2*c0, 2*c0) == 0) continue;
            for(int c1=c0; c1<C; ++c1) generate_borders(r0, r1, c0, c1);
        }
    }
    for(int r0=0; r0<R; ++r0) for(int c1=0; c1<C; ++c1) {
        for(int r1=r0; r1<R; ++r1) {
            if(pcannot_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2) > 0) break;
            if(pmust_query(2*r0, 2*r1+2, 2*c1+2, 2*c1+2) == 0) continue;
            for(int c0=c1; c0>=0; --c0) generate_borders(r0, r1, c0, c1);
        }
    }

    // --- GUROBI OPTIMIZATION ---
    vector<instruction> lp_ops_list(lp_candidates.begin(), lp_candidates.end());
    int n_vars = lp_ops_list.size();

    try {
        GRBEnv env = GRBEnv(true);
        env.set("OutputFlag", "0");
        env.start();
        GRBModel model = GRBModel(env);

        vector<GRBVar> vars(n_vars);
        for(int i = 0; i < n_vars; ++i) {
            vars[i] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY, "v" + to_string(i));
        }

        map<pair<int,int>, GRBLinExpr> edge_expr;
        for (int i = 0; i < n_vars; ++i) {
            auto& I = lp_ops_list[i];
            auto add_edge = [&](int r, int c) { if (must[r][c]) edge_expr[{r, c}] += vars[i]; };
            if (I.ins == "A") {
                for (int r = 2 * I.r0; r <= 2 * I.r1 + 2; ++r) 
                    for (int c = 2 * I.c0; c <= 2 * I.c1 + 2; ++c) 
                        if (r % 2 + c % 2 == 1) add_edge(r, c);
            } else {
                for (char cmd : I.ins) {
                    if (cmd == 'T') for (int c = 2 * I.c0; c <= 2 * I.c1 + 2; ++c) if (c % 2 == 1) add_edge(2 * I.r0, c);
                    if (cmd == 'B') for (int c = 2 * I.c0; c <= 2 * I.c1 + 2; ++c) if (c % 2 == 1) add_edge(2 * I.r1 + 2, c);
                    if (cmd == 'L') for (int r = 2 * I.r0; r <= 2 * I.r1 + 2; ++r) if (r % 2 == 1) add_edge(r, 2 * I.c0);
                    if (cmd == 'R') for (int r = 2 * I.r0; r <= 2 * I.r1 + 2; ++r) if (r % 2 == 1) add_edge(r, 2 * I.c1 + 2);
                }
            }
        }

        for (auto& kv : edge_expr) {
            model.addConstr(kv.second >= 1.0);
        }

        // Setup the Custom Callback - Triggered every 5.0 seconds
        PeriodicOutputCallback cb(vars, lp_ops_list, greedy_ops, 5.0);
        model.setCallback(&cb);
        model.optimize();

        // Print final optimum chronological list (LP -> Greedy in reverse)
        vector<instruction> final_lp_ops;
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL || model.get(GRB_IntAttr_SolCount) > 0) {
            for(int i = 0; i < n_vars; ++i) {
                if (vars[i].get(GRB_DoubleAttr_X) > 0.5) final_lp_ops.push_back(lp_ops_list[i]);
            }
        }

        cout << "\n=== FINAL OPTIMAL SOLUTION ===\n";
        cout << final_lp_ops.size() + greedy_ops.size() << "\n";
        for (auto& op : final_lp_ops) cout << op.r0 << " " << op.r1 << " " << op.c0 << " " << op.c1 << " " << op.ins << "\n";
        for (auto it = greedy_ops.rbegin(); it != greedy_ops.rend(); ++it) {
            cout << it->r0 << " " << it->r1 << " " << it->c0 << " " << it->c1 << " " << it->ins << "\n";
        }

    } catch(GRBException& e) {
        cerr << "Gurobi Error: " << e.getMessage() << endl;
    } catch(...) {}

    return 0;
}
