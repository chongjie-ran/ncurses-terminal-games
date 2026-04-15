#include <iostream>
#include <vector>
using namespace std;

// LC96 Unique Binary Search Trees
// Count number of BSTs with n distinct values [1..n]
// G(n) = sum(G(i-1) * G(n-i)) for i=1..n (Catalan number)
// dp[i] = number of BSTs with i nodes
class Solution {
public:
    int numTrees(int n) {
        vector<long long> dp(n+1, 0);
        dp[0] = 1; dp[1] = 1;
        for (int i = 2; i <= n; ++i) {
            for (int j = 1; j <= i; ++j) {
                dp[i] += dp[j-1] * dp[i-j];
            }
        }
        return (int)dp[n];
    }
};

int main() {
    Solution sol;
    vector<int> tests = {1,2,3,4,5,10,15};
    vector<int> expected = {1,2,5,14,42,16796,9694845};
    int passed = 0;
    for (int i = 0; i < tests.size(); ++i) {
        int result = sol.numTrees(tests[i]);
        bool ok = (result == expected[i]);
        cout << "LC96 n=" << tests[i] << ": " << result << " (expected " << expected[i] << ") " << (ok?"✓":"✗") << endl;
        if (ok) passed++;
    }
    cout << "Result: " << passed << "/" << tests.size() << " passed" << endl;
    return 0;
}
