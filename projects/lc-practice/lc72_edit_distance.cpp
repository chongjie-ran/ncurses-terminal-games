// LC72 Edit Distance - Hard
// 字符串word1转word2的最少操作数
// 操作：插入、删除、替换
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// DP[i][j] = word1前i个字符转word2前j个字符的最少操作数
// 若word1[i-1]==word2[j-1]: dp[i][j]=dp[i-1][j-1]
// 否则: dp[i][j]=min(dp[i-1][j]+1, dp[i][j-1]+1, dp[i-1][j-1]+1)
//       (删除, 插入, 替换)
int minDistance(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    // base: 一个空串转另一个串需要j次插入
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (word1[i-1] == word2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]}) + 1;
        }
    }
    return dp[m][n];
}

// 空间优化版 O(n)
int minDistance1D(string word1, string word2) {
    int m = word1.size(), n = word2.size();
    vector<int> dp(n+1, 0), prev(n+1, 0);
    for (int j = 0; j <= n; j++) dp[j] = j;
    
    for (int i = 1; i <= m; i++) {
        prev = dp;
        dp[0] = i;
        for (int j = 1; j <= n; j++) {
            if (word1[i-1] == word2[j-1])
                dp[j] = prev[j-1];
            else
                dp[j] = min({prev[j], dp[j-1], prev[j-1]}) + 1;
        }
    }
    return dp[n];
}

int main() {
    // Test 1: horse -> ros
    // horse → ros: 3步 (删除e, 替换h→r, 删除e... wait)
    // horse(1删除e)→hors→ros (1替换h→r, 删除e) = 3
    // horse(1删除e)→hors(2删除e)→ros(3替换h→r) 等等
    // horse → ros: 3
    // 方案1: horse→rorse(插入r)→rose(替换s→o)→ros(删除e) = 3
    // 方案2: horse→ros(3步)
    // 答案: 3
    string w1 = "horse", w2 = "ros";
    cout << "horse→ros: " << minDistance(w1, w2) << " (expected 3)" << endl;
    cout << "1D: " << minDistance1D(w1, w2) << " (expected 3)" << endl;
    
    // Test 2: intention -> execution
    // 答案: 5
    w1 = "intention"; w2 = "execution";
    cout << "intention→execution: " << minDistance(w1, w2) << " (expected 5)" << endl;
    
    // Test 3: "" -> "abc"
    w1 = ""; w2 = "abc";
    cout << "→abc: " << minDistance(w1, w2) << " (expected 3)" << endl;
    
    // Test 4: "abc" -> ""
    w1 = "abc"; w2 = "";
    cout << "abc→: " << minDistance(w1, w2) << " (expected 3)" << endl;
    
    // Test 5: "distance" -> "editing"
    w1 = "distance"; w2 = "editing";
    cout << "distance→editing: " << minDistance(w1, w2) << endl;
    
    return 0;
}
