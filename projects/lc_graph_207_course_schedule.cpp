// LC207 Course Schedule - 检测有向图是否有环 (Kahn算法/BFS拓扑排序)
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        vector<int> indegree(numCourses, 0);
        for(auto& p: prerequisites){
            adj[p[1]].push_back(p[0]);
            indegree[p[0]]++;
        }
        queue<int> q;
        for(int i=0;i<numCourses;i++){
            if(indegree[i]==0) q.push(i);
        }
        int visited = 0;
        while(!q.empty()){
            int u = q.front(); q.pop();
            visited++;
            for(int v: adj[u]){
                if(--indegree[v]==0) q.push(v);
            }
        }
        return visited == numCourses;
    }
    
    // DFS检测环
    bool canFinishDFS(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        for(auto& p: prerequisites){
            adj[p[1]].push_back(p[0]);
        }
        vector<int> state(numCourses, 0); // 0=未访问, 1=访问中, 2=完成
        for(int i=0;i<numCourses;i++){
            if(dfs(i, adj, state)) return false;
        }
        return true;
    }
private:
    bool dfs(int u, vector<vector<int>>& adj, vector<int>& state){
        if(state[u]==1) return true;  // 访问中发现环
        if(state[u]==2) return false; // 已完成，无环
        state[u]=1;
        for(int v: adj[u]){
            if(dfs(v, adj, state)) return true;
        }
        state[u]=2;
        return false;
    }
};

int main(){
    Solution s;
    vector<vector<int>> p1 = {{1,0}};
    cout << s.canFinish(2, p1) << endl; // true (可以完成)
    
    vector<vector<int>> p2 = {{1,0},{0,1}};
    cout << s.canFinish(2, p2) << endl; // false (有环)
    
    return 0;
}
