#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

class Solution {
public:
    int numDecodings(string s) {
        if (s.empty() || s[0] == '0') return 0;
        int n = s.size();
        vector<int> dp(n + 1, 0);
        dp[0] = 1; dp[1] = 1;
        for (int i = 2; i <= n; ++i) {
            if (s[i-1] != '0') dp[i] += dp[i-1];
            int two = atoi(s.substr(i-2, 2).c_str());
            if (two >= 10 && two <= 26) dp[i] += dp[i-2];
        }
        return dp[n];
    }
};

int main() {
    Solution sol;
    vector<string> tests = {"12","226","06","10","101","27","111111111111111111111111111111111111111111111"};
    vector<int> expected = {2,3,0,1,1,1,1836311903};
    int passed = 0;
    for (int i = 0; i < tests.size(); ++i) {
        int result = sol.numDecodings(tests[i]);
        bool ok = (result == expected[i]);
        cout << "LC91 \"" << tests[i] << "\": " << result << " (expected " << expected[i] << ") " << (ok?"✓":"✗") << endl;
        if (ok) passed++;
    }
    cout << "Result: " << passed << "/" << tests.size() << " passed" << endl;
    return 0;
}
