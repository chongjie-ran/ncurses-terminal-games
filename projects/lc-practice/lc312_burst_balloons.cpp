// LC312 Burst Balloons - 戳气球获得最多硬币
// 关键词: 区间DP
// 难度: Hard

#include <iostream>
#include <vector>
using namespace std;

int maxCoins(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    
    // 虚拟气球: 两端为1
    vector<int> val(n + 2, 1);
    for (int i = 0; i < n; ++i) val[i + 1] = nums[i];
    
    // dp大小: (n+2) x (n+2), 初始化0
    vector<vector<int>> dp(n + 2, vector<int>(n + 2, 0));
    
    // 区间长度从1到n+1 (气球数量+1)
    // dp[i][j]需要len=j-i，所以最大len是n+1 (i=0, j=n+1)
    for (int len = 1; len <= n + 1; ++len) {
        for (int i = 0; i + len < n + 2; ++i) {
            int j = i + len;
            for (int k = i + 1; k < j; ++k) {
                dp[i][j] = max(dp[i][j], 
                    dp[i][k] + dp[k][j] + val[i] * val[k] * val[j]);
            }
        }
    }
    return dp[0][n + 1];
}

// 测试
int main() {
    // 测试1: [3,1,5,8] → 167
    vector<int> nums1 = {3, 1, 5, 8};
    cout << "Test 1: " << maxCoins(nums1) << " (expected: 167)" << endl;
    
    // 测试2: [1,5] → 10
    vector<int> nums2 = {1, 5};
    cout << "Test 2: " << maxCoins(nums2) << " (expected: 10)" << endl;
    
    // 测试3: [] → 0
    vector<int> nums3 = {};
    cout << "Test 3: " << maxCoins(nums3) << " (expected: 0)" << endl;
    
    // 测试4: [1] → 1
    vector<int> nums4 = {1};
    cout << "Test 4: " << maxCoins(nums4) << " (expected: 1)" << endl;
    
    return 0;
}
