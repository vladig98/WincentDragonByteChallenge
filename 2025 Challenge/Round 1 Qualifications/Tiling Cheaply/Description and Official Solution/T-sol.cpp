/*
 * this is a sample full solution
 */
#include <bits/stdc++.h>
using namespace std;

const vector< vector<int> > SEVEN_SOLUTION = {
    {  0,  0,  1,  2,  2,  3,  3 },
    {  0,  4,  1,  1,  2,  3,  3 },
    {  4,  4,  5,  5,  6,  7,  7 },
    {  8,  8,  5,  9,  6,  6,  7 },
    {  8, 10, 10,  9,  9, 11, 11 },
    { 12, 10, 13, 13, 14, 15, 11 },
    { 12, 12, 13, 14, 14, 15, 15 }
};

int N, T;
vector< vector<int> > board;

void report() {
    // determine a valid coloring of adjacent tiles
    vector<int> color(T, 9);
    for (int t=0; t<T; ++t) {
        vector<bool> seen(10, false);
        for (int r=0; r<N; ++r) for (int c=0; c<N; ++c) if (board[r][c] == t) {
            for (int dr=-1; dr<=1; ++dr) for (int dc=-1; dc<=1; ++dc) {
                if (abs(dr) + abs(dc) != 1) continue;
                int nr = r+dr, nc = c+dc;
                if (!(0 <= nr && nr < N && 0 <= nc && nc < N)) continue;
                seen[ color[ board[nr][nc] ] ] = true;
            }
        }
        color[t] = 0; 
        while (seen[color[t]]) ++color[t];
    }
    // print the board
    for (auto row : board) { for (int x : row) { cout << (char)('a'+color[x]); } cout << endl; }
}

void add_squares(int rlo, int clo, int rhi, int chi) {
    for (int r=rlo; r<rhi; r+=2) for (int c=clo; c<chi; c+=2) {
        board[r][c] = board[r+1][c] = board[r][c+1] = board[r+1][c+1] = T;
        ++T;
    }
}

int main() {
    int TC;
    cin >> TC;
    while (TC--) {
        cin >> N;
        board.clear();
        board.resize( N, vector<int>(N, -1) );
        if (N == 1 || N == 3 || N == 5) { cout << "NO\n"; continue; }
        if (N % 2 == 0) {
            T = 0;
            for (int r=0; r<N; r+=2) add_squares(r, 0, r+2, N);
        } else {
            for (int r=0; r<7; ++r) for (int c=0; c<7; ++c) board[r][c] = SEVEN_SOLUTION[r][c];
            T = 16;
            for (int n=9; n<=N; n+=2) {
                // add two more strips along bottom + right
                board[n-2][0] = board[n-1][0] = board[n-1][1] = T+0;
                board[n-2][1] = board[n-2][2] = board[n-1][2] = T+1;
                board[0][n-2] = board[0][n-1] = board[1][n-1] = T+2;
                board[1][n-2] = board[2][n-2] = board[2][n-1] = T+3;
                T += 4;
                add_squares(n-2, 3, n, n);
                add_squares(3, n-2, n-2, n);
            }
        }
        cout << "YES\n";
        report();
    }
}
