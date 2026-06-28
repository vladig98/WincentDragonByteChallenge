#include<bits/stdc++.h>
// #include "../../testlib.h"

using namespace std;

#define FOR(i,n)	for(int i=0;i<(int)n;i++)
#define FOB(i,n)	for(int i=n;i>=1;i--)
#define MP(x,y)	make_pair((x),(y))
#define ii pair<int, int>
#define lli long long int
#define ld long double
#define ulli unsigned long long int
#define lili pair<lli, lli>
#ifdef EBUG
#define DBG	if(1)
#else
#define DBG	if(0)
#endif
#define SIZE(x) int(x.size())
const int infinity = 2000000999 / 2;
const long long int inff = 4000000000000000999;

typedef complex<long double> point;

template<class T>
T get() {
    T a;
    cin >> a;
    return a;
}

template <class T, class U>
ostream& operator<<(ostream& out, const pair<T, U> &par) {
    out << "[" << par.first << ";" << par.second << "]";
    return out;
}

template <class T>
ostream& operator<<(ostream& out, const set<T> &cont) {
    out << "{";
    for (const auto &x:cont) out << x << ", ";
    out << "}";
    return out;
}

template <class T, class U>
ostream& operator<<(ostream& out, const map<T,U> &cont) {
    out << "{";
    for (const auto &x:cont) out << x << ", ";
    
    out << "}"; return out;
}

template <class T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    FOR(i, v.size()){
        if(i) out << " ";
        out << v[i];
    }
    out << endl;
    return out;
}

bool ccw(point p, point a, point b) {
    if((conj(a - p) * (b - p)).imag() <= 0) return false;
    else return true;
}

int main(int argc, char *argv[]) {
    cin.sync_with_stdio(false);
    cout.sync_with_stdio(false);
    
    string user_out = argv[3], task_in = argv[1], valid_out = argv[2];
    ifstream usr(user_out, ifstream::in);
    ifstream task(task_in, ifstream::in);
    ifstream val(valid_out, ifstream::in);
    
    bool test_orientability = true;
    if (task_in.substr(SIZE(task_in) - 4, 4)  == "2.in" || (SIZE(task_in) >= 15 && task_in.substr(SIZE(task_in) - 15, 15) == "input_medium.in")) {
        test_orientability = false;
    }
    
    int T;
    task >> T;
    
    for (int ti = 1; ti <= T; ti++) {
        int n, m;
        task >> n >> m;
        int cost_usr, cost_val;
        string cost_usr_str;
        val >> cost_val;
        usr >> cost_usr_str;

        stringstream ss(cost_usr_str);
        ss >> cost_usr;
        if (!ss) {
            cerr << "WA: test case " << ti << ": missing number of turns, got '" << (cost_usr_str.size() <= 10 ? cost_usr_str : (cost_usr_str.substr(0, 10) + "...")) << "'" << endl;
            exit(1);
        }
        
        if (cost_val != cost_usr) {
            cerr << "WA: test case " << ti << ": wrong number of turns, got " << cost_usr << " expected " << cost_val << endl;
            exit(1);
        }
        
        vector<string> real(n), out(n);
        int diff = 0;
        FOR(i, n) {
            string temp;
            val >> temp;
            usr >> out[i];
            if (out[i].size() != m) {
                cerr << "WA: test case " << ti << ": line must consist of exactly m characters" << endl;
                exit(1);
            }
            for (auto ch : out[i]) {
                switch (ch) {
                    case '>':
                    case 'v':
                    case '<':
                    case '^':
                        break;
                    default:
                    {
                        cerr << "WA: test case " << ti << ": line must consist only of characters >v<^" << endl;
                        exit(1);
                    }
                }
            }
            task >> real[i];
        }
        FOR(i, n) {
            FOR(j, m) {
                bool allowed;
                task >> allowed;
                if (out[i][j] != real[i][j] && test_orientability) {
                    diff ++;
                    if (!allowed) {
                        cerr << "WA: test case " << ti << ": change not allowed" << endl;
                        exit(1);
                    }
                }
            }
        }
        if (diff != cost_usr && test_orientability) {
            cerr << "WA: test case " << ti << ": claimed number of changes does not match actual number of changes" << endl;
            exit(1);
        }
        
        if (!test_orientability) continue;
        
        vector<vector<int> > visited(n, vector<int>(m, -1));
        unordered_map<char, int> DX, DY;
        DY['>'] = 1; DX['>'] = 0;
        DY['<'] = -1; DX['<'] = 0;
        DY['^'] = 0; DX['^'] = -1;
        DY['v'] = 0; DX['v'] = 1;
        
        int t = 0;
        FOR(i, n) {
            FOR(j, m) {
                if (visited[i][j] != -1) {
                    DBG cout << "Going ";
                    int x = i, y = j;
                    while ((x >= 0 && x < n && y >= 0 && y < m) && visited[x][y] == -1) {
                        DBG cout << "(" << x << ", " << y << ") -> ";
                        visited[x][y] = t;
                        char c = out[x][y];
                        x += DX[c];
                        y += DY[c];
                    }
                    DBG cout << "(" << x << ", " << y << ")\n";
                    if (x >= 0 && x < n && y >= 0 && y < m && visited[x][y] == t) {
                        cerr << "WA: test case " << ti << ": loop detected" << endl;
                        DBG cout << "From " << i << " " << j << " returned to " << x << "; " << y << endl;
                        exit(1);
                    }
                }
                t ++;
            }
        }
        
    }
    
    cerr << 1 << endl;  // TODO podľa skutočného bodovania
    cerr << "OK" << endl;
   
}
