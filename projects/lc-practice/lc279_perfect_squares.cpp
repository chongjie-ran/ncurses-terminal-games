#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// LC279 完全平方数
// BFS/DP: 求最少完全平方数之和等于n
int numSquares(int n) {
    vector<int> dp(n+1, INT_MAX);
    dp[0] = 0;
    for(int i=1; i<=n; i++){
        for(int j=1; j*j<=i; j++){
            dp[i] = min(dp[i], dp[i-j*j]+1);
        }
    }
    return dp[n];
}

// 验证
int main(){
    // 测试
    vector<int> tests = {1, 2, 3, 4, 5, 12, 13};
    for(int n: tests){
        cout << n << " -> " << numSquares(n) << endl;
    }
    return 0;
}
