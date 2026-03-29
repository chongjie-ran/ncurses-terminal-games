// LC994 Rotting Oranges
// BFS multi-source from all initially rotten oranges
// Track minutes/min steps until all fresh oranges infected

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        queue<pair<int,int>> q;
        int fresh = 0;
        for (int i=0;i<m;i++) {
            for (int j=0;j<n;j++) {
                if (grid[i][j]==2) q.emplace(i,j);
                else if (grid[i][j]==1) fresh++;
            }
        }
        if (fresh==0) return 0;
        static const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        int minutes = 0;
        while (!q.empty()) {
            int sz = q.size();
            while (sz--) {
                auto [x,y] = q.front(); q.pop();
                for (auto& d: dirs) {
                    int nx=x+d[0], ny=y+d[1];
                    if (nx>=0&&nx<m&&ny>=0&&ny<n&&grid[nx][ny]==1) {
                        grid[nx][ny]=2;
                        q.emplace(nx,ny);
                        fresh--;
                    }
                }
            }
            if (!q.empty()) minutes++;
        }
        return fresh==0 ? minutes : -1;
    }
};

int main() {
    Solution s;
    vector<vector<int>> g1 = {{2,1,1},{1,1,0},{0,1,1}};
    cout << s.orangesRotting(g1) << endl; // 4

    vector<vector<int>> g2 = {{2,1,1},{0,1,1},{1,0,1}};
    cout << s.orangesRotting(g2) << endl; // -1

    vector<vector<int>> g3 = {{0,2}};
    cout << s.orangesRotting(g3) << endl; // 0
    return 0;
}
