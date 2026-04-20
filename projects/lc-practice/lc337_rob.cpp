#include <iostream>
#include <unordered_map>
using namespace std;

// LC337 House Robber III - 二叉树版本
// 问题：二叉树中选不相邻的节点求最大金额
// 核心：DFS返回 (not_rob, rob) 元组

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

pair<int,int> dfs(TreeNode* node, unordered_map<TreeNode*,pair<int,int>>& memo) {
    if (!node) return {0, 0};
    if (memo.count(node)) return memo[node];
    
    auto left = dfs(node->left, memo);
    auto right = dfs(node->right, memo);
    
    int not_rob = left.second + right.second;  // 不抢此节点，子节点可抢
    int rob = node->val + left.first + right.first;  // 抢此节点，子节点不可抢
    
    memo[node] = {not_rob, max(not_rob, rob)};
    return memo[node];
}

int rob(TreeNode* root) {
    unordered_map<TreeNode*,pair<int,int>> memo;
    return dfs(root, memo).second;
}

int main() {
    // [3,2,3,null,3,null,1] -> 7
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->right = new TreeNode(3);
    root->right->right = new TreeNode(1);
    cout << rob(root) << endl;  // 7
    
    // [3,4,5,1,3,null,1] -> 9
    TreeNode* root2 = new TreeNode(3);
    root2->left = new TreeNode(4);
    root2->right = new TreeNode(5);
    root2->left->left = new TreeNode(1);
    root2->left->right = new TreeNode(3);
    root2->right->right = new TreeNode(1);
    cout << rob(root2) << endl;  // 9
    
    return 0;
}
