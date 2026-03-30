// LC97 Interleaving String — Hard
// 判断s3是否由s1和s2交叉构成
#include <cstdio>
#include <vector>
#include <string>
using namespace std;

// dp[i][j] = s1[0..i-1] 和 s2[0..j-1] 能构成 s3[0..i+j-1]
bool isInterleave(string s1, string s2, string s3) {
    int m = s1.size(), n = s2.size();
    if (m + n != s3.size()) return false;
    vector<char> dp(n+1, 0);
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 && j == 0) {
                dp[j] = 1;
            } else if (i == 0) {
                dp[j] = dp[j-1] && s2[j-1] == s3[j-1];
            } else if (j == 0) {
                dp[j] = dp[j] && s1[i-1] == s3[i-1];
            } else {
                dp[j] = (dp[j] && s1[i-1] == s3[i+j-1]) ||
                        (dp[j-1] && s2[j-1] == s3[i+j-1]);
            }
        }
    }
    return dp[n];
}

int main() {
    printf("s1=aab, s2=axy, s3=aaxaby: %d (expect 1)\n",
        isInterleave("aab","axy","aaxaby"));
    printf("s1=aab, s2=abx, s3=abaabx: %d (expect 1)\n",
        isInterleave("aab","abx","abaabx"));
    printf("s1=abc, s2=def, s3=adbcf: %d (expect 0)\n",
        isInterleave("abc","def","adbcf"));
    return 0;
}
