#include <iostream>
#include <string>
#include <vector>
using namespace std;

// LC72 Edit Distance
// min operations to convert word1 to word2
// ops: insert, delete, replace
int minDistance(const string& word1, const string& word2) {
    int m = word1.size(), n = word2.size();
    // dp[i][j] = min ops to convert word1[0..i) to word2[0..j)
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    
    // base case: convert empty to word2[0..j) = j inserts
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    // base case: convert word1[0..i) to empty = i deletes
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (word1[i-1] == word2[j-1]) {
                dp[i][j] = dp[i-1][j-1]; // no op
            } else {
                dp[i][j] = 1 + min({
                    dp[i-1][j],   // delete word1[i-1]
                    dp[i][j-1],   // insert word2[j-1]
                    dp[i-1][j-1]  // replace
                });
            }
        }
    }
    return dp[m][n];
}

int main() {
    struct Test { string w1; string w2; int expected; };
    vector<Test> tests = {
        {"horse", "ros", 3},     // horse->ror(se) horse->ros 3 ops
        {"intention", "execution", 5},
        {"", "", 0},
        {"a", "", 1},
        {"", "a", 1},
        {"a", "b", 1},
        {"abc", "abc", 0},
        {"abc", "def", 3},
        {"horse", "ros", 3},
        {"dinitrophenol", "dinosaur", 7},
    };
    
    for (auto& t : tests) {
        int r = minDistance(t.w1, t.w2);
        cout << (r == t.expected ? "✅" : "❌") 
             << " \"" << t.w1 << "\" → \"" << t.w2 << "\" => " 
             << r << " (expected " << t.expected << ")\n";
    }
    return 0;
}
