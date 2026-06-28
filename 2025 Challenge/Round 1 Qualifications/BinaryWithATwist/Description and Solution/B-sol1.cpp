#include <bits/stdc++.h>

using namespace std;

long long solve(long long n)
{
    // check bases up to 1000 (+-)

    for(int base=3;base<=1005;base++)
    {
        long long tmp = n;
        bool ok = true;
        while(tmp && ok)
        {
            if(tmp%base>1) ok = false;
            tmp /= base;
        }

        if(ok)
        {
           return base;
        }
    }

    // log_1000(10^18) = 6. So in bases higher than 100, n can be only up to 6 digits.
    // Check all binary strings up to length 6, binary search for smallest base with >= value.
    // Smaller base -> Greater binary string, so search top-to-bottom

    for(int mask=(1<<6)-1;mask>0;--mask)
    {
        long long lo = 2, hi = n+1;

        while(hi-lo>1)
        {
            long long m = (hi+lo)/2;
            long long v = 0,cur = 1;
            for(int i=0;i<9&&v<=n;++i)
            {

                if((mask>>i)&1) v += cur;

                if(cur <= n/m) cur *= m;
                else cur = n+47;
            }

            if(v >= n) hi = m;
            else lo = m;
        }

        long long v = 0,cur = 1;
        for(int i=0;i<6&&v<=n;++i)
        {
            if((mask>>i)&1) v += cur;

            if(cur <= n/hi) cur *= hi;
            else cur = n+47;
        }

        if(v==n) return hi;
    }

    return -1;
}

int main()
{
    int t;
    cin >> t;
    while(t--) {
        long long n;
        cin >> n;
        cout << solve(n) << "\n";
    }
}