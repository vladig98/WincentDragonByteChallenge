#include <bits/stdc++.h>
using namespace std;

const int MAX_OPERATIONS = 25000;

void error(const string &msg) { cout << msg << endl; exit(1); }

int R, C;
vector<string> goal;
vector<string> state;

void draw_hline(vector<string> &grid, int r, int cmin, int cmax, char what='-') { for (int c=cmin; c<=cmax; ++c) grid[2*r][2*c+1] = what; }
void draw_vline(vector<string> &grid, int rmin, int rmax, int c, char what='|') { for (int r=rmin; r<=rmax; ++r) grid[2*r+1][2*c] = what; }
void draw(vector<string> &grid, int rmin, int rmax, int cmin, int cmax, const string &cmds) {
    for (char cmd : cmds) {
        if (cmd == 'T') draw_hline(grid, rmin, cmin, cmax);
        if (cmd == 'B') draw_hline(grid, rmax+1, cmin, cmax);
        if (cmd == 'L') draw_vline(grid, rmin, rmax, cmin);
        if (cmd == 'R') draw_vline(grid, rmin, rmax, cmax+1);
        if (cmd == 'A') {
            for (int r=rmin; r<=rmax+1; ++r) draw_hline(grid, r, cmin, cmax);
            for (int c=cmin; c<=cmax+1; ++c) draw_vline(grid, rmin, rmax, c);
        }
        if (cmd == 'E') {
            for (int r=rmin; r<=rmax+1; ++r) draw_hline(grid, r, cmin, cmax, '.');
            for (int c=cmin; c<=cmax+1; ++c) draw_vline(grid, rmin, rmax, c, '.');
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) error( string("") + "correct usage: ./" + argv[0] + " input contestants_output");
    ifstream fin(argv[1]), fout(argv[2]);
    if (!fin || !fout) error( "failed to open files to grade" );
    fin >> R >> C;
    goal.resize(2*R+1);
    for (auto &row : goal) fin >> row;
    if (!fin) error( "failed to read input" );
    state.resize(2*R+1, string(2*C+1, '.'));
    for (int r=0; r<=R; ++r) for (int c=0; c<=C; ++c) state[2*r][2*c] = '+';

    int n, ops;
    fout >> ops;
    if (!fout) error( "failed to read n");
    n = ops;
    if (n < 0 || n > MAX_OPERATIONS) error( "n outside allowed range" );
    while (n--) {
        int rmin, rmax, cmin, cmax;
        string cmds;
        fout >> rmin >> rmax >> cmin >> cmax >> cmds;
        if (!fout) error( "failed to read a command description" );
        if (!(0 <= rmin && rmin <= rmax && rmax <= R-1)) error( "invalid row range (" + to_string(rmin) + " " + to_string(rmax) + ")" );
        if (!(0 <= cmin && cmin <= cmax && cmax <= C-1)) error( "invalid column range (" + to_string(cmin) + " " + to_string(cmax) + ")" );
        bool ok_cmds = false;
        if (cmds == "A" || cmds == "E") ok_cmds = true;
        bool ok_sides = true;
        for (char x : cmds) if (x != 'T' && x != 'B' && x != 'L' && x != 'R') ok_sides = false;
        if (ok_sides) ok_cmds = true;
        if (!ok_cmds) error( "invalid command" );
        draw(state, rmin, rmax, cmin, cmax, cmds);
    }
    string token;
    fout >> token;
    if (fout) error( "too many tokens in the output" );
    if (state != goal) {
        for (unsigned r=0; r<state.size(); ++r) for (unsigned c=0; c<state[0].size(); ++c) if (state[r][c] != goal[r][c]) {
            error( "final drawing incorrect: at r" + to_string(r) + "c" + to_string(c) + " expected '" + goal[r][c] + "' got '" + state[r][c] + "'" );
        }
    }

    double score = min(100., 10 + 220000./ops);
    printf("%.2f\nOK (in %d operations)\n", score, ops);
    exit(0);
}
