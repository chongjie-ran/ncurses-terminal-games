
#include <iostream>
#include <vector>
using namespace std;

// LC474 Ones and Zeroes - 二维背包
pair<int,int> count01(string s) {
    int z=0, o=0;
    for(char c: s) c=='0' ? z++ : o++;
    return {z,o};
}

int findMaxForm(vector<string>& strs, int m, int n) {
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    for (auto& s: strs) {
        auto [z,o] = count01(s);
        for (int i=m; i>=z; i--) {
            for (int j=n; j>=o; j--) {
                dp[i][j] = max(dp[i][j], dp[i-z][j-o] + 1);
            }
        }
    }
    return dp[m][n];
}

int main() {
    vector<string> s = {"10","0001","111001","1","0"};
    cout << findMaxForm(s, 5, 3) << endl; // 4
    return 0;
}
