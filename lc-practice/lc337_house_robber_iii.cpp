// LC337 House Robber III - 二叉树房屋盗窃
// 状态: 每个节点返回 {rob, not_rob}
// rob = left.not_rob + right.not_rob + node.val
// not_rob = max(left.rob, left.not_rob) + max(right.rob, right.not_rob)
#include <iostream>
#include <vector>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    vector<int> dfs(TreeNode* node) {
        if (!node) return {0, 0};
        auto left = dfs(node->left);
        auto right = dfs(node->right);
        // rob = 不抢左右子树 + node值
        int rob = left[1] + right[1] + node->val;
        // not_rob = 左右子树各自的最大值
        int not_rob = max(left[0], left[1]) + max(right[0], right[1]);
        return {rob, not_rob};
    }
    int rob(TreeNode* root) {
        auto res = dfs(root);
        return max(res[0], res[1]);
    }
};

int main() {
    Solution s;
    // [3,2,3,null,3,null,1]
    TreeNode* r = new TreeNode(3);
    r->left = new TreeNode(2); r->right = new TreeNode(3);
    r->left->right = new TreeNode(3);
    r->right->right = new TreeNode(1);
    cout << s.rob(r) << " (expect 7)" << endl;
    
    // [3,4,5,1,3,null,1]
    TreeNode* r2 = new TreeNode(3);
    r2->left = new TreeNode(4); r2->right = new TreeNode(5);
    r2->left->left = new TreeNode(1); r2->left->right = new TreeNode(3);
    r2->right->right = new TreeNode(1);
    cout << s.rob(r2) << " (expect 9)" << endl;
    return 0;
}
