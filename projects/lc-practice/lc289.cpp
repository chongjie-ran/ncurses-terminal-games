// LC289 生命游戏
// 规则: 根据周围8格活细胞数决定生死
// 思路: 原地修改，用高位存储新状态，位运算分离新旧状态
#include <iostream>
#include <vector>
using namespace std;
void gameOfLife(vector<vector<int>>& board) {
    int m = board.size(), n = board[0].size();
    int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    for (int i=0;i<m;i++) {
        for (int j=0;j<n;j++) {
            int live = 0;
            for (auto& d: dirs) {
                int x=i+d[0], y=j+d[1];
                if (x>=0&&x<m&&y>=0&&y<n) live += board[x][y]&1;
            }
            int cur = board[i][j]&1;
            if (cur==1 && (live==2||live==3)) board[i][j]|=2;
            if (cur==0 && live==3) board[i][j]|=2;
        }
    }
    for (int i=0;i<m;i++) for (int j=0;j<n;j++) board[i][j]>>=1;
}
int main() {
    vector<vector<int>> b = {{0,1,0},{0,0,1},{1,1,1},{0,0,0}};
    gameOfLife(b);
    for (auto& r: b) { for (int x: r) cout<<x<<" "; cout<<endl; }
    return 0;
}
