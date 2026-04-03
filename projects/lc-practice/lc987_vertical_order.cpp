// LC987 Vertical Order Traversal of a Binary Tree
// Hard - BFS + Map
// 核心: col,row,val三元组排序, 用map自动排序

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;

// TreeNode定义
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void dfs(TreeNode* node, int row, int col,
         map<int, map<int, set<int>>>& mp) {
    if (!node) return;
    mp[col][row].insert(node->val);
    dfs(node->left, row + 1, col - 1, mp);
    dfs(node->right, row + 1, col + 1, mp);
}

vector<vector<int>> verticalTraversal(TreeNode* root) {
    map<int, map<int, set<int>>> mp;
    dfs(root, 0, 0, mp);

    vector<vector<int>> res;
    for (auto& colPair : mp) {
        vector<int> colVals;
        for (auto& rowPair : colPair.second) {
            for (int v : rowPair.second) {
                colVals.push_back(v);
            }
        }
        res.push_back(colVals);
    }
    return res;
}

int main() {
    // Test 1: [3,9,20,null,null,15,7]
    TreeNode* t1 = new TreeNode(3);
    t1->left = new TreeNode(9);
    t1->right = new TreeNode(20);
    t1->right->left = new TreeNode(15);
    t1->right->right = new TreeNode(7);

    vector<vector<int>> r1 = verticalTraversal(t1);
    cout << "Test1: ";
    for (auto& col : r1) {
        cout << "[";
        for (int v : col) cout << v << ",";
        cout << "] ";
    }
    cout << endl;
    // Expected: [[9],[3,15],[20],[7]]

    // Test 2: [1,2,3,4,5,6,7]
    TreeNode* t2 = new TreeNode(1);
    t2->left = new TreeNode(2);
    t2->right = new TreeNode(3);
    t2->left->left = new TreeNode(4);
    t2->left->right = new TreeNode(5);
    t2->right->left = new TreeNode(6);
    t2->right->right = new TreeNode(7);

    vector<vector<int>> r2 = verticalTraversal(t2);
    cout << "Test2: ";
    for (auto& col : r2) {
        cout << "[";
        for (int v : col) cout << v << ",";
        cout << "] ";
    }
    cout << endl;
    // Expected: [[4],[2],[1,5,6],[3],[7]]

    return 0;
}
