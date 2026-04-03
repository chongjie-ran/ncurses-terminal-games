// LC112 Path Sum
// 分类: Binary Tree DFS
// 难度: Easy
// 思路: 从根到叶子的路径上, sum等于targetSum
// 递归: targetSum - root.val, 到叶子节点时判断是否为0
// 时间: O(n), 空间: O(h)

#include <iostream>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    // 递归法: 从当前节点出发, 判断是否存在到叶子节点的路径和为targetSum
    bool hasPathSum(TreeNode* root, int targetSum) {
        if (!root) return false;
        // 叶子节点, 且差值为0
        if (!root->left && !root->right) {
            return root->val == targetSum;
        }
        // 递归检查左右子树
        return hasPathSum(root->left, targetSum - root->val) ||
               hasPathSum(root->right, targetSum - root->val);
    }
    
    // 迭代法: 用栈保存 (node, remaining_sum)
    bool hasPathSumIter(TreeNode* root, int targetSum) {
        if (!root) return false;
        // 栈: pair(node, remaining_sum)
        struct Item { TreeNode* node; int sum; };
        Item stack[10000];
        int top = 0;
        stack[top++] = {root, targetSum - root->val};
        
        while (top > 0) {
            Item cur = stack[--top];
            if (!cur.node->left && !cur.node->right && cur.sum == 0) {
                return true;
            }
            if (cur.node->right) stack[top++] = {cur.node->right, cur.sum - cur.node->right->val};
            if (cur.node->left)  stack[top++] = {cur.node->left, cur.sum - cur.node->left->val};
        }
        return false;
    }
};
