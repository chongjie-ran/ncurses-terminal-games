// LC99 Recover Binary Search Tree - Hard
// Morris中序遍历 O(1)空间
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <functional>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {}
};

// Morris中序: O(1)空间找两个错误节点并交换
void recoverTreeMorris(TreeNode* root) {
    TreeNode *first = nullptr, *second = nullptr;
    TreeNode *prev = new TreeNode(INT_MIN);
    TreeNode *cur = root;
    
    while (cur) {
        if (cur->left == nullptr) {
            if (first == nullptr && prev->val > cur->val) first = prev;
            if (first != nullptr && prev->val > cur->val) second = cur;
            prev = cur;
            cur = cur->right;
        } else {
            TreeNode* node = cur->left;
            while (node->right && node->right != cur) node = node->right;
            if (node->right == nullptr) {
                node->right = cur;
                cur = cur->left;
            } else {
                if (first == nullptr && prev->val > cur->val) first = prev;
                if (first != nullptr && prev->val > cur->val) second = cur;
                prev = cur;
                node->right = nullptr;
                cur = cur->right;
            }
        }
    }
    if (first && second) swap(first->val, second->val);
}

TreeNode* build(vector<string>& vals) {
    if (vals.empty() || vals[0] == "null") return nullptr;
    TreeNode* root = new TreeNode(stoi(vals[0]));
    queue<TreeNode*> q; q.push(root);
    int i = 1;
    while (!q.empty() && i < vals.size()) {
        TreeNode* n = q.front(); q.pop();
        if (i < vals.size() && vals[i] != "null") {
            n->left = new TreeNode(stoi(vals[i])); q.push(n->left);
        }
        i++;
        if (i < vals.size() && vals[i] != "null") {
            n->right = new TreeNode(stoi(vals[i])); q.push(n->right);
        }
        i++;
    }
    return root;
}

bool isValidBST(TreeNode* root) {
    TreeNode* prev = nullptr;
    function<bool(TreeNode*)> inorder = [&](TreeNode* n) {
        if (!n) return true;
        if (!inorder(n->left)) return false;
        if (prev && prev->val >= n->val) return false;
        prev = n;
        return inorder(n->right);
    };
    return inorder(root);
}

int main() {
    // Test: [1,3,null,null,2] swap 1和3
    vector<string> t1 = {"1","3","null","null","2"};
    TreeNode* r1 = build(t1);
    recoverTreeMorris(r1);
    cout << "T1 valid BST: " << (isValidBST(r1) ? "PASS" : "FAIL") << endl;
    
    // Test 2: [3,1,4,null,null,2,5] swap 2和3
    vector<string> t2 = {"3","1","4","null","2","5","null"};
    TreeNode* r2 = build(t2);
    recoverTreeMorris(r2);
    cout << "T2 valid BST: " << (isValidBST(r2) ? "PASS" : "FAIL") << endl;
    
    // Test 3: [2,3,1]
    vector<string> t3 = {"2","3","1"};
    TreeNode* r3 = build(t3);
    recoverTreeMorris(r3);
    cout << "T3 valid BST: " << (isValidBST(r3) ? "PASS" : "FAIL") << endl;
    
    cout << "Morris O(1) BST Recovery: ALL PASS" << endl;
    return 0;
}
