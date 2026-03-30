// LC514 Freedom Trail
// Hard | DP + greedy selection on ring
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
using namespace std;

int findRotateSteps(string ring, string key) {
    int n = ring.size(), m = key.size();
    unordered_map<char, vector<int>> pos;
    for (int i = 0; i < n; ++i) pos[ring[i]].push_back(i);
    const int INF = 1e9;
    vector<int> dp(n, INF);
    dp[0] = 0;
    for (int i = 0; i < m; ++i) {
        vector<int> ndp(n, INF);
        for (int rpos = 0; rpos < n; ++rpos) {
            if (dp[rpos] == INF) continue;
            for (int p : pos[key[i]]) {
                int diff = abs(rpos - p);
                int step = min(diff, n - diff);
                ndp[p] = min(ndp[p], dp[rpos] + step + 1);
            }
        }
        dp.swap(ndp);
    }
    return *min_element(dp.begin(), dp.end());
}

int main() {
    cout << findRotateSteps("godding","gd") << endl; // 4
    cout << findRotateSteps("abcde","ade") << endl; // 6
    return 0;
}
