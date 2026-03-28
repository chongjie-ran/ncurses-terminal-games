// LC1463 - Cherry Pickup II
// Hard DP: 滚动数组 O(m*n*n)
// 两人从(row=0, col=0)和(row=0, col=n-1)同时出发，收集最多樱桃
// 重叠格计一次（每人独立grid，共享格只计一个grid的值）
//
// 穷举验证正确性:
//   Test1: [[3,1,1],[2,5,1],[1,5,5]] → 21 (原repo错误写成24)
//   Test2: 4×7 → 11 (原repo错误写成10)
//   Test3: [[1,1],[1,1]] → 4 ✅
//
// 验证: g++ -std=c++17 -O2 lc1463_cherry_pickup_ii.cpp && ./a.out

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    int cherryPickup(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        const int NEG = -1e9;
        // dp[c1][c2] = 在当前行 r，机器人A在 c1，机器人B在 c2 时的最大樱桃数
        vector<vector<int>> dp(n, vector<int>(n, NEG));
        dp[0][n-1] = grid[0][0] + grid[0][n-1]; // row=0 已在起点（不重复计）

        for (int r = 1; r < m; ++r) {
            vector<vector<int>> ndp(n, vector<int>(n, NEG));
            for (int c1 = 0; c1 < n; ++c1) {
                for (int c2 = 0; c2 < n; ++c2) {
                    // 重叠格计一次（不是两次）
                    int cur = (c1 == c2) ? grid[r][c1] : grid[r][c1] + grid[r][c2];
                    for (int pc1 = max(0, c1-1); pc1 <= min(n-1, c1+1); ++pc1) {
                        for (int pc2 = max(0, c2-1); pc2 <= min(n-1, c2+1); ++pc2) {
                            if (dp[pc1][pc2] == NEG) continue;
                            ndp[c1][c2] = max(ndp[c1][c2], dp[pc1][pc2] + cur);
                        }
                    }
                }
            }
            dp.swap(ndp);
        }

        int ans = 0;
        for (int c1 = 0; c1 < n; ++c1)
            for (int c2 = 0; c2 < n; ++c2)
                ans = max(ans, dp[c1][c2]);
        return ans;
    }
};

int main() {
    Solution sol;
    // Test1: [[3,1,1],[2,5,1],[1,5,5]] → 21
    vector<vector<int>> g1 = {{3,1,1},{2,5,1},{1,5,5}};
    cout << "Test1: " << sol.cherryPickup(g1) << " (correct: 21)" << endl;

    // Test2: 4×7 → 11
    vector<vector<int>> g2 = {{1,0,0,0,0,0,1},{2,0,0,0,0,0,0},{3,0,0,0,0,0,0},{4,0,0,0,0,0,0}};
    cout << "Test2: " << sol.cherryPickup(g2) << " (correct: 11)" << endl;

    // Test3: 2×2 全1 → 4
    vector<vector<int>> g3 = {{1,1},{1,1}};
    cout << "Test3: " << sol.cherryPickup(g3) << " (correct: 4)" << endl;
    return 0;
}
