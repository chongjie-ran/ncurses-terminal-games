// LC169 Majority Element - Boyer-Moore Voting
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int candidate = 0, count = 0;
        for (int n : nums) {
            if (count == 0) candidate = n;
            count += (n == candidate) ? 1 : -1;
        }
        return candidate;
    }
};

int main() {
    Solution sol;
    vector<vector<int>> tests = {{3,2,3}, {2,2,1,1,1,2,2}, {1}, {1,1,2,2,1,2,1}, {2,2,2,1,3}};
    vector<int> expected = {3, 2, 1, 1, 2};
    int passed = 0;
    for (int i = 0; i < tests.size(); i++) {
        int result = sol.majorityElement(tests[i]);
        bool ok = result == expected[i];
        cout << "Test " << i+1 << ": " << (ok ? "✅" : "❌") 
             << " got=" << result << " exp=" << expected[i] << endl;
        passed += ok;
    }
    cout << "Result: " << passed << "/" << tests.size() << endl;
    return 0;
}
