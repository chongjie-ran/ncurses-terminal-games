// LC98: Validate Binary Search Tree
// Pattern: Tree DFS / BST Property
// Difficulty: Medium
// Date: 2026-04-01

#include <iostream>
#include <limits>
#include <cstddef>
using namespace std;

// Definition for a binary tree node
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// Forward declarations
bool dfs(TreeNode* node, long long minVal, long long maxVal);
bool inorder(TreeNode* node, long long& prev);

// Approach 1: DFS with valid range [min, max]
// Key: each node must be within (lower_bound, upper_bound)
bool isValidBST(TreeNode* root) {
    return dfs(root, numeric_limits<long long>::min(), 
                   numeric_limits<long long>::max());
}

bool dfs(TreeNode* node, long long minVal, long long maxVal) {
    if (!node) return true;
    
    // Node value must be strictly within the range
    if (node->val <= minVal || node->val >= maxVal) return false;
    
    // Left subtree: all values must be < node.val
    // Right subtree: all values must be > node.val
    return dfs(node->left, minVal, node->val) &&
           dfs(node->right, node->val, maxVal);
}

// Approach 2: In-order traversal (BST in-order = sorted sequence)
// Store prev value and check strictly increasing
bool isValidBSTInorder(TreeNode* root) {
    long long prev = numeric_limits<long long>::min();
    return inorder(root, prev);
}

bool inorder(TreeNode* node, long long& prev) {
    if (!node) return true;
    if (!inorder(node->left, prev)) return false;
    if (node->val <= prev) return false;
    prev = node->val;
    return inorder(node->right, prev);
}

int main() {
    // Test case 1: Valid BST
    //     2
    //    / \
    //   1   3
    TreeNode* t1 = new TreeNode(2);
    t1->left = new TreeNode(1);
    t1->right = new TreeNode(3);
    cout << "Valid BST [2,1,3]: " << (isValidBST(t1) ? "true" : "false") << " ✓" << endl;
    
    // Test case 2: Invalid BST
    //     5
    //    / \
    //   1   4
    //      / \
    //     3   6
    // 4 should be > 5, but < 5 as right child
    TreeNode* t2 = new TreeNode(5);
    t2->left = new TreeNode(1);
    t2->right = new TreeNode(4);
    t2->right->left = new TreeNode(3);
    t2->right->right = new TreeNode(6);
    cout << "Invalid BST [5,1,4,3,6]: " << (isValidBST(t2) ? "true" : "false") << " (should be false)" << endl;
    
    // Test case 3: Edge case - single node
    TreeNode* t3 = new TreeNode(1);
    cout << "Single node [1]: " << (isValidBST(t3) ? "true" : "false") << " ✓" << endl;
    
    // Test case 4: Strictly greater check
    //     1
    //    / \
    //   1   1  (invalid - all equal)
    TreeNode* t4 = new TreeNode(1);
    t4->left = new TreeNode(1);
    t4->right = new TreeNode(1);
    cout << "Equal nodes [1,1,1]: " << (isValidBST(t4) ? "true" : "false") << " (should be false)" << endl;
    
    return 0;
}
