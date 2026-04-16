// LC练习 - 2026-04-16 下午
// 专题：图论/DFS/BFS + 高级DP
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// LC417: 太平洋大西洋水流 - 多源DFS
class Solution417 {
public:
    vector<pair<int,int>> pacificAtlantic(vector<vector<int>>& heights) {
        if(heights.empty()) return {};
        int m=heights.size(), n=heights[0].size();
        vector<vector<bool>> pac(m,vector<bool>(n,false)), atl(m,vector<bool>(n,false));
        
        for(int i=0;i<m;i++) {
            dfs(heights,pac,i,0);
            dfs(heights,atl,i,n-1);
        }
        for(int j=0;j<n;j++) {
            dfs(heights,pac,0,j);
            dfs(heights,atl,m-1,j);
        }
        
        vector<pair<int,int>> res;
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++)
                if(pac[i][j] && atl[i][j])
                    res.push_back({i,j});
        return res;
    }
    
private:
    void dfs(vector<vector<int>>& h, vector<vector<bool>>& visited, int r, int c) {
        visited[r][c]=true;
        int m=h.size(), n=h[0].size();
        int dr[4]={-1,1,0,0}, dc[4]={0,0,-1,1};
        for(int i=0;i<4;i++) {
            int nr=r+dr[i], nc=c+dc[i];
            if(nr>=0 && nr<m && nc>=0 && nc<n && !visited[nr][nc] && h[nr][nc]>=h[r][c])
                dfs(h,visited,nr,nc);
        }
    }
};

// LC994: 腐烂的橘子 - BFS层序
class Solution994 {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m=grid.size(), n=grid[0].size();
        queue<pair<int,int>> q;
        int fresh=0, time=0;
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++) {
                if(grid[i][j]==2) q.push({i,j});
                else if(grid[i][j]==1) fresh++;
            }
        if(fresh==0) return 0;
        
        int dr[4]={-1,1,0,0}, dc[4]={0,0,-1,1};
        while(!q.empty()) {
            int sz=q.size();
            while(sz--) {
                auto [r,c]=q.front(); q.pop();
                for(int i=0;i<4;i++) {
                    int nr=r+dr[i], nc=c+dc[i];
                    if(nr>=0 && nr<m && nc>=0 && nc<n && grid[nr][nc]==1) {
                        grid[nr][nc]=2;
                        fresh--;
                        q.push({nr,nc});
                    }
                }
            }
            if(!q.empty()) time++;
        }
        return fresh==0?time:-1;
    }
};

int main() {
    Solution417 s417;
    vector<vector<int>> h1 = {{1,2,2,3,5},{3,2,3,4,4},{2,4,5,3,1},{6,7,1,4,5},{5,1,1,2,4}};
    auto res417 = s417.pacificAtlantic(h1);
    cout<<"LC417: "<<res417.size()<<" cells (expected 7)"<<endl;
    for(auto& p:res417) cout<<"("<<p.first<<","<<p.second<<") ";
    cout<<endl;
    
    Solution994 s994;
    vector<vector<int>> g1 = {{2,1,1},{1,1,0},{0,1,1}};
    cout<<"LC994: "<<s994.orangesRotting(g1)<<" minutes (expected 4)"<<endl;
    
    return 0;
}
