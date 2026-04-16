// LC207: 课程表 - 拓扑排序 (BFS/DFS)
// 判断有向图是否有环
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Solution207 {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        vector<int> indegree(numCourses, 0);
        for(auto& p: prerequisites) {
            adj[p[1]].push_back(p[0]);
            indegree[p[0]]++;
        }
        
        queue<int> q;
        for(int i=0;i<numCourses;i++)
            if(indegree[i]==0) q.push(i);
        
        int visited=0;
        while(!q.empty()) {
            int course=q.front(); q.pop();
            visited++;
            for(int next: adj[course]) {
                if(--indegree[next]==0) q.push(next);
            }
        }
        return visited==numCourses;
    }
};

// 方法2: DFS检测环
class Solution207_DFS {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        for(auto& p: prerequisites) adj[p[1]].push_back(p[0]);
        
        vector<int> state(numCourses, 0);
        for(int i=0;i<numCourses;i++)
            if(dfs(i, adj, state)) return false;
        return true;
    }
private:
    bool dfs(int course, vector<vector<int>>& adj, vector<int>& state) {
        if(state[course]==1) return true;
        if(state[course]==2) return false;
        state[course]=1;
        for(int next: adj[course])
            if(dfs(next, adj, state)) return true;
        state[course]=2;
        return false;
    }
};

int main() {
    Solution207 s;
    vector<vector<int>> p1 = {{1,0},{2,0},{3,1},{3,2}};
    cout<<"LC207: "<<(s.canFinish(4,p1)?"true":"false")<<" (expected true)"<<endl;
    
    Solution207_DFS s2;
    cout<<"LC207(DFS): "<<(s2.canFinish(4,p1)?"true":"false")<<endl;
    
    return 0;
}
