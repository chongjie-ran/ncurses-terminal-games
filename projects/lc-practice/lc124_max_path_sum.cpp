/*
LC124 - Binary Tree Maximum Path Sum
分类: Tree DP
难度: Hard
日期: 2026-04-05

思路:
  后序遍历 + 维护全局最大:
  maxGain(node) = max(0, maxGain(left), maxGain(right)) + node->val
  每个节点的可能最大路径 = left_gain + right_gain + node->val
  
关键洞察:
  - 路径可以从任意节点开始和结束（不是从根到叶子）
  - maxGain表示包含该节点且向下一个方向延伸的最大贡献
  - 全局max在整个递归过程中维护

时间: O(n), 空间: O(h)递归栈
*/

#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    int maxSum;
    Solution(): maxSum(INT_MIN) {}
    
    int maxGain(TreeNode* node) {
        if (!node) return 0;
        int left = max(maxGain(node->left), 0);
        int right = max(maxGain(node->right), 0);
        int pathThroughNode = left + right + node->val;
        maxSum = max(maxSum, pathThroughNode);
        return node->val + max(left, right);
    }
    
    int maxPathSum(TreeNode* root) {
        maxGain(root);
        return maxSum;
    }
};

int main() {
    Solution s;
    // [1,2,3] → 6
    TreeNode* t1 = new TreeNode(1);
    t1->left = new TreeNode(2);
    t1->right = new TreeNode(3);
    assert(s.maxPathSum(t1) == 6);
    cout << "Test1=6 ✅ [1,2,3]";
    
    // [-10,9,20,null,null,15,7] → 42
    Solution s2;
    TreeNode* t2 = new TreeNode(-10);
    t2->left = new TreeNode(9);
    t2->right = new TreeNode(20);
    t2->right->left = new TreeNode(15);
    t2->right->right = new TreeNode(7);
    assert(s2.maxPathSum(t2) == 42);
    cout << ", Test2=42 ✅ [-10,9,20,15,7]";
    
    // [2,-1] → 2
    Solution s3;
    TreeNode* t3 = new TreeNode(2);
    t3->right = new TreeNode(-1);
    assert(s3.maxPathSum(t3) == 2);
    cout << ", Test3=2 ✅ [2,-1]";
    
    cout << "\nAll tests PASS\n";
    return 0;
}
