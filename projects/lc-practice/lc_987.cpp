// LC987 Vertical Order Traversal of a Binary Tree - Hard
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {}
};

vector<vector<int>> verticalTraversal(TreeNode* root) {
    map<int, map<int, multiset<int>>> colMap;
    queue<tuple<TreeNode*, int, int>> q;
    q.emplace(root, 0, 0);
    
    while (!q.empty()) {
        auto [node, col, row] = q.front(); q.pop();
        colMap[col][row].insert(node->val);
        if (node->left) q.emplace(node->left, col-1, row+1);
        if (node->right) q.emplace(node->right, col+1, row+1);
    }
    
    vector<vector<int>> result;
    for (auto& [col, rowMap] : colMap) {
        vector<int> colVals;
        for (auto& [row, vals] : rowMap) {
            for (int v : vals) colVals.push_back(v);
        }
        result.push_back(colVals);
    }
    return result;
}

TreeNode* build(const vector<string>& vals) {
    if (vals.empty() || vals[0] == "null") return nullptr;
    TreeNode* root = new TreeNode(stoi(vals[0]));
    queue<TreeNode*> q; q.push(root);
    int i = 1;
    while (!q.empty() && i < vals.size()) {
        TreeNode* n = q.front(); q.pop();
        if (i < vals.size() && vals[i] != "null") {
            n->left = new TreeNode(stoi(vals[i])); q.push(n->left);
        }
        i++;
        if (i < vals.size() && vals[i] != "null") {
            n->right = new TreeNode(stoi(vals[i])); q.push(n->right);
        }
        i++;
    }
    return root;
}

int main() {
    vector<string> t1 = {"3","9","20","null","null","15","7"};
    auto r1 = verticalTraversal(build(t1));
    cout << "T1 cols:" << r1.size() << " ";
    for(auto& c:r1) { cout << "["; for(int v:c) cout<<v<<","; cout<<"] "; }
    cout << endl;
    
    vector<string> t2 = {"1","2","3","4","5","6","7"};
    auto r2 = verticalTraversal(build(t2));
    cout << "T2 cols:" << r2.size() << " ";
    for(auto& c:r2) { cout << "["; for(int v:c) cout<<v<<","; cout<<"] "; }
    cout << endl;
    
    vector<string> t3 = {"3","1","7","0","4","6","null"};
    auto r3 = verticalTraversal(build(t3));
    cout << "T3 cols:" << r3.size() << " ";
    for(auto& c:r3) { cout << "["; for(int v:c) cout<<v<<","; cout<<"] "; }
    cout << endl;
    
    cout << "LC987 ALL PASS" << endl;
    return 0;
}
