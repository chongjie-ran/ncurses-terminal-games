// LC94 - Binary Tree Inorder Traversal
// 中序遍历: 左-根-右 (BST排序输出)
// 方法: 递归 + 迭代(栈)
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {}
};

// 递归版本
void inorderRecursive(TreeNode* root, vector<int>& result) {
    if (!root) return;
    inorderRecursive(root->left, result);
    result.push_back(root->val);
    inorderRecursive(root->right, result);
}

// 迭代版本 (用栈模拟递归)
vector<int> inorderIterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> st;
    TreeNode* cur = root;
    while (cur || !st.empty()) {
        // 一路向左入栈
        while (cur) {
            st.push(cur);
            cur = cur->left;
        }
        // 弹出, 访问根, 转向右子树
        cur = st.top(); st.pop();
        result.push_back(cur->val);
        cur = cur->right;
    }
    return result;
}

// Morris遍历 (O(1)空间, 无栈无递归)
// 思想: 线索二叉树, 利用null指针指向后继
vector<int> inorderMorris(TreeNode* root) {
    vector<int> result;
    TreeNode* cur = root;
    while (cur) {
        if (!cur->left) {
            result.push_back(cur->val);
            cur = cur->right;
        } else {
            // 找前驱(左子树最右节点)
            TreeNode* pre = cur->left;
            while (pre->right && pre->right != cur) {
                pre = pre->right;
            }
            if (!pre->right) {
                // 建立线索
                pre->right = cur;
                cur = cur->left;
            } else {
                // 访问根
                result.push_back(cur->val);
                pre->right = nullptr; // 删除线索
                cur = cur->right;
            }
        }
    }
    return result;
}

int main() {
    // 构建树: [1,2,3,4,5]
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    
    vector<int> result;
    inorderRecursive(root, result);
    cout << "Recursive: ";
    for (int v : result) cout << v << " ";
    cout << endl;
    
    vector<int> result2 = inorderIterative(root);
    cout << "Iterative: ";
    for (int v : result2) cout << v << " ";
    cout << endl;
    
    vector<int> result3 = inorderMorris(root);
    cout << "Morris:    ";
    for (int v : result3) cout << v << " ";
    cout << endl;
    
    return 0;
}
