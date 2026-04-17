#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
using namespace std;

// 并查集模板
class UnionFind {
public:
    vector<int> parent, rank;
    
    UnionFind(int n) {
        parent.resize(n);
        rank.assign(n, 0);
        iota(parent.begin(), parent.end(), 0);
    }
    
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // 路径压缩
        }
        return parent[x];
    }
    
    void unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        if (rank[px] < rank[py]) {
            parent[px] = py;
        } else if (rank[px] > rank[py]) {
            parent[py] = px;
        } else {
            parent[py] = px;
            rank[px]++;
        }
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

// ===== LC547 省份数量 =====
int findCircleNum(vector<vector<int>>& isConnected) {
    int n = isConnected.size();
    UnionFind uf(n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (isConnected[i][j]) uf.unite(i, j);
        }
    }
    unordered_set<int> provinces;
    for (int i = 0; i < n; i++) provinces.insert(uf.find(i));
    return provinces.size();
}

// ===== LC200 岛屿数量 =====
int numIslands(vector<vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    int m = grid.size(), n = grid[0].size();
    UnionFind uf(m * n);
    vector<pair<int,int>> dirs = {{-1,0},{1,0},{0,-1},{0,1}};
    
    auto idx = [&](int i, int j) { return i * n + j; };
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                for (auto [di, dj] : dirs) {
                    int ni = i + di, nj = j + dj;
                    if (ni>=0 && ni<m && nj>=0 && nj<n && grid[ni][nj]=='1') {
                        uf.unite(idx(i,j), idx(ni,nj));
                    }
                }
            }
        }
    }
    
    unordered_set<int> islands;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (grid[i][j] == '1')
                islands.insert(uf.find(idx(i,j)));
    return islands.size();
}

// ===== LC721 账户合并 =====
vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
    unordered_map<string, int> email_to_idx;
    unordered_map<string, string> email_to_name;
    int n = accounts.size();
    UnionFind uf(n);
    
    for (int i = 0; i < n; i++) {
        for (int j = 1; j < accounts[i].size(); j++) {
            string email = accounts[i][j];
            email_to_name[email] = accounts[i][0];
            if (email_to_idx.find(email) == email_to_idx.end()) {
                email_to_idx[email] = i;
            } else {
                uf.unite(i, email_to_idx[email]);
            }
        }
    }
    
    unordered_map<int, vector<string>> groups;
    for (auto& [email, idx] : email_to_idx) {
        int root = uf.find(idx);
        groups[root].push_back(email);
    }
    
    vector<vector<string>> result;
    for (auto& [root, emails] : groups) {
        sort(emails.begin(), emails.end());
        vector<string> acc;
        acc.push_back(email_to_name[emails[0]]);
        acc.insert(acc.end(), emails.begin(), emails.end());
        result.push_back(acc);
    }
    return result;
}

int main() {
    // ===== LC547 测试 =====
    cout << "=== LC547 省份数量 ===" << endl;
    vector<vector<int>> isConnected = {
        {1,1,0},
        {1,1,0},
        {0,0,1}
    };
    cout << "expect 2, got " << findCircleNum(isConnected) << endl;
    isConnected = {{1,0,0},{0,1,0},{0,0,1}};
    cout << "expect 3, got " << findCircleNum(isConnected) << endl;
    
    // ===== LC200 测试 =====
    cout << "=== LC200 岛屿数量 ===" << endl;
    vector<vector<char>> grid = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    cout << "expect 1, got " << numIslands(grid) << endl;
    grid = {
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };
    cout << "expect 3, got " << numIslands(grid) << endl;
    
    // ===== LC721 测试 =====
    cout << "=== LC721 账户合并 ===" << endl;
    vector<vector<string>> accounts = {
        {"John","johnsmith@mail.com","john_newyork@mail.com"},
        {"John","johnsmith@mail.com","john00@mail.com"},
        {"Mary","mary@mail.com"},
        {"John","johnnybravo@mail.com"}
    };
    auto res = accountsMerge(accounts);
    for (auto& acc : res) {
        for (auto& s : acc) cout << s << " ";
        cout << endl;
    }
    
    return 0;
}
