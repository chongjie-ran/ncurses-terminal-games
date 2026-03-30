#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maximalSquare(vector<vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> dp(n + 1, 0);
    int maxSide = 0, prev = 0;
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (matrix[i-1][j-1] == '1') {
                dp[j] = min({dp[j], dp[j-1], prev}) + 1;
                maxSide = max(maxSide, dp[j]);
            } else {
                dp[j] = 0;
            }
            prev = temp;
        }
        prev = 0; // reset prev for next row
    }
    return maxSide * maxSide;
}

int main() {
    vector<vector<char>> m1 = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << maximalSquare(m1) << endl; // 4 (2x2 square at bottom-right)
    vector<vector<char>> m2 = {{'0'}};
    cout << maximalSquare(m2) << endl; // 0
    vector<vector<char>> m3 = {{'1'}};
    cout << maximalSquare(m3) << endl; // 1
    return 0;
}
