#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

struct TropicalLongLong {
  static constexpr long long MAX_VALUE = numeric_limits<long long>::max() / 2;

  long long value = MAX_VALUE;

  explicit operator bool() const { return value != MAX_VALUE; }

  void operator+=(const TropicalLongLong& other) {
    value = min(value, other.value);
  }

  TropicalLongLong operator+(const TropicalLongLong& other) const {
    return {min(value, other.value)};
  }

  TropicalLongLong operator*(const TropicalLongLong& other) const {
    return {min(value + other.value, MAX_VALUE)};
  }
};

template <typename T>
struct Vec {
  vector<T> v;

  Vec() {}
  Vec(int n) : v(n, T{}) {}

  T dot(const Vec<T>& other) const {
    assert(v.size() == other.v.size());
    T result = {};
    for (int i = 0; i < v.size(); i++) {
      result += v[i] * other.v[i];
    }
    return result;
  }

  Vec<T> operator*(const Vec<T>& other) const {
    assert(v.size() == other.v.size());
    Vec<T> result(v.size());
    for (int i = 0; i < v.size(); i++) {
      result.v[i] = v[i] * other.v[i];
    }
    return result;
  }
};

vector<int> getSignature(const Vec<TropicalLongLong>& v) {
  vector<int> result(v.v.size());
  long long minValue = numeric_limits<int>::max();
  for (auto x : v.v) {
    minValue = min(minValue, x.value);
  }
  for (int i = 0; i < v.v.size(); i++) {
    result[i] = (v.v[i].value >= TropicalLongLong::MAX_VALUE)
                    ? -1
                    : int(v.v[i].value - minValue);
  }
  return result;
}

template <typename T>
struct Mat {
  vector<vector<T>> M;

  Mat() {}
  Mat(int n) : M(n, vector<T>(n, T{})) {}

  Vec<T> operator*(const Vec<T>& v) const {
    int n = M.size();
    assert(v.v.size() == n);
    Vec<T> result(n);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        result.v[i] += M[i][j] * v.v[j];
      }
    }
    return result;
  }

  Mat<T> operator*(const Mat<T>& r) const {
    int n = M.size();
    assert(n == r.M.size());
    Mat<T> result(n);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
          result.M[i][k] += M[i][j] * r.M[j][k];
        }
      }
    }
    return result;
  }
};

typedef Vec<TropicalLongLong> TropicalVec;
typedef Mat<TropicalLongLong> TropicalMat;

const int dr[4] = {0, 1, 0, -1};
const int dc[4] = {1, 0, -1, 0};

bool checkSingleRow(int c, int row) {
  for (int ci = 0; ci < c; ci++) {
    bool value = bool(row & (1 << ci));
    bool neighborsWith[2] = {false, false};
    for (int dir = 0; dir < 4; dir++) {
      int ri2 = dr[dir];
      if (ri2 != 0) {
        continue;
      }
      int ci2 = ci + dc[dir];
      if (ci2 < 0 || ci2 >= c) {
        continue;
      }
      bool neighborValue = bool(row & (1 << ci2));
      neighborsWith[neighborValue] = true;
    }
    if (value) {
      if (neighborsWith[0] && neighborsWith[1]) {
        return false;
      }
    } else {
      if (!neighborsWith[0] || !neighborsWith[1]) {
        return false;
      }
    }
  }
  return true;
}

bool checkFirstRow(int c, const int rows[2]) {
  for (int ci = 0; ci < c; ci++) {
    bool value = bool(rows[0] & (1 << ci));
    bool neighborsWith[2] = {false, false};
    for (int dir = 0; dir < 4; dir++) {
      int ri2 = dr[dir];
      if (ri2 < 0 || ri2 > 1) {
        continue;
      }
      int ci2 = ci + dc[dir];
      if (ci2 < 0 || ci2 >= c) {
        continue;
      }
      bool neighborValue = bool(rows[ri2] & (1 << ci2));
      neighborsWith[neighborValue] = true;
    }
    if (value) {
      if (neighborsWith[0] && neighborsWith[1]) {
        return false;
      }
    } else {
      if (!neighborsWith[0] || !neighborsWith[1]) {
        return false;
      }
    }
  }
  return true;
}

bool checkFirstRow(int c, int row0, int row1) {
  int rows[2] = {row0, row1};
  return checkFirstRow(c, rows);
}

