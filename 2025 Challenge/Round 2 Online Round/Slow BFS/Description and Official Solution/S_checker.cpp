#include <bits/stdc++.h>
using namespace std;

const int MAX_RC = 300;
const int MAZE_SIZE = 5000;
const char MAZE_CHAR = '.';
const char MAZE_WALL = '#';
const vector<pair<int, int>> DIRECTIONS = { {-1, 0}, {0, -1}, {0, 1}, {1, 0} };

// reporting and terminating
void skap(const string& msg, int exit_code = 1) { cout << msg << endl; exit(exit_code); }
void cskap(bool cond, const string& msg, int exit_code = 1) { if (!cond) { skap(msg, exit_code); } }

int R = 0;
int C = 0;
vector<vector<int>> seen_cache;
int seen_cache_id = 0;

// Finds the first occurrence of MAZE_CHAR and returns its coordinates.
// Exits if no MAZE_CHAR is found.
pair<int, int> get_maze_starting_point(const vector<string>& maze) {
    for (int r_idx = 0; r_idx < R; ++r_idx) {
        for (int c_idx = 0; c_idx < C; ++c_idx) {
            if (maze[r_idx][c_idx] == MAZE_CHAR) {
                return {r_idx, c_idx};
            }
        }
    }
    skap("No . in maze");
    return {-1, -1}; // Should not be reached
}

// Checks if all MAZE_CHAR cells form a single connected component using DFS.
bool check_maze_connected(const vector<string>& maze) {
    pair<int, int> start_node = get_maze_starting_point(maze);
    vector<pair<int, int>> stack;
    stack.push_back(start_node);
    
    set<pair<int, int>> seen;
    seen.insert(start_node);

    while (!stack.empty()) {
        pair<int, int> curr = stack.back();
        stack.pop_back();
        int x = curr.first;
        int y = curr.second;

        for (const auto& dir : DIRECTIONS) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (nx < 0 || ny < 0 || nx >= R || ny >= C) continue;
            if (maze[nx][ny] != MAZE_CHAR || seen.count({nx, ny})) continue;
            
            seen.insert({nx, ny});
            stack.push_back({nx, ny});
        }
    }

    // Check if all MAZE_CHAR cells were visited.
    for (int r_idx = 0; r_idx < R; ++r_idx) {
        for (int c_idx = 0; c_idx < C; ++c_idx) {
            if (maze[r_idx][c_idx] == MAZE_CHAR && seen.find({r_idx, c_idx}) == seen.end()) {
                return false;
            }
        }
    }
    return true;
}

// Checks if the maze is a tree (V-1 = E), assuming it's connected.
bool check_maze_tree(const vector<string>& maze) {
    long long verts = 0, edges = 0;

    for (int x = 0; x < R; ++x) {
        for (int y = 0; y < C; ++y) {
            if (maze[x][y] != MAZE_CHAR) continue;
            
            verts++;
            // To avoid double-counting, only check neighbors up and left.
            for (int i = 0; i < 2; ++i) {
                const auto& dir = DIRECTIONS[i];
                int nx = x + dir.first;
                int ny = y + dir.second;
                if (nx < 0 || ny < 0 || nx >= R || ny >= C) continue;
                if (maze[nx][ny] == MAZE_CHAR) {
                    edges++;
                }
            }
        }
    }
    return edges == verts - 1;
}

// Reads and validates the maze from the input file stream.
vector<string> parse_maze(ifstream& f, int r, int c) {
    vector<string> maze;
    string row;

    for (int rownum = 0; rownum < r; ++rownum) {
        if (!getline(f, row)) {
            skap("Failed to read row #" + to_string(rownum + 1));
        }

        // Handle potential Windows-style line endings (\r\n)
        if (!row.empty() && row.back() == '\r') {
            row.pop_back();
        }
        
        cskap(row.length() == (size_t)c, "row #" + to_string(rownum + 1) + " has " + to_string(row.length()) + " characters, should have " + to_string(c));
        
        string badchars;
        for (char ch : row) {
            if (ch != MAZE_CHAR && ch != MAZE_WALL) {
                badchars += ch;
            }
        }
        cskap(badchars.empty(), "invalid characters in maze: " + badchars);
        maze.push_back(row);
    }
    
    long long maze_size_count = 0;
    for(const auto& maze_row : maze) {
        for(char cell : maze_row) {
            if (cell == MAZE_CHAR) {
                maze_size_count++;
            }
        }
    }
    cskap(maze_size_count == MAZE_SIZE, "maze should be size " + to_string(MAZE_SIZE) + " but is " + to_string(maze_size_count));

    cskap(check_maze_connected(maze), "maze is not connected");
    cskap(check_maze_tree(maze), "maze is not a tree");

    return maze;
}

long long runtimes_from(const vector<string>& maze, int start_x, int start_y) {
    if (seen_cache.empty()) {
        seen_cache.assign(R, vector<int>(C, 0));
    }

    seen_cache_id++;

    queue<pair<int, int>> q;
    q.push({start_x, start_y});
    seen_cache[start_x][start_y] = seen_cache_id;
    
    long long res = 0;
    long long steps = 1; // Counter for nodes enqueued so far

    while (!q.empty()) {
        pair<int, int> curr = q.front();
        q.pop();
        int x = curr.first;
        int y = curr.second;
        
        res += steps;

        for (const auto& dir : DIRECTIONS) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (nx < 0 || ny < 0 || nx >= R || ny >= C) continue;
            if (maze[nx][ny] != MAZE_CHAR || seen_cache[nx][ny] == seen_cache_id) continue;
            
            seen_cache[nx][ny] = seen_cache_id;
            q.push({nx, ny});
            steps++;
        }
    }
    return res;
}

long long compute_quality(const vector<string>& maze) {
    long long total_quality = 0;
    for (int x = 0; x < R; ++x) {
        for (int y = 0; y < C; ++y) {
            if (maze[x][y] == MAZE_CHAR) {
                total_quality += runtimes_from(maze, x, y);
            }
        }
    }
    return total_quality;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    cskap(argc == 2, "Arguments should be <contestant_output>");
    const char* fn_contestant = argv[1];

    ifstream f(fn_contestant);
    cskap(f.is_open(), "Could not open output file.");
    
    string first_line;
    try {
        cskap(static_cast<bool>(getline(f, first_line)), "Could not parse r, c in first line");
        stringstream ss(first_line);
        cskap(ss >> R && ss >> C, "Could not parse r, c in first line");
    } catch (...) {
        skap("Could not parse r, c in first line");
    }

    cskap(R >= 1 && R <= MAX_RC, "r is out of bounds: " + to_string(R));
    cskap(C >= 1 && C <= MAX_RC, "c is out of bounds: " + to_string(C));

    vector<string> maze = parse_maze(f, R, C);
    long long quality = compute_quality(maze);

    long long score = quality;
    score /= 100'000'000;
    score -= 626;
    score *= 2;
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    cout << score << endl << "OK" << endl;
    return 0;
}
