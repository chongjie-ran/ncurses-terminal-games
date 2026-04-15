// LC437 Path Sum III - 前缀和+哈希表
#include <iostream>
#include <unordered_map>
#include <functional>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    int pathSum(TreeNode* root, int targetSum) {
        unordered_map<long, int> prefix;
        prefix[0] = 1;
        long curr = 0;
        int result = 0;
        
        function<void(TreeNode*)> dfs = [&](TreeNode* node) {
            if (!node) return;
            curr += node->val;
            result += prefix[curr - targetSum];
            prefix[curr]++;
            dfs(node->left);
            dfs(node->right);
            prefix[curr]--;
            curr -= node->val;
        };
        
        dfs(root);
        return result;
    }
};

int main() {
    Solution sol;
    
    // 测试: [1,-2,-3,1,3,-2,null,-1], target=-1
    // 路径: 1, 1->-2->1=-1, -2->1=-1, -1
    TreeNode* n[8];
    for (int i = 0; i < 8; i++) n[i] = new TreeNode(0);
    n[0]->val=1; n[1]->val=-2; n[2]->val=-3;
    n[3]->val=1; n[4]->val=3; n[5]->val=-2;
    n[6]->val=-1; n[7]->val=INT_MAX; // null placeholder
    // children: 0->1,2; 1->3,4; 2->5,6
    n[0]->left=n[1]; n[0]->right=n[2];
    n[1]->left=n[3]; n[1]->right=n[4];
    n[2]->left=n[5]; n[2]->right=n[6];
    
    int ans = sol.pathSum(n[0], -1);
    cout << "Test [1,-2,-3,1,3,-2,-1] target=-1 (期望3): " << ans << " → " << (ans==3?"✓":"✗") << endl;
    
    // 单节点 1→1
    TreeNode* r2 = new TreeNode(1);
    cout << "Test [1] target=1 (期望1): " << sol.pathSum(r2, 1) << endl;
    
    // [0,1,1], target=1 → 4
    TreeNode* n3[3];
    n3[0]=new TreeNode(0); n3[1]=new TreeNode(1); n3[2]=new TreeNode(1);
    n3[0]->left=n3[1]; n3[0]->right=n3[2];
    cout << "Test [0,1,1] target=1 (期望4): " << sol.pathSum(n3[0], 1) << endl;
    
    return 0;
}
