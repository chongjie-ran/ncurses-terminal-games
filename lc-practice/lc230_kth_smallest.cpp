// LC230: 二叉搜索树中第K小的元素
// 难度: Medium | 分类: 二叉搜索树
// 核心: BST中序遍历得到有序序列
#include <iostream>
#include <vector>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 方法1: 中序遍历 + 计数
class Solution1 {
public:
    int kthSmallest(TreeNode* root, int k) {
        int count = 0;
        return inorder(root, k, count);
    }
    
    int inorder(TreeNode* node, int k, int& count) {
        if (!node) return -1;
        
        // 左
        int left = inorder(node->left, k, count);
        if (left != -1) return left;
        
        // 根
        count++;
        if (count == k) return node->val;
        
        // 右
        return inorder(node->right, k, count);
    }
};

// 方法2: 堆排序思想
class Solution2 {
public:
    int kthSmallest(TreeNode* root, int k) {
        vector<int> nums;
        stack<TreeNode*> st;
        TreeNode* curr = root;
        
        while (curr || !st.empty()) {
            while (curr) {
                st.push(curr);
                curr = curr->left;
            }
            curr = st.top(); st.pop();
            nums.push_back(curr->val);
            curr = curr->right;
        }
        
        return nums[k-1];
    }
};

// 测试
int main() {
    // 构建 BST:       5
    //             3     7
    //           2   4 6   8
    TreeNode* root = new TreeNode(5);
    root->left = new TreeNode(3);
    root->right = new TreeNode(7);
    root->left->left = new TreeNode(2);
    root->left->right = new TreeNode(4);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(8);
    
    Solution1 sol1;
    cout << "LC230 Kth Smallest:" << endl;
    for (int k = 1; k <= 8; k++) {
        cout << "k=" << k << " -> " << sol1.kthSmallest(root, k) << endl;
    }
    
    cout << endl << "验证: [2,3,4,5,6,7,8]" << endl;
    
    return 0;
}
