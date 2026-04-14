// LC200 Number of Islands - BFS flood-fill
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.empty()) return 0;
        int m = grid.size(), n = grid[0].size(), islands = 0;
        const int DX[4] = {1,-1,0,0};
        const int DY[4] = {0,0,1,-1};
        for (int i=0;i<m;i++) {
            for (int j=0;j<n;j++) {
                if (grid[i][j]=='1') {
                    islands++;
                    queue<pair<int,int>> q;
                    q.push({i,j});
                    grid[i][j]='0';
                    while (!q.empty()) {
                        auto [x,y]=q.front(); q.pop();
                        for (int d=0;d<4;d++) {
                            int nx=x+DX[d], ny=y+DY[d];
                            if (nx>=0&&nx<m&&ny>=0&&ny<n&&grid[nx][ny]=='1') {
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

int main() {
    Solution s;
    vector<vector<char>> g1 = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    if (s.numIslands(g1)!=1) { cout<<"FAIL g1"<<endl; return 1; }
    vector<vector<char>> g2 = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    if (s.numIslands(g2)!=3) { cout<<"FAIL g2"<<endl; return 1; }
    vector<vector<char>> g3 = {{'1'}};
    if (s.numIslands(g3)!=1) { cout<<"FAIL g3"<<endl; return 1; }
    vector<vector<char>> g4 = {{'0'}};
    if (s.numIslands(g4)!=0) { cout<<"FAIL g4"<<endl; return 1; }
    vector<vector<char>> g5 = {
        {'1','1','1'},
        {'0','1','0'},
        {'1','1','1'}
    };
    if (s.numIslands(g5)!=1) { cout<<"FAIL g5"<<endl; return 1; }
    cout << "5/5" << endl;
    return 0;
}
