// LC547 Number of Provinces
// Medium - Union-Find / DFS
// 给定n×n邻接矩阵isConnected,城市i到j有边表示同一省份,计算省份数量
// Time: O(n^2) Space: O(n)

#include <vector>
#include <iostream>
using namespace std;

// 方法1: Union-Find (推荐 - 可处理动态合并)
class UnionFind {
    vector<int> parent, rank;
public:
    UnionFind(int n): parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]); // 路径压缩
        return parent[x];
    }
    void unionSets(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        // 按秩合并
        if (rank[px] < rank[py]) swap(px, py);
        parent[py] = px;
        if (rank[px] == rank[py]) rank[px]++;
    }
    int count() {
        int cnt = 0;
        for (int i = 0; i < parent.size(); i++) 
            if (parent[i] == i) cnt++;
        return cnt;
    }
};

class Solution1 {
public:
    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        UnionFind uf(n);
        for (int i = 0; i < n; i++) {
            for (int j = i+1; j < n; j++) { // 只看上三角
                if (isConnected[i][j] == 1) {
                    uf.unionSets(i, j);
                }
            }
        }
        return uf.count();
    }
};

// 方法2: DFS (图的遍历)
class Solution2 {
public:
    int findCircleNum(vector<vector<int>>& isConnected) {
        int n = isConnected.size();
        int provinces = 0;
        vector<int> visited(n, 0);
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                dfs(isConnected, visited, i, n);
                provinces++;
            }
        }
        return provinces;
    }
private:
    void dfs(vector<vector<int>>& M, vector<int>& visited, int i, int n) {
        for (int j = 0; j < n; j++) {
            if (M[i][j] == 1 && !visited[j]) {
                visited[j] = 1;
                dfs(M, visited, j, n);
            }
        }
    }
};

// 测试
int main() {
    Solution1 s1;
    Solution2 s2;
    
    vector<vector<int>> g1 = {
        {1,1,0},
        {1,1,0},
        {0,0,1}
    };
    cout << "Test1: " << s1.findCircleNum(g1) << " (expected 2)" << endl;
    
    vector<vector<int>> g2 = {
        {1,0,0,1},
        {0,1,1,0},
        {0,1,1,1},
        {1,0,1,1}
    };
    cout << "Test2: " << s2.findCircleNum(g2) << " (expected 1)" << endl;
    
    vector<vector<int>> g3 = {{1,0,0},{0,1,0},{0,0,1}};
    cout << "Test3: " << s1.findCircleNum(g3) << " (expected 3)" << endl;
    
    return 0;
}

// 核心洞察:
// 1. 邻接矩阵isConnected: isConnected[i][j]=1表示i和j相连(同一省份)
// 2. Union-Find: 遍历上三角矩阵,将相连的城市合并,最后数root数量
// 3. DFS: 对每个未访问城市做DFS标记所有可达城市,省份数+1
// 4. 时间O(n^2): 遍历邻接矩阵建立union;空间O(n): parent数组或visited
// 5. 本质上与LC200 Number of Islands相同,只是输入格式不同(矩阵vs列表)
