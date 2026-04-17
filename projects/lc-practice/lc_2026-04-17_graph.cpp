// LC practice 2026-04-17 - 图论BFS/DFS专题
// 关键洞察：Water flows low -> high (反向!)
// 从边界反向DFS：邻居能流到(i,j) iff h[ni][nj] <= h[i][j]
// 因为水往低处流，所以水从(ni,nj)流到(i,j)要求ni>=i (neighbor更高)
#include <iostream>
#include <vector>
using namespace std;

// ===== LC200: Number of Islands =====
class Solution200 {
public:
    int numIslands(vector<vector<char>>& grid) {
        int m=grid.size(), n=grid[0].size(), cnt=0;
        for(int i=0;i<m;i++) for(int j=0;j<n;j++)
            if(grid[i][j]=='1'){dfs(grid,i,j,m,n);cnt++;}
        return cnt;
    }
private:
    void dfs(vector<vector<char>>& g,int i,int j,int m,int n){
        if(i<0||j<0||i>=m||j>=n||g[i][j]!='1')return;
        g[i][j]='0';
        dfs(g,i+1,j,m,n);dfs(g,i-1,j,m,n);
        dfs(g,i,j+1,m,n);dfs(g,i,j-1,m,n);
    }
};

// ===== LC207: Course Schedule (拓扑排序) =====
class Solution207 {
public:
    bool canFinish(int n, vector<vector<int>>& edges) {
        vector<vector<int>> g(n);vector<int> indeg(n,0);
        for(auto&e:edges){g[e[0]].push_back(e[1]);indeg[e[1]]++;}
        queue<int> q;
        for(int i=0;i<n;i++) if(indeg[i]==0)q.push(i);
        int visited=0;
        while(!q.empty()){int u=q.front();q.pop();visited++;
            for(int v:g[u]) if(--indeg[v]==0)q.push(v);}
        return visited==n;
    }
};

// ===== LC417: Pacific Atlantic Water Flow =====
// 从海洋边界反向DFS
// 邻居(ni,nj)能流到(i,j) iff h[ni][nj] <= h[i][j]
// (水往低处流，所以(i,j)能从邻居(ni,nj)到达要求邻居更高)
class Solution417 {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& h) {
        int m=h.size(),n=h[0].size();
        vector<vector<int>> p(m,vector<int>(n,0)),a(m,vector<int>(n,0));
        for(int i=0;i<m;i++){
            dfs(h,p,i,0,m,n);   // 左->太平洋
            dfs(h,a,i,n-1,m,n); // 右->大西洋
        }
        for(int j=0;j<n;j++){
            dfs(h,p,0,j,m,n);   // 上->太平洋
            dfs(h,a,m-1,j,m,n); // 下->大西洋
        }
        vector<vector<int>> res;
        for(int i=0;i<m;i++) for(int j=0;j<n;j++)
            if(p[i][j]&&a[i][j]) res.push_back({i,j});
        return res;
    }
private:
    void dfs(vector<vector<int>>& h,vector<vector<int>>& vis,int i,int j,int m,int n){
        if(vis[i][j])return;
        vis[i][j]=1;
        const int d[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
        for(auto&dir:d){
            int ni=i+dir[0],nj=j+dir[1];
            if(ni<0||nj<0||ni>=m||nj>=n)continue;
            // 邻居(更高)能流到当前格(更低)
            if(h[ni][nj]<=h[i][j]&&!vis[ni][nj])
                dfs(h,vis,ni,nj,m,n);
        }
    }
};

int main(){
    Solution200 s200;
    vector<vector<char>> g1={{'1','1','0','0','0'},{'1','1','0','0','0'},{'0','0','1','0','0'},{'0','0','0','1','1'}};
    cout<<"LC200:"<<s200.numIslands(g1)<<" (expect 3)"<<endl;
    
    Solution207 s207;
    vector<vector<int>> e1={{1,0}},e2={{1,0},{0,1}};
    cout<<"LC207:"<<s207.canFinish(2,e1)<<" (expect 1), "<<s207.canFinish(2,e2)<<" (expect 0)"<<endl;
    
    Solution417 s417;
    vector<vector<int>> h={{1,2,2,3,5},{3,2,3,5,5},{6,5,5,5,5},{9,8,6,7,7}};
    auto r=s417.pacificAtlantic(h);
    cout<<"LC417:"<<r.size()<<" (expect 7)"<<endl;
    for(auto&c:r) cout<<"("<<c[0]<<","<<c[1]<<") ";
    cout<<endl;
    return 0;
}
