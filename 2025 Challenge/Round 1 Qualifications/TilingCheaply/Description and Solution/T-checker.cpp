#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include "testlib.h"
using namespace std;

#define For(i,n) for(int i=0; i<((int)n); i++)
#define mp(a,b) make_pair((a),(b))
typedef long long ll;
typedef pair<int,int> pii;

#define MARKED_TILE '#'

vector<string> read_tiling(int size) {
    vector<string> result;
    For(i, size) {
        ouf.skipBlanks();
        string raw_row = ouf.readString();
        bool only_lowercase = true;
        string row = "";
        For(j, raw_row.size()) {
            if(isBlanks(raw_row[j])) continue;
            if(raw_row[j] < 'a' || raw_row[j] > 'z') only_lowercase = false;           
            row += raw_row[j];
        }
        quitif((int)row.size() != size, _wa, "Output is not an n times n square.");
        quitif(!only_lowercase, _wa, "Character in tiling is not a lowercase letter.");
        result.push_back(row);
    }
    return result;
}

int dx[] = {1, -1, 0, 0};
int dy[] = {0, 0, 1, -1};

bool check_one_tile(vector<string>& tiling, int x, int y, int size) {
    char letter = tiling[x][y];
    queue<pii> Q;
    Q.push({x, y});
    tiling[x][y] = MARKED_TILE;

    int tile_size = 0;
    map<int, int> X, Y;
    pii upper = {size, size}, lower = {-1, -1};

    while(!Q.empty()) {
        pii pos = Q.front();
        Q.pop();

        tile_size++;
        X[pos.first]++; Y[pos.second]++;
        upper.first = min(upper.first, pos.first);
        upper.second = min(upper.second, pos.second);
        lower.first = max(lower.first, pos.first);
        lower.second = max(lower.second, pos.second);

        For(i, 4) {
            int new_x = pos.first + dx[i], new_y = pos.second + dy[i];
            if(new_x < 0 || new_x >= size || new_y < 0 || new_y >= size) continue;
            if(tiling[new_x][new_y] == letter) {
                tiling[new_x][new_y] = MARKED_TILE;
                Q.push({new_x, new_y});
            }
        }
    }

    quitif(tile_size != 3 && tile_size != 4, _wa, "Tile of incorrect size %d.", tile_size);
    
    int tile_width = lower.first - upper.first + 1, tile_height = lower.second - upper.second + 1;
    
    if(tile_size == 3) {
        quitif(tile_width != 2 || tile_height != 2, _wa, "Trimino of incorrect shape.");
        return true;
    }
    
    if(tile_width == 2 && tile_height == 2) return false;
    
    if(tile_width != 2) {
        swap(tile_width, tile_height);
        swap(X, Y);
    }
    
    quitif(tile_width != 2 || tile_height != 3, _wa, "Tetromino of incorrect shape.");

    vector<int> x_count, y_count, x_correct = {2, 2}, y_correct = {1, 2, 1};
    for(auto it = X.begin(); it != X.end(); it++) x_count.push_back(it->second);
    for(auto it = Y.begin(); it != Y.end(); it++) y_count.push_back(it->second);
    quitif(x_count != x_correct || y_count != y_correct, _wa, "Tetromino of incorrect shape.");

    return false;
}

void check_test() {
    int n = inf.readInt();
    string correct_answer = ans.readWord();
    string answer = ouf.readWord();

    quitif(answer != "NO" && answer != "YES", _wa, "YES or NO expected but not found.");
    
    if(answer == "NO") {
        quitif(correct_answer != answer, _wa, "Existing tiling not found.");
        return;
    }

    vector<string> tiling = read_tiling(n);

    int trimino_count = 0;
    For(i, n) {
        For(j, n) {
            if(tiling[i][j] == MARKED_TILE) continue;
            if(check_one_tile(tiling, i, j, n))
                trimino_count++;
        }
    }

    quitif(correct_answer == "NO", _fail, "User's output is a valid tiling, but no solution is expected.");
    int min_price = ans.readInt();

    if(trimino_count > min_price) {
        // quitf(_wa, "Tiling contains %d triminos instead of %d.", trimino_count, min_price);
        quitf(_wa, "Tiling is not the cheapest one possible.");
    }
    else if(trimino_count < min_price) {
        quitf(_fail, "User's output is a valid tiling and contains smaller number of triminos, %d instead of %d as expected.", trimino_count, min_price);
    }
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int test_count = inf.readInt();

    For(i, test_count) {
        check_test();
    }

    quit(_ok, "Everything went smoothly.");
}