bool checkOnes(int c, const int rows[2]) {
  for (int ri = 0; ri < 2; ri++) {
    for (int ci = 0; ci < c; ci++) {
      bool value = bool(rows[ri] & (1 << ci));
      if (value) {
        bool neighborsWith[2] = {false, false};
        for (int dir = 0; dir < 4; dir++) {
          int ri2 = ri + dr[dir];
          if (ri2 < 0 || ri2 > 1) {
            continue;
          }
          int ci2 = ci + dc[dir];
          if (ci2 < 0 || ci2 >= c) {
            continue;
          }
          bool neighborValue = bool(rows[ri2] & (1 << ci2));
          neighborsWith[neighborValue] = true;
        }
        if (neighborsWith[0] && neighborsWith[1]) {
          return false;
        }
      }
    }
  }
  return true;
}

bool checkMiddleRow(int c, const int rows[3]) {
  for (int ci = 0; ci < c; ci++) {
    bool value = bool(rows[1] & (1 << ci));
    bool neighborsWith[2] = {false, false};
    for (int dir = 0; dir < 4; dir++) {
      int ri2 = 1 + dr[dir];
      int ci2 = ci + dc[dir];
      if (ci2 < 0 || ci2 >= c) {
        continue;
      }
      bool neighborValue = bool(rows[ri2] & (1 << ci2));
      neighborsWith[neighborValue] = true;
    }
    if (value) {
      if (neighborsWith[0] && neighborsWith[1]) {
        return false;
      }
    } else {
      if (!neighborsWith[0] || !neighborsWith[1]) {
        return false;
      }
    }
  }
  return true;
}

int countNumOnes(int row) {
  int result = 0;
  while (row > 0) {
    result += row % 2;
    row /= 2;
  }
  return result;
}

bool matches(int row, string pattern) {
  int c = pattern.size();
  for (int i = 0; i < c; i++) {
    bool value = bool(row & (1 << i));
    switch (pattern[i]) {
      case '0': {
        if (value) {
          return false;
        }
        break;
      }
      case '1': {
        if (!value) {
          return false;
        }
        break;
      }
      case '*': {
        break;
      }
      default: {
        assert(false);
      }
    }
  }
  return true;
}

struct TwoRows {
  int c = 0;
  int rows[2] = {};

  TwoRows() {}
  TwoRows(int c0, int rowsInit[2]) : c(c0) {
    rows[0] = rowsInit[0];
    rows[1] = rowsInit[1];
  }
  TwoRows(int c0, int row0, int row1) : c(c0) {
    rows[0] = row0;
    rows[1] = row1;
  }

  static TwoRows decode(int c, int code) {
    TwoRows result;
    result.c = c;
    result.rows[0] = code >> c;
    result.rows[1] = code & ((1 << c) - 1);
    return result;
  }

  int countNumOnes() const {
    return ::countNumOnes(rows[0]) + ::countNumOnes(rows[1]);
  }

  int encode() const { return (rows[0] << c) | rows[1]; }

  bool isValid() const { return checkOnes(c, rows); }

  bool isValidFirstRow() const { return checkFirstRow(c, rows); }

  bool isValidLastRow() const { return checkFirstRow(c, rows[1], rows[0]); }

  TwoRows then(int row2) const { return TwoRows(c, rows[1], row2); }

  bool canExtendWith(int row2) const {
    int cand_rows[3] = {rows[0], rows[1], row2};
    if (!checkMiddleRow(c, cand_rows)) {
      return false;
    }
    return then(row2).isValid();
  }
};

vector<TwoRows> getValidStates(int c) {
  vector<TwoRows> result;
  for (int row1 = 0; row1 < (1 << c); row1++) {
    for (int row2 = 0; row2 < (1 << c); row2++) {
      TwoRows cand(c, row1, row2);
      if (cand.isValid()) {
        result.push_back(cand);
      }
    }
  }
  return result;
}

class Solver {
 protected:
  int c;
  vector<TwoRows> states;

  TropicalMat m;
  vector<TropicalMat> mSmallPowers;
  TropicalVec vStart, vEnd;

  TropicalVec getMatchingStatesVec(int ri, string pattern) const {
    assert(0 <= ri && ri <= 1);
    assert(pattern.size() == c);
    TropicalVec result(states.size());
    for (int i = 0; i < states.size(); i++) {
      if (matches(states[i].rows[ri], pattern)) {
        result.v[i] = {0};
      }
    }
    return result;
  }

