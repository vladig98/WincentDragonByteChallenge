#include <bits/stdc++.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/gmp.hpp>
using namespace std;
using BigRational = boost::multiprecision::cpp_rational;
// using number = long double;
using number = boost::multiprecision::mpf_float_100;

int N, K;
number P;
vector< vector<number> > convince_probability;

void calculate_convince_probability() {
    /* if we talk to *a* people with a different opinion from ours
       and convince each with probability *p*
       what is the probability of convincing exactly *b* of them? */
    convince_probability.clear();
    convince_probability.resize( K+1, vector<number>(K+1, number(0)) );
    convince_probability[0][0] = number(1);
    for (int a=1; a<=K; ++a) for (int b=0; b<=a; ++b) {
        convince_probability[a][b] = (1-P) * convince_probability[a-1][b];
        if (b > 0) convince_probability[a][b] += P * convince_probability[a-1][b-1];
    }
}

vector< vector< vector<number> > > select_probability_distribution;
/* select_probability_distribution[good][pick]
 * is the probability distribution for when we're picking from N-1-(K - *pick*) people
 * such that *good* of them are good
 * more precisely, entry [good][pick][j] is the probability of picking exactly j good ones
 */

void calculate_select_probability_distribution() {
    select_probability_distribution.clear();
    select_probability_distribution.resize(N);
    for (int n=0; n<N; ++n) select_probability_distribution[n].resize(K+1);
    for (int n=0; n<N; ++n) select_probability_distribution[n][0] = vector<number>(1, 1); // for sure picking 0 good when picking 0 total
    for (int pick=1; pick<=K; ++pick) {
        for (int n=0; n<N; ++n) {
            select_probability_distribution[n][pick] = vector<number>(pick+1, 0);
            number first_picked_good = number(n) / (N-1-K+pick);
            for (int i=0; i<pick; ++i) {
                if (n > 0) select_probability_distribution[n][pick][i+1] += first_picked_good * select_probability_distribution[n-1][pick-1][i];
                select_probability_distribution[n][pick][i] += (1 - first_picked_good) * select_probability_distribution[n][pick-1][i];
            }
        }
    }
}

vector<number> transition_probabilities(int x) {
    /* among N people, *x* currently hold our opinion (0 < *x* < N)
       for each i from x-K to x+K determine the probability that next day there will be i people with our opinion */
    
    number select_good_speaker = number(x) / N;
    
    vector<number> answers( 2*K+1, number(0) );
    answers[K] = number(1);

    vector<number> add = select_probability_distribution[x-1][K];
    for (int i=0; i<=K; ++i) for (int j=0; j<=K-i; ++j) answers[K+j] -= select_good_speaker * add[i] * convince_probability[K-i][j];

    vector<number> subtract = select_probability_distribution[x][K];
    for (int i=0; i<=K; ++i) for (int j=0; j<=i; ++j) answers[K-j] -= (1 - select_good_speaker) * subtract[i] * convince_probability[i][j];
    
    return answers;
}

vector<number> solve_sparse_system(vector< vector<number> > &sparse_lhs, vector<number> &rhs) {
    for (int i=0; i<=N; ++i) {
        number pivot = sparse_lhs[i][K]; // = lhs[i][i]
        assert(pivot != 0);
        for (int j=i+1; j<=i+K && j<=N; ++j) {
            int off = j-i;
            number factor = sparse_lhs[j][K-off] / pivot; // = lhs[j][i] / lhs[i][i]
            if (factor == 0) continue;
            for (int q=K-off; q<=2*K; ++q) sparse_lhs[j][q] -= factor * (q+off <= 2*K ? sparse_lhs[i][q+off] : number(0));
            rhs[j] -= factor * rhs[i];
        }
    }
    vector<number> solution(N+1, 0);
    for (int i=N; i>=0; --i) {
        number known = 0;
        for (int j=1; j<=K && i+j<=N; ++j) known += sparse_lhs[i][K+j] * solution[i+j];
        solution[i] = (rhs[i] - known) / sparse_lhs[i][K];
    }
    return solution;
}

void solve_one() {
    cin >> N >> K;
    int _P;
    cin >> _P;
    P = number(_P) / 100;

    vector<int> start(N);
    for (int n=0; n<N; ++n) cin >> start[n];

    calculate_convince_probability();
    calculate_select_probability_distribution();

    vector< vector<number> > sparse_lhs(N+1);
    vector<number> rhs(N+1, 0);

    sparse_lhs[0] = vector<number>(2*K+1, 0);
    sparse_lhs[N] = vector<number>(2*K+1, 0);
    sparse_lhs[0][K] = number(1); // = lhs[0][0]
    sparse_lhs[N][K] = number(1); // = lhs[N][N]

    for (int x=1; x<N; ++x) {
        sparse_lhs[x] = transition_probabilities(x);
        rhs[x] = number(x) / N;
    }

    auto f = solve_sparse_system(sparse_lhs, rhs);

    map<int, int> startcnt;    
    for (int a : start) ++startcnt[a];
    number answer = 0;
    for (auto rec : startcnt) answer += f[rec.second];

    cout << fixed << setprecision(numeric_limits<number>::digits10);
    cout << answer << endl;
}

int main() {
    int TC;
    cin >> TC;
    while (TC--) solve_one();
}
