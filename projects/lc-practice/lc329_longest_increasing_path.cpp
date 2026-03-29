// LC329 Longest Increasing Path in Matrix - DFS+Memo / Topo Sort
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

int dfs(const vector<vector<int>>& m, int i, int j,
        vector<vector<int>>& memo, int M, int N) {
    if (memo[i][j] != 0) return memo[i][j];
    const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    int best = 1;
    for (auto& d : dirs) {
        int ni = i+d[0], nj = j+d[1];
        if (ni<0||ni>=M||nj<0||nj>=N) continue;
        if (m[ni][nj] <= m[i][j]) continue;
        best = max(best, 1 + dfs(m, ni, nj, memo, M, N));
    }
    memo[i][j] = best;
    return best;
}

int longestIncreasingPath(vector<vector<int>>& matrix) {
    if (matrix.empty()) return 0;
    int M = matrix.size(), N = matrix[0].size();
    vector<vector<int>> memo(M, vector<int>(N, 0));
    int ans = 0;
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            ans = max(ans, dfs(matrix, i, j, memo, M, N));
    return ans;
}

int main() {
    vector<vector<int>> m1 = {{9,9,4},{6,6,8},{2,1,1}};
    cout << longestIncreasingPath(m1) << endl; // 4
    vector<vector<int>> m2 = {{3,4,3},{3,3,3}};
    cout << longestIncreasingPath(m2) << endl; // 3
    return 0;
}