 public:
  Solver() {}
  Solver(int c0, int mSmallPowersMax = 1) : c(c0) {
    states = getValidStates(c);
    unordered_map<int, int> codeToIndex;
    for (int i = 0; i < states.size(); i++) {
      codeToIndex[states[i].encode()] = i;
    }

    m = {states.size()};
    for (int i = 0; i < states.size(); i++) {
      for (int row2 = 0; row2 < (1 << c); row2++) {
        if (states[i].canExtendWith(row2)) {
          TwoRows ext = states[i].then(row2);
          int extIdx = codeToIndex[ext.encode()];
          m.M[extIdx][i] += TropicalLongLong{countNumOnes(row2)};
        }
      }
    }

    mSmallPowersMax = max(mSmallPowersMax, 1);
    mSmallPowers.resize(mSmallPowersMax + 1);
    mSmallPowers[1] = m;
    for (int i = 2; i <= mSmallPowersMax; i++) {
      mSmallPowers[i] = mSmallPowers[i - 1] * m;
    }

    vStart = {states.size()};
    for (int i = 0; i < states.size(); i++) {
      if (states[i].isValidFirstRow()) {
        vStart.v[i] += TropicalLongLong{states[i].countNumOnes()};
      }
    }

    vEnd = {states.size()};
    for (int i = 0; i < states.size(); i++) {
      if (states[i].isValidLastRow()) {
        vEnd.v[i] = TropicalLongLong{0};
      }
    }
  }

  long long solveSingleRow(string patternStart, string patternEnd) const {
    assert(patternStart.size() == c);
    assert(patternEnd.size() == c);
    TropicalLongLong best = {};
    for (int cand = 0; cand < (1 << c); cand++) {
      if (checkSingleRow(c, cand) && matches(cand, patternStart) &&
          matches(cand, patternEnd)) {
        best += TropicalLongLong{countNumOnes(cand)};
      }
    }
    return best.value;
  }

  pair<int, int> detectPeriodicity(TropicalVec v) const {
    map<vector<int>, int> T;
    auto sgn = getSignature(v);
    for (int i = 0;; i++) {
      sgn.pop_back();  // very last entry is a loop: 11111 requires all 1s
                       // in both previous and next rows.
      auto it = T.find(sgn);
      if (it == T.end()) {
        T[sgn] = i;
        v = m * v;
        sgn = getSignature(v);
      } else {
        return {it->second, i};
      }
    }
  }

  TropicalVec solvePeriodicMulHelper(int exp, TropicalVec v) const {
    if (exp == 0) {
      return v;
    }
    if (exp < mSmallPowers.size()) {
      return mSmallPowers[exp] * v;
    }
    v = mSmallPowers.back() * v;
    exp -= (mSmallPowers.size() - 1);
    while (exp > 0) {
      v = m * v;
      exp--;
    }
    return v;
  }

  long long solvePeriodic(long long r, string patternStart,
                          string patternEnd) const {
    if (r == 0) {
      return 0;
    }
    if (r == 1) {
      return solveSingleRow(patternStart, patternEnd);
    }
    assert(r >= 2);
    r -= 2;
    TropicalVec v = getMatchingStatesVec(0, patternStart) * vStart;
    TropicalVec vPatternEnd = getMatchingStatesVec(1, patternEnd);
    pair<int, int> p = detectPeriodicity(v);
    if (r <= p.first) {
      v = solvePeriodicMulHelper(r, v);
      return vEnd.dot(vPatternEnd * v).value;
    }
    int cycleLength = p.second - p.first;
    int phase = (r - p.first) % cycleLength;
    long long numCycles = (r - p.first) / cycleLength;
    v = solvePeriodicMulHelper(p.first + phase, v);
    TropicalVec vPhase0 = v;
    v = solvePeriodicMulHelper(cycleLength, v);
    TropicalVec vPhase1 = v;

    // this also (hackily) solves the case of 11111... since that
    // one has periodicity 1, and 1 divides any number
    vector<long long> diffPerCycle(v.v.size());
    for (int i = 0; i < v.v.size(); i++) {
      assert(bool(vPhase0.v[i]) == bool(vPhase1.v[i]));
      diffPerCycle[i] =
          bool(vPhase0.v[i]) ? (vPhase1.v[i].value - vPhase0.v[i].value) : 0;
    }
    v = vPhase0;
    for (int i = 0; i < v.v.size(); i++) {
      v.v[i].value += diffPerCycle[i] * numCycles;
    }
    return vEnd.dot(vPatternEnd * v).value;
  }
};

long long wrapForDisplay(long long result) {
  return (result == TropicalLongLong::MAX_VALUE ? -1 : result);
}

int main() {
  int t;
  cin >> t;

  Solver solvers[7];
  for (int i = 0; i < 7; i++) {
    solvers[i] = Solver(i + 1, (t >= 10000 ? 30 : 1));
  }
  for (int ti = 0; ti < t; ti++) {
    // cerr << ti + 1 << "/" << t << "\n";
    int c;
    long long r;
    string p, q;
    cin >> c >> r >> p >> q;
    long long result = solvers[c - 1].solvePeriodic(r, p, q);
    cout << wrapForDisplay(result) << "\n";
  }
  return 0;
}