// LC104 Maximum Depth of Binary Tree
// 分类: Binary Tree DFS/BFS
// 难度: Easy
// 思路: 递归深度优先(后序遍历), 最大深度 = max(左子树深度, 右子树深度) + 1
// 时间: O(n), 空间: O(h) 其中h是树高度, 最坏O(n)

#include <iostream>
#include <algorithm>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    // 方法1: 递归DFS (后序遍历)
    int maxDepth(TreeNode* root) {
        if (!root) return 0;
        int left_depth = maxDepth(root->left);
        int right_depth = maxDepth(root->right);
        return max(left_depth, right_depth) + 1;
    }
    
    // 方法2: 迭代BFS (层序遍历, 计数层数)
    int maxDepthBFS(TreeNode* root) {
        if (!root) return 0;
        int depth = 0;
        // 用队列实现BFS
        TreeNode* queue[10000];
        int front = 0, back = 0;
        queue[back++] = root;
        while (front < back) {
            int level_size = back - front;
            for (int i = 0; i < level_size; i++) {
                TreeNode* node = queue[front++];
                if (node->left) queue[back++] = node->left;
                if (node->right) queue[back++] = node->right;
            }
            depth++;
        }
        return depth;
    }
};
