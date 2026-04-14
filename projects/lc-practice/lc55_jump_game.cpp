// LC55 Jump Game
// 贪心：从左到右，维护能到达的最远距离
// 如果当前位置 > 最远可达，则失败
#include <iostream>
#include <vector>
using namespace std;

bool canJump(vector<int>& nums) {
    int farthest = 0;
    for (int i = 0; i < nums.size(); i++) {
        if (i > farthest) return false;  // 当前位置不可达
        farthest = max(farthest, i + nums[i]);
        if (farthest >= nums.size() - 1) return true;  // 可达终点
    }
    return true;
}

// DP版本（参考）
bool canJumpDP(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 0);
    dp[0] = nums[0];
    for (int i = 1; i < n; i++) {
        if (dp[i-1] < i) return false;
        dp[i] = max(dp[i-1], i + nums[i]);
    }
    return true;
}

int main() {
    vector<int> t1 = {2,3,1,1,4};
    vector<int> t2 = {3,2,1,0,4};
    vector<int> t3 = {0};
    vector<int> t4 = {2,0,0};
    
    cout << (canJump(t1) ? "true" : "false") << endl;  // true
    cout << (canJump(t2) ? "true" : "false") << endl;  // false
    cout << (canJump(t3) ? "true" : "false") << endl;  // true
    cout << (canJump(t4) ? "true" : "false") << endl;  // true
}
