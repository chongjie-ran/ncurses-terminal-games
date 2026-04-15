// LC647 回文子串 - 中等
// 计数回文子串个数
#include <iostream>
#include <string>
using namespace std;

// 中心扩展法 O(n^2)
int countSubstrings(string s) {
    int n = s.size(), count = 0;
    for (int center = 0; center < 2*n - 1; center++) {
        int left = center / 2;
        int right = left + center % 2;
        while (left >= 0 && right < n && s[left] == s[right]) {
            count++;
            left--;
            right++;
        }
    }
    return count;
}

// DP方法
int countSubstringsDP(string s) {
    int n = s.size(), count = 0;
    vector<vector<bool>> dp(n, vector<bool>(n, false));
    for (int i = n-1; i >= 0; i--) {
        for (int j = i; j < n; j++) {
            dp[i][j] = (s[i] == s[j]) && (j-i < 2 || dp[i+1][j-1]);
            if (dp[i][j]) count++;
        }
    }
    return count;
}

int main() {
    cout << countSubstrings("abc") << endl; // 3
    cout << countSubstringsDP("abc") << endl;
    cout << countSubstrings("aaa") << endl; // 6
    cout << countSubstringsDP("aaa") << endl;
    cout << countSubstrings("aba") << endl; // 4
    cout << countSubstringsDP("aba") << endl;
    return 0;
}
