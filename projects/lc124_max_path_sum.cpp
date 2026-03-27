// LC124: Binary Tree Maximum Path Sum
// Hard - Tree DP
// O(n) time, O(h) space (recursive stack)
#include <algorithm>
#include <climits>
#include <iostream>
using namespace std;
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
private:
    int max_sum = INT_MIN;
    
    // 返回从该节点向下到叶子节点的最大路径和（单支）
    int dfs(TreeNode* node) {
        if (!node) return 0;
        
        // 计算左右子树的贡献（如果为负则丢弃）
        int left_gain = max(0, dfs(node->left));
        int right_gain = max(0, dfs(node->right));
        
        // 以当前节点为拐点的完整路径（左右+自己）
        int path_through_node = node->val + left_gain + right_gain;
        max_sum = max(max_sum, path_through_node);
        
        // 返回从该节点向下延伸的最大单支路径和
        return node->val + max(left_gain, right_gain);
    }
    
public:
    int maxPathSum(TreeNode* root) {
        dfs(root);
        return max_sum;
    }
};

// 测试
int main() {
    // [1,2,3] → 6 (1->2 + 1->3 = 6, 但实际是 1+2+3=6)
    // 但注意路径不能分叉，所以 2+1+3 不合法
    // 合法路径：1->2 (和3), 1->3 (和2), 2->1->3 (分叉，不合法)
    // 最大是 2->1->3 = 6，但这是分叉路径
    // 实际上最大路径可能是 2->1->3 = 6
    // 在本题中，路径可以在任意节点"拐弯"，所以 2->1->3 是合法的
    // 验证：root=[-10,9,20,null,null,15,7], 最大=42 (15->20->7=42)
    
    {
        Solution s;
        TreeNode* root = new TreeNode(-10);
        root->left = new TreeNode(9);
        root->right = new TreeNode(20);
        root->right->left = new TreeNode(15);
        root->right->right = new TreeNode(7);
        cout << s.maxPathSum(root) << endl; // 预期 42
    }
    {
        Solution s;
        TreeNode* single = new TreeNode(1);
        cout << s.maxPathSum(single) << endl; // 预期 1
    }
    {
        Solution s;
        TreeNode* neg = new TreeNode(-3);
        cout << s.maxPathSum(neg) << endl; // 预期 -3
    }
    
    return 0;
}
