#include <cassert>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>
using namespace std;

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

vector<int> getValidFirstRows(int c) {
  vector<int> result;
  for (int row1 = 0; row1 < (1 << c); row1++) {
    for (int row2 = 0; row2 < (1 << c); row2++) {
      TwoRows cand(c, row1, row2);
      if (cand.isValidFirstRow()) {
        result.push_back(row1);
        break;
      }
    }
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

long long solveSingleRow(int c, string pattern1, string pattern2) {
  long long best = numeric_limits<long long>::max();
  for (int row = 0; row < (1 << c); row++) {
    if (checkSingleRow(c, row) && matches(row, pattern1) &&
        matches(row, pattern2)) {
      best = min(best, (long long)countNumOnes(row));
    }
  }
  return (best == numeric_limits<long long>::max() ? -1 : best);
}

vector<unordered_map<int, long long>> precompute(int c, int firstRow,
                                                 int maxR) {
  assert(maxR >= 2);
  vector<unordered_map<int, long long>> dp;
  dp.resize(maxR + 1);
  for (int row2 = 0; row2 < (1 << c); row2++) {
    TwoRows state(c, firstRow, row2);
    if (!state.isValid() || !state.isValidFirstRow()) {
      continue;
    }
    int code = state.encode();
    auto entry = dp[2].find(code);
    if (entry == dp[2].end()) {
      dp[2][code] = state.countNumOnes();
    } else {
      entry->second = min(entry->second, (long long)state.countNumOnes());
    }
  }
  for (int ri = 3; ri <= maxR; ri++) {
    for (auto entry : dp[ri - 1]) {
      TwoRows state = TwoRows::decode(c, entry.first);
      for (int nrow = 0; nrow < (1 << c); nrow++) {
        if (!state.canExtendWith(nrow)) {
          continue;
        }
        TwoRows nstate = state.then(nrow);
        int ncode = nstate.encode();
        long long ncount = entry.second + countNumOnes(nrow);
        auto nentry = dp[ri].find(ncode);
        if (nentry == dp[ri].end()) {
          dp[ri][ncode] = ncount;
        } else {
          nentry->second = min(nentry->second, ncount);
        }
      }
    }
  }

  // we are only interested in matching the last row; do a projection

  vector<unordered_map<int, long long>> result;
  result.resize(maxR + 1);

  for (int ri = 2; ri <= maxR; ri++) {
    for (auto entry : dp[ri]) {
      TwoRows state = TwoRows::decode(c, entry.first);
      if (state.isValidLastRow()) {
        int lastRow = state.rows[1];
        auto lastRowEntry = result[ri].find(lastRow);
        if (lastRowEntry == result[ri].end()) {
          result[ri][lastRow] = entry.second;
        } else {
          lastRowEntry->second = min(lastRowEntry->second, entry.second);
        }
      }
    }
  }

  return result;
}

struct Solver {
  int c;
  unordered_map<int, vector<unordered_map<int, long long>>> answers;

  void init(int c0, int maxR) {
    c = c0;
    vector<int> validFirstRows = getValidFirstRows(c);
    for (int i = 0; i < validFirstRows.size(); i++) {
      cerr << "precomputing c = " << c << ", " << i + 1 << "/";
      cerr << validFirstRows.size() << "\n";
      int firstRow = validFirstRows[i];
      answers[firstRow] = precompute(c, firstRow, maxR);
    }
  }

  long long solve(int r, string patternStart, string patternEnd) {
    if (r == 1) {
      return solveSingleRow(c, patternStart, patternEnd);
    }
    long long best = numeric_limits<long long>::max();
    for (const auto& entry : answers) {
      if (!matches(entry.first, patternStart)) {
        continue;
      }
      assert(r < entry.second.size());
      for (auto lastRowEntry : entry.second[r]) {
        if (!matches(lastRowEntry.first, patternEnd)) {
          continue;
        }
        best = min(best, lastRowEntry.second);
      }
    }
    return (best == numeric_limits<long long>::max() ? -1 : best);
  }
};

int main() {
  int t;
  cin >> t;

  struct Query {
    int c, r;
    string p, q;
  };

  vector<Query> qs;
  int maxR = 0;
  for (int ti = 0; ti < t; ti++) {
    int c;
    long long r;
    string p, q;
    cin >> c >> r >> p >> q;
    assert(r <= 1000000);
    qs.push_back({c, (int)r, p, q});
    maxR = max(maxR, (int)r);
  }

  Solver solvers[8];
  for (int c = 1; c <= 7; c++) {
    solvers[c].init(c, maxR);
  }

  for (const auto& query : qs) {
    long long result = solvers[query.c].solve(query.r, query.p, query.q);
    cout << result << "\n";
  }

  return 0;
}