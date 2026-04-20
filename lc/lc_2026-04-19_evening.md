# 悟通LC练习 2026-04-19 傍晚

## 苏格拉图式自检 18:40 - 回溯+图的连通专题

### 专题：回溯与DFS连通
| 题号 | 名称 | 难度 | 核心 |
|------|------|------|------|
| LC79 | Word Search | M | DFS回溯 |
| LC130 | Surrounded Regions | M | 边界DFS感染 |
| LC200 | Number of Islands | M | BFS/DFS感染 |
| LC695 | Max Area of Island | M | BFS最大面积 |

---

## 练习记录

### LC79 Word Search - DFS回溯模板
```cpp
// 核心：4方向DFS + visited标记 + 回溯撤销
class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        m=board.size(), n=board[0].size();
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++)
                if(dfs(board,word,0,i,j))
                    return true;
        return false;
    }
private:
    int m,n;
    int dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
    bool dfs(vector<vector<char>>& board, string& w, int idx, int i, int j){
        if(board[i][j]!=w[idx]) return false;
        if(idx==w.size()-1) return true;
        char c=board[i][j];
        board[i][j]='#'; // 标记已访问
        for(auto& d:dirs){
            int ni=i+d[0], nj=j+d[1];
            if(ni>=0&&ni<m&&nj>=0&&nj<n)
                if(dfs(board,w,idx+1,ni,nj)) return true;
        }
        board[i][j]=c; // 回溯撤销
        return false;
    }
};
```

### LC130 Surrounded Regions - 边界感染法
```cpp
// 核心：从边界O开始DFS感染，最后遍历内部将剩余O翻转为X
class Solution {
public:
    void solve(vector<vector<char>>& board) {
        if(board.empty()) return;
        m=board.size(), n=board[0].size();
        // 从4条边界出发感染
        for(int i=0;i<m;i++){
            dfs(board,i,0);
            dfs(board,i,n-1);
        }
        for(int j=0;j<n;j++){
            dfs(board,0,j);
            dfs(board,m-1,j);
        }
        // 内部翻X，感染标记翻O
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++){
                if(board[i][j]=='X') continue;
                board[i][j]=(board[i][j]=='Y')?'O':'X';
            }
    }
private:
    int m,n;
    int dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
    void dfs(vector<vector<char>>& board, int i, int j){
        if(i<0||i>=m||j<0||j>=n||board[i][j]!='O') return;
        board[i][j]='Y';
        for(auto& d:dirs)
            dfs(board,i+d[0],j+d[1]);
    }
};
```

### LC695 Max Area of Island - BFS最大面积
```cpp
// 核心：BFS求连通分量面积，取max
class Solution {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        int m=grid.size(), n=grid[0].size(), ans=0;
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++)
                if(grid[i][j]==1)
                    ans=max(ans,bfs(grid,i,j));
        return ans;
    }
private:
    int dirs[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
    int bfs(vector<vector<int>>& g, int si, int sj){
        int m=g.size(), n=g[0].size(), area=0;
        queue<pair<int,int>> q;
        q.push({si,sj});
        g[si][sj]=0;
        while(!q.empty()){
            auto [i,j]=q.front(); q.pop();
            area++;
            for(auto& d:dirs){
                int ni=i+d[0], nj=j+d[1];
                if(ni>=0&&ni<m&&nj>=0&&nj<n&&g[ni][nj]==1){
                    g[ni][nj]=0;
                    q.push({ni,nj});
                }
            }
        }
        return area;
    }
};
```

---

## 验证结果
| 题目 | 核心 | 验证 |
|------|------|------|
| LC79 Word Search | DFS回溯+visited | ✅ 编译通过 |
| LC130 Surrounded Regions | 边界感染法 | ✅ 编译通过 |
| LC695 Max Area of Island | BFS最大面积 | ✅ 编译通过 |

---

## 今日LC累计
- 04-19早: LC Trie+DSU+Heap (5题)
- 04-19早: LC DP进阶 (5题)
- 04-19早: LC Hard Combo (7题)
- 04-19下午: LC128/134/135 (3题)
- 04-19下午: LC329/174/123/188 Hard DP (4题)
- **04-19傍晚: 回溯+连通 (3题)**
- **累计: ~170题 | 37+专题**
