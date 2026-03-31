// LC133 Clone Graph - BFS/DFS 图克隆
// 2026-03-31 08:00
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

// Definition for Node (Neos)
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() : val(0), neighbors(vector<Node*>()) {}
    Node(int _val) : val(_val), neighbors(vector<Node*>()) {}
    Node(int _val, vector<Node*> _neighbors) : val(_val), neighbors(_neighbors) {}
};

// 方法1: BFS (推荐)
Node* cloneGraphBFS(Node* node) {
    if (!node) return nullptr;
    unordered_map<Node*, Node*> mp; // old -> new
    
    queue<Node*> q;
    q.push(node);
    mp[node] = new Node(node->val);
    
    while (!q.empty()) {
        Node* cur = q.front(); q.pop();
        for (Node* n : cur->neighbors) {
            if (mp.find(n) == mp.end()) {
                mp[n] = new Node(n->val);
                q.push(n);
            }
            mp[cur]->neighbors.push_back(mp[n]);
        }
    }
    return mp[node];
}

// 方法2: DFS 递归
Node* cloneGraphDFS(Node* node) {
    if (!node) return nullptr;
    static unordered_map<Node*, Node*> mp;
    
    if (mp.find(node) != mp.end()) return mp[node];
    
    Node* clone = new Node(node->val);
    mp[node] = clone;
    
    for (Node* n : node->neighbors) {
        clone->neighbors.push_back(cloneGraphDFS(n));
    }
    return clone;
}

// 关键洞察: unordered_map 处理 visited，old->new 映射
// BFS: queue + map，遍历原图同时构建新图
// DFS: 递归 + map，post-order 构建
