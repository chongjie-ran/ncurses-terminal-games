// Advanced topics practice - 2026-04-15

// LC207 Course Schedule - Topological Sort (Kahn's Algorithm)
#include <vector>
#include <queue>
using namespace std;

bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> inDegree(numCourses, 0);
    
    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);
        inDegree[p[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++) {
        if (inDegree[i] == 0) q.push(i);
    }
    
    int count = 0;
    while (!q.empty()) {
        int course = q.front(); q.pop();
        count++;
        for (int next : adj[course]) {
            if (--inDegree[next] == 0) q.push(next);
        }
    }
    
    return count == numCourses;
}

// LC685 Redundant Connection II - Union-Find with cycle detection
class UnionFind {
public:
    vector<int> parent, rank;
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;
        if (rank[ra] < rank[rb]) swap(ra, rb);
        parent[rb] = ra;
        if (rank[ra] == rank[rb]) rank[ra]++;
        return true;
    }
};

// Test verification
int main() {
    // LC207: 2 courses, [[1,0]] -> can finish
    // LC685: redundant connection detection
    return 0;
}
