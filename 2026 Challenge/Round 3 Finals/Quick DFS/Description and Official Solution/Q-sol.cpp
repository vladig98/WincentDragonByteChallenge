#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Slab { ll a, b, lo, hi; };
struct Phase { ll c0, c1, lo, hi, t0; int xd, v0; };

vector<Slab> SL;
vector<Phase> PH;
vector<int> STK;
map<ll, ll> part;
ll TL, TR, TOTAL;

int slabIdx(ll x) {
    int i = upper_bound(SL.begin(), SL.end(), x, [](ll v, const Slab &s) { return v < s.a; }) - SL.begin() - 1;
    return (i >= 0 && x <= SL[i].b) ? i : -1;
}

bool unvis(ll x, ll &a, ll &b) {
    int i = slabIdx(x);
    if (i < 0) return false;
    a = SL[i].lo; b = SL[i].hi;
    if (x < TL || x > TR) return true;
    auto it = part.find(x);
    if (it == part.end()) return false;
    b = it->second - 1;
    return true;
}

bool isFree(ll x, ll y) { ll a, b; return unvis(x, a, b) && a <= y && y <= b; }

ll plen(const Phase &p) { return (llabs(p.c1 - p.c0) + 1) * (p.hi - p.lo + 1); }

bool goesUp(const Phase &p, ll j) { return (j % 2 == 0) == (p.v0 > 0); }

pair<ll, ll> cellAt(const Phase &p, ll off) {
    ll h = p.hi - p.lo + 1, j = off / h, pos = off % h;
    return {p.c0 + p.xd * j, goesUp(p, j) ? p.lo + pos : p.hi - pos};
}

ll timeIn(const Phase &p, ll x, ll y) {
    ll h = p.hi - p.lo + 1, j = (x - p.c0) * p.xd;
    return p.t0 + j * h + (goesUp(p, j) ? y - p.lo : p.hi - y);
}

void emit(ll c0, ll c1, int xd, ll lo, ll hi, int v0) {
    PH.push_back({c0, c1, lo, hi, TOTAL + 1, xd, v0});
    TOTAL += plen(PH.back());
    STK.push_back(PH.size() - 1);
}

bool frontier(const Phase &p, ll &bx, ll &by) {
    ll bt = -1;
    auto cand = [&](ll x, ll y) { ll t = timeIn(p, x, y); if (t > bt) { bt = t; bx = x; by = y; } };
    auto side = [&](ll x, ll nb) {
        ll a, b;
        if (!unvis(nb, a, b)) return;
        a = max(a, p.lo); b = min(b, p.hi);
        if (a <= b) cand(x, goesUp(p, (x - p.c0) * p.xd) ? b : a);
    };
    side(p.c0, p.c0 - p.xd);
    side(p.c1, p.c1 + p.xd);
    if (isFree(p.c0, p.lo - 1)) cand(p.c0, p.lo);
    return bt >= 0;
}

void simulate(ll area) {
    ll cx = 0, cy = SL[slabIdx(0)].hi;
    TL = TR = 0;
    emit(0, 0, 1, 0, cy, 1);
    if (SL[slabIdx(0)].lo < 0) part[0] = 0;
    while (TOTAL < area) {
        if (isFree(cx, cy - 1)) {
            ll lo = SL[slabIdx(cx)].lo;
            emit(cx, cx, 1, lo, cy - 1, -1);
            part.erase(cx);
            cy = lo;
            continue;
        }
        int xd = isFree(cx + 1, cy) ? 1 : isFree(cx - 1, cy) ? -1 : 0;
        if (!xd) {
            while (!frontier(PH[STK.back()], cx, cy)) STK.pop_back();
            continue;
        }
        ll nx = cx + xd;
        const Slab &s = SL[slabIdx(nx)];
        if ((nx < TL || nx > TR) && (cy == s.lo || cy == s.hi)) {
            int v0 = cy == s.lo ? 1 : -1;
            ll lim = xd > 0 ? s.b : s.a;
            if (xd < 0 && isFree(cx, s.lo)) lim = nx;
            emit(nx, lim, xd, s.lo, s.hi, v0);
            TL = min(TL, lim); TR = max(TR, lim);
            cx = lim;
            cy = goesUp(PH.back(), llabs(lim - nx)) ? s.hi : s.lo;
        } else {
            ll top = (nx < TL || nx > TR) ? s.hi : part[nx] - 1;
            emit(nx, nx, 1, cy, top, 1);
            TL = min(TL, nx); TR = max(TR, nx);
            if (cy == s.lo) part.erase(nx); else part[nx] = cy;
            cx = nx; cy = top;
        }
    }
}

void buildRegion(vector<array<ll, 4>> &ev) {
    sort(ev.begin(), ev.end());
    multiset<ll> lows, highs;
    for (size_t i = 0; i < ev.size();) {
        ll x = ev[i][0];
        for (; i < ev.size() && ev[i][0] == x; i++) {
            if (ev[i][1] == 1) { lows.insert(ev[i][2]); highs.insert(ev[i][3]); }
            else { lows.erase(lows.find(ev[i][2])); highs.erase(highs.find(ev[i][3])); }
        }
        if (lows.empty()) continue;
        ll lo = *lows.begin(), hi = *highs.rbegin();
        if (!SL.empty() && SL.back().lo == lo && SL.back().hi == hi) SL.back().b = ev[i][0] - 1;
        else SL.push_back({x, ev[i][0] - 1, lo, hi});
    }
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int t;
    cin >> t;
    string out;
    while (t--) {
        int n, q;
        cin >> n >> q;
        SL.clear(); PH.clear(); STK.clear(); part.clear(); TOTAL = 0;
        vector<array<ll, 4>> ev;
        for (int i = 0; i < n; i++) {
            ll x1, y1, x2, y2;
            cin >> x1 >> y1 >> x2 >> y2;
            ev.push_back({x1, 1, y1, y2});
            ev.push_back({x2 + 1, -1, y1, y2});
        }
        buildRegion(ev);
        ll area = 0;
        for (auto &s : SL) area += (s.b - s.a + 1) * (s.hi - s.lo + 1);
        simulate(area);

        map<pair<ll, ll>, int> single;
        map<ll, int> multi;
        for (int i = 0; i < (int)PH.size(); i++) {
            if (PH[i].c0 == PH[i].c1) single[{PH[i].c0, PH[i].lo}] = i;
            else multi[min(PH[i].c0, PH[i].c1)] = i;
        }
        while (q--) {
            int type;
            cin >> type;
            if (type == 1) {
                ll k;
                cin >> k;
                int i = upper_bound(PH.begin(), PH.end(), k, [](ll v, const Phase &p) { return v < p.t0; }) - PH.begin() - 1;
                auto c = cellAt(PH[i], k - PH[i].t0);
                out += to_string(c.first) + " " + to_string(c.second) + "\n";
            } else {
                ll x, y;
                cin >> x >> y;
                auto it = multi.upper_bound(x);
                int i;
                if (it != multi.begin() && max(PH[prev(it)->second].c0, PH[prev(it)->second].c1) >= x) i = prev(it)->second;
                else i = prev(single.upper_bound({x, y}))->second;
                out += to_string(timeIn(PH[i], x, y)) + "\n";
            }
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
}
