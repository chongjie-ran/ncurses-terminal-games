// LC200 Number of Islands
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if(grid.empty() || grid[0].empty()) return 0;
        int m = grid.size(), n = grid[0].size(), islands = 0;
        vector<int> dx = {0,1,0,-1}, dy = {1,0,-1,0};
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j]=='1'){
                    islands++;
                    queue<pair<int,int>> q;
                    q.push({i,j});
                    grid[i][j]='0';
                    while(!q.empty()){
                        auto [x,y]=q.front();q.pop();
                        for(int d=0;d<4;d++){
                            int nx=x+dx[d], ny=y+dy[d];
                            if(nx>=0&&nx<m&&ny>=0&&ny<n&&grid[nx][ny]=='1'){
                                grid[nx][ny]='0';
                                q.push({nx,ny});
                            }
                        }
                    }
                }
            }
        }
        return islands;
    }
};

int main(){
    Solution s;
    vector<vector<char>> g1 = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    cout << s.numIslands(g1) << endl; // 1
    
    vector<vector<char>> g2 = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << s.numIslands(g2) << endl; // 3
    return 0;
}
