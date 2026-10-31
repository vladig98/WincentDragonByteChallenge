#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    int t;
    cin >> t;
    while (t--) {
        int n, b, m;
        string C;
        cin >> n >> b >> m >> C;

        auto W = [&](ll s) -> ll {
            if (m == 1) return 0;
            if (m == 2) return s * (s - 1) / 2;
            return 5 * (s * (s - 1) * (s - 2) / 6) + s * (s - 1) / 2 * (n - s);
        };

        vector<int> P = {0};
        for (int i = 0; i < n - 1; i++)
            if (C[i] == '-') P.push_back(i + 1);
        P.push_back(n);
        int B = P.size() - 1, r = B - b;

        vector<ll> prev(B + 1), cur(B + 1);
        vector<vector<int>> par(r + 1);
        for (int i = 1; i <= min(B, 1 + b); i++) cur[i] = W(P[i]);

        for (int p = 2; p <= r; p++) {
            swap(prev, cur);
            int hi = min(B, p + b);
            par[p].resize(hi - p + 1);
            auto F = [&](int j, int i) { return prev[j] + W(P[i] - P[j]); };
            vector<pair<int, int>> st;
            for (int i = p; i <= hi; i++) {
                while (!st.empty() && st.back().second < i) st.pop_back();
                int j = i - 1;
                while (!st.empty() && F(j, st.back().second) > F(st.back().first, st.back().second))
                    st.pop_back();
                if (st.empty()) {
                    st.push_back({j, hi});
                } else if (F(j, i) > F(st.back().first, i)) {
                    int lo = i, up = st.back().second;
                    while (lo < up) {
                        int mid = (lo + up + 1) / 2;
                        if (F(j, mid) > F(st.back().first, mid)) lo = mid;
                        else up = mid - 1;
                    }
                    st.push_back({j, lo});
                }
                int k = st.back().first;
                cur[i] = F(k, i);
                par[p][i - p] = k;
            }
        }

        string res(n - 1, 'C');
        for (int p = r, i = B; p >= 2; p--) {
            i = par[p][i - p];
            res[P[i] - 1] = '-';
        }
        cout << res << '\n';
    }
}
