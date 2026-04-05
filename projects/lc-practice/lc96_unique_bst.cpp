// LC96: Unique Binary Search Trees
// Medium - DP (Catalan Number)
// O(n) time, O(n) space
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int numTrees(int n) {
        vector<int> dp(n+1, 0);
        dp[0] = 1; // 空树
        dp[1] = 1; // 单节点
        
        for (int i = 2; i <= n; i++) {
            for (int j = 1; j <= i; j++) {
                // j作为根，左边有j-1个节点，右边有i-j个节点
                dp[i] += dp[j-1] * dp[i-j];
            }
        }
        return dp[n];
    }
};

int main() {
    Solution s;
    cout << "n=3: " << s.numTrees(3) << " (expected 5)" << endl;
    cout << "n=1: " << s.numTrees(1) << " (expected 1)" << endl;
    cout << "n=0: " << s.numTrees(0) << " (expected 1)" << endl;
    cout << "n=4: " << s.numTrees(4) << " (expected 14)" << endl;
    cout << "n=5: " << s.numTrees(5) << " (expected 42)" << endl;
    return 0;
}
