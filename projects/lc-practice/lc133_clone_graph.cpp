// LC133 Clone Graph
// BFS with visited map (unordered_map<node*, Node*>)
// Time: O(V+E), Space: O(V)

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() : val(0), neighbors(vector<Node*>()) {}
    Node(int _val) : val(_val), neighbors(vector<Node*>()) {}
    Node(int _val, vector<Node*> _neighbors) : val(_val), neighbors(_neighbors) {}
};

class Solution {
public:
    Node* cloneGraph(Node* node) {
        if (!node) return nullptr;
        unordered_map<Node*, Node*> visited;
        queue<Node*> q;
        
        // Clone root
        visited[node] = new Node(node->val);
        q.push(node);
        
        while (!q.empty()) {
            Node* cur = q.front(); q.pop();
            for (Node* neighbor : cur->neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited[neighbor] = new Node(neighbor->val);
                    q.push(neighbor);
                }
                // Connect cloned node
                visited[cur]->neighbors.push_back(visited[neighbor]);
            }
        }
        return visited[node];
    }
};
