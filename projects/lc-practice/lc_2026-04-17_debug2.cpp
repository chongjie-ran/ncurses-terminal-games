// Debug: trace LC417 step by step
#include <iostream>
#include <vector>
using namespace std;

class Solution417 {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& h) {
        int m=h.size(), n=h[0].size();
        vector<vector<int>> p(m, vector<int>(n, 0));
        vector<vector<int>> a(m, vector<int>(n, 0));
        for (int i=0;i<m;i++) {
            dfs(h,p,i,0,m,n);
            dfs(h,a,i,n-1,m,n);
        }
        for (int j=0;j<n;j++) {
            dfs(h,p,0,j,m,n);
            dfs(h,a,m-1,j,m,n);
        }
        vector<vector<int>> res;
        for (int i=0;i<m;i++)
            for (int j=0;j<n;j++)
                if (p[i][j]&&a[i][j]) res.push_back({i,j});
        return res;
    }
private:
    void dfs(vector<vector<int>>& h, vector<vector<int>>& vis, int i, int j, int m, int n) {
        if (vis[i][j]) return;
        vis[i][j]=1;
        const int dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
        for (auto& d:dirs) {
            int x=i+d[0], y=j+d[1];
            // Water flows: h[i][j] >= h[x][y] means water at (i,j) can flow to (x,y)
            // So from ocean coast inward: current cell can reach neighbor only if 
            // current height >= neighbor height
            if (x<0||y<0||x>=m||y>=n) continue;
            if (h[i][j] >= h[x][y] && !vis[x][y]) {
                dfs(h,vis,x,y,m,n);
            }
        }
    }
};

int main() {
    vector<vector<int>> h = {{1,2,2,3,5},{3,2,3,5,5},{6,5,5,5,5},{9,8,6,7,7}};
    Solution417 s;
    auto r = s.pacificAtlantic(h);
    cout << "Count: " << r.size() << endl;
    for (auto& c : r) cout << "(" << c[0] << "," << c[1] << ") ";
    cout << endl;
    return 0;
}
