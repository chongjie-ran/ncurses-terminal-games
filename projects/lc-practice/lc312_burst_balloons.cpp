// LC312 Burst Balloons - 戳气球
// 难度: Hard
// 分类: 区间DP
// 思路: dp[i][j] = (i,j)区间内最后戳第k个气球的最大值
// dp[i][j] = max(dp[i][k] + dp[k][j] + nums[i]*nums[k]*nums[j])
// 关键洞察: 添加虚拟气球(nums[0]=1, nums[n+1]=1)，最后戳的气球决定乘积
// 复杂度: O(n^3) 时间, O(n^2) 空间

#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// 方法1: 区间DP (自底向上)
// 思路: dp[i][j] = 开区间(i,j)内戳破所有气球能获得的最大金币数
// 枚举最后一个戳破的气球k: dp[i][j] = max(dp[i][k] + dp[k][j] + nums[i]*nums[k]*nums[j])
int maxCoins(vector<int>& nums) {
    int n = nums.size();
    // 添加虚拟气球
    vector<int> a(n + 2, 1);
    for (int i = 0; i < n; i++) a[i + 1] = nums[i];
    n = a.size();
    
    // dp[i][j] = 开区间(i,j)的最大金币 (i < j)
    vector<vector<int>> dp(n, vector<int>(n, 0));
    
    // 枚举区间长度
    for (int len = 3; len <= n; len++) {
        for (int i = 0; i + len <= n; i++) {
            int j = i + len - 1;
            // 枚举最后戳的气球k (i < k < j)
            for (int k = i + 1; k < j; k++) {
                dp[i][j] = max(dp[i][j],
                    dp[i][k] + dp[k][j] + a[i] * a[k] * a[j]);
            }
        }
    }
    
    return dp[0][n - 1];
}

// 方法2: 记忆化递归
int dfs(vector<int>& a, int i, int j, vector<vector<int>>& memo) {
    if (i + 1 >= j) return 0;
    if (memo[i][j] > 0) return memo[i][j];
    int ans = 0;
    for (int k = i + 1; k < j; k++) {
        ans = max(ans,
            dfs(a, i, k, memo) + dfs(a, k, j, memo) + a[i] * a[k] * a[j]);
    }
    memo[i][j] = ans;
    return ans;
}

int maxCoins2(vector<int>& nums) {
    vector<int> a(nums.size() + 2, 1);
    for (int i = 0; i < nums.size(); i++) a[i + 1] = nums[i];
    vector<vector<int>> memo(a.size(), vector<int>(a.size(), 0));
    return dfs(a, 0, a.size() - 1, memo);
}

int main() {
    struct TestCase {
        vector<int> nums;
        int expected;
    };
    
    vector<TestCase> tests = {
        {{3,1,5,8}, 167},      // 经典案例
        {{1,5}, 10},           // 1*1*5 + 1*5*1 = 10
        {{}, 0},               // 空数组
        {{1}, 1},              // 1*1*1 = 1
        {{1,1}, 2},            // 1*1*1 + 1*1*1
        {{1,5,8}, 104},        // 88+8+8=104
    };
    
    for (auto& tc : tests) {
        int r1 = maxCoins(tc.nums);
        int r2 = maxCoins2(tc.nums);
        cout << "[";
        for (int x : tc.nums) cout << x << ",";
        cout << "] => " << r1 << "/" << r2 << " (exp:" << tc.expected << ") "
             << (r1 == tc.expected && r2 == tc.expected ? "✅" : "❌") << endl;
    }
    
    return 0;
}
