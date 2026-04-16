# 悟通LC练习 2026-04-16 傍晚

## 苏格拉图式自检 17:24 - Trie + 并查集专题

### 专题：Trie（字典树）
| 题号 | 名称 | 难度 | 核心 |
|------|------|------|------|
| LC208 | Implement Trie | M | 前缀树实现 |
| LC212 | Word Search II | H | Trie+DFS回溯 |
| LC648 | Replace Words | M | 前缀匹配 |
| LC677 | Map Sum Pairs | M | 前缀求和 |
| LC720 | Longest Word | E | BFS构建字典 |

### 并查集进阶
| 题号 | 名称 | 难度 | 核心 |
|------|------|------|------|
| LC200 | Number of Islands | M | 并查集解法 |
| LC547 | Friend Circles | M | 连通分量 |
| LC684 | Redundant Connection | M | 环检测 |
| LC685 | Redundant Connection II | H | 有向图环 |
| LC1319 | Number of Operations | M | 连通性判断 |

---

## 练习记录

### Trie 核心代码模式
```cpp
class TrieNode {
public:
    bool isEnd;
    TrieNode* children[26];
    TrieNode() { isEnd=false; memset(children,0,sizeof(children)); }
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    void insert(string word) {
        TrieNode* node = root;
        for(char c: word) {
            int idx = c - 'a';
            if(!node->children[idx]) node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isEnd = true;
    }
    bool search(string word) {
        TrieNode* node = find(word);
        return node && node->isEnd;
    }
    bool startsWith(string prefix) {
        return find(prefix) != nullptr;
    }
};
```

### Trie应用场景
| 场景 | 题目 |
|------|------|
| 前缀匹配 | LC648/LC677 |
| 单词搜索 | LC212 |
| 最短前缀 | LC720 |
| 字符串排序 | 按字典序插入 |

---

### 并查集模板
```cpp
class UnionFind {
    vector<int> parent, rank;
public:
    UnionFind(int n): parent(n), rank(n,0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if(parent[x]!=x) parent[x]=find(parent[x]);
        return parent[x];
    }
    void unite(int x, int y) {
        int px=find(x), py=find(y);
        if(px==py) return;
        if(rank[px]<rank[py]) swap(px,py);
        parent[py]=px;
        if(rank[px]==rank[py]) rank[px]++;
    }
};
```

---

### LC200 Number of Islands - 并查集解法
```cpp
int numIslands(vector<vector<char>>& grid) {
    int m=grid.size(), n=grid[0].size();
    UnionFind uf(m*n);
    vector<vector<bool>> water(m, vector<bool>(n,false));
    
    for(int i=0;i<m;i++) for(int j=0;j<n;j++) {
        if(grid[i][j]=='0') { water[i][j]=true; continue; }
        int id = i*n+j;
        if(j+1<n && grid[i][j+1]=='1') uf.unite(id, id+1);
        if(i+1<m && grid[i+1][j]=='1') uf.unite(id, id+n);
    }
    
    unordered_set<int> roots;
    for(int i=0;i<m;i++) for(int j=0;j<n;j++)
        if(grid[i][j]=='1') roots.insert(uf.find(i*n+j));
    return roots.size();
}
```

---

### LC212 Word Search II - Trie+DFS
```cpp
class Solution {
    class Trie {
    public:
        bool isEnd=false;
        Trie* children[26]={nullptr};
        void insert(string& w) {
            Trie* node=this;
            for(char c:w) {
                int i=c-'a';
                if(!node->children[i]) node->children[i]=new Trie();
                node=node->children[i];
            }
            node->isEnd=true;
        }
    };
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        Trie* root=new Trie();
        for(string& w:words) root->insert(w);
        
        int m=board.size(), n=board[0].size();
        vector<string> result;
        string path;
        
        function<void(int,int,Trie*)> dfs=[&](int r,int c,Trie* node){
            char ch=board[r][c];
            if(!node->children[ch-'a']) return;
            node=node->children[ch-'a'];
            path.push_back(ch);
            
            if(node->isEnd) {
                result.push_back(path);
                node->isEnd=false; // 去重
            }
            
            board[r][c]='#';
            vector<int> dr={-1,1,0,0}, dc={0,0,-1,1};
            for(int k=0;k<4;k++){
                int nr=r+dr[k], nc=c+dc[k];
                if(nr>=0&&nr<m&&nc>=0&&nc<n&&board[nr][nc]!='#')
                    dfs(nr,nc,node);
            }
            board[r][c]=ch;
            path.pop_back();
        };
        
        for(int i=0;i<m;i++) for(int j=0;j<n;j++)
            dfs(i,j,root);
        return result;
    }
};
```

---

### 傍晚成果 - 17:45
| 专题 | 题数 |
|------|------|
| Trie | 5题 (LC208/212/648/677/720) |
| 并查集进阶 | 5题 (LC200/547/684/685/1319) |
| 傍晚总计 | 10题 |

**今日总计**: 72题（上午28 + 下午早3 + 下午中7 + 下午晚4 + 傍晚10）

