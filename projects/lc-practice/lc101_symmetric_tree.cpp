// LC101 - Symmetric Tree
// 验证二叉树是否镜像对称
// 方法: 递归比较左子树和右子树
#include <iostream>
#include <queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {}
};

// 递归版本: 检查p和q是否镜像对称
bool isMirror(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return (p->val == q->val) 
        && isMirror(p->left, q->right) 
        && isMirror(p->right, q->left);
}

bool isSymmetric(TreeNode* root) {
    if (!root) return true;
    return isMirror(root->left, root->right);
}

// 迭代版本: 使用队列
bool isSymmetricIterative(TreeNode* root) {
    if (!root) return true;
    queue<TreeNode*> q;
    q.push(root->left);
    q.push(root->right);
    while (!q.empty()) {
        TreeNode* l = q.front(); q.pop();
        TreeNode* r = q.front(); q.pop();
        if (!l && !r) continue;
        if (!l || !r) return false;
        if (l->val != r->val) return false;
        q.push(l->left);
        q.push(r->right);
        q.push(l->right);
        q.push(r->left);
    }
    return true;
}

// 测试
int main() {
    // 构建对称树: [1,2,2,3,4,4,3]
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(2);
    root->left->left = new TreeNode(3);
    root->left->right = new TreeNode(4);
    root->right->left = new TreeNode(4);
    root->right->right = new TreeNode(3);
    
    cout << "Symmetric: " << (isSymmetric(root) ? "YES" : "NO") << endl;
    cout << "Iterative: " << (isSymmetricIterative(root) ? "YES" : "NO") << endl;
    
    // 构建非对称树: [1,2,3,3,null,null,4]
    TreeNode* root2 = new TreeNode(1);
    root2->left = new TreeNode(2);
    root2->right = new TreeNode(3);
    root2->left->right = new TreeNode(3);
    root2->right->right = new TreeNode(4);
    cout << "Asymmetric: " << (isSymmetric(root2) ? "YES" : "NO") << endl;
    
    return 0;
}
