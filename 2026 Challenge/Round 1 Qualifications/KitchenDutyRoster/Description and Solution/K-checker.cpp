#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>

using namespace std;

long long nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    if (r > n / 2) r = n - r;
    long long res = 1;
    for (int i = 1; i <= r; ++i) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <contestants_output_file>\n";
        return 2;
    }

    ifstream fin(argv[1]);
    if (!fin) {
        cerr << "Error: Cannot open input file " << argv[1] << "\n";
        return 2;
    }

    ifstream fout(argv[2]);
    if (!fout) {
        cerr << "Error: Cannot open contestants output file " << argv[2] << "\n";
        return 2;
    }

    int t;
    if (!(fin >> t)) {
        cerr << "Error: Cannot read number of test cases from input file\n";
        return 2;
    }

    for (int tc = 1; tc <= t; ++tc) {
        int n, k, p;
        if (!(fin >> n >> k >> p)) {
            cerr << "Error: Missing input data for test case " << tc << "\n";
            return 2;
        }

        long long optimal_d = 0;
        if (n < 2 * k) {
            optimal_d = nCr(n, k);
        } else {
            optimal_d = nCr(n - 1, k - 1);
        }

        long long contestant_d;
        if (!(fout >> contestant_d)) {
            cout << "Wrong answer: failed to read d in test case " << tc << "\n";
            return 1;
        }

        if (contestant_d != optimal_d) {
            // cout << "Wrong answer in test case " << tc << ": expected d=" << optimal_d << ", found " << contestant_d << "\n";
            cout << "Wrong answer: incorrect value of d.\n";
            return 1;
        }

        if (p == 1) {
            unordered_set<unsigned long long> seen;
            vector<unsigned long long> crews;
            crews.reserve(contestant_d);

            for (long long i = 0; i < contestant_d; ++i) {
                unsigned long long mask = 0;
                
                for (int j = 0; j < k; ++j) {
                    int bit;
                    if (!(fout >> bit)) {
                        cout << "Wrong answer: failed to read rosters in test case " << tc << "\n";
                        return 1;
                    }
                    if (bit < 0 || bit >= n) {
                        cout << "Wrong answer in test case " << tc << ": invalid person " << bit << "\n";
                        return 1;
                    }
                    if (mask & (1ULL << bit)) {
                        cout << "Wrong answer in test case " << tc << ": duplicate person in crew\n";
                        return 1;
                    }
                    mask |= (1ULL << bit);
                }

                if (!seen.insert(mask).second) {
                    cout << "Wrong answer in test case " << tc << ": two days with same crew\n";
                    return 1;
                }
                
                crews.push_back(mask);
            }

            if (n >= 2 * k) {
                for (size_t i = 0; i < crews.size(); ++i) {
                    for (size_t j = i + 1; j < crews.size(); ++j) {
                        if ((crews[i] & crews[j]) == 0) {
                            cout << "Wrong answer in test case " << tc << ": disjoint crews\n";
                            return 1;
                        }
                    }
                }
            }
        }
    }

    cout << "OK\n";
    return 0;
}
