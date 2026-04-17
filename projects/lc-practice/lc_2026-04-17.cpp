// LC practice 2026-04-17 - 周五专研
// 专题：单调栈(LC84/85) + BST(LC98/230) 复习
// 重点：单调递减栈处理柱状图， BST验证与查找
#include <bits/stdc++.h>
using namespace std;

// ===== LC84: Largest Rectangle in Histogram =====
// 难度: Hard
// 分类: 单调栈
// 核心: 单调递增栈 + 哨兵0 + 弹出时计算面积
// 模板: while h[st.top] > cur: pop+计算; push i
class Solution84 {
public:
    int largestRectangleArea(vector<int>& h) {
        stack<int> st; // 存索引，单调递增
        int n = h.size(), ans = 0;
        for (int i = 0; i <= n; i++) {
            int cur = (i == n) ? 0 : h[i]; // 哨兵
            while (!st.empty() && h[st.top()] > cur) {
                int height = h[st.top()]; st.pop();
                int width = st.empty() ? i : (i - st.top() - 1);
                ans = max(ans, height * width);
            }
            st.push(i);
        }
        return ans;
        // 时间O(n)，空间O(n)
        // 弹出时: height是当前bar高度，左边界=st.top+1，右边界=i-1
    }
};

// ===== LC85: Maximal Rectangle =====
// 难度: Hard
// 分类: 单调栈 + DP
// 核心: 把每行当成柱状图底部，累加高度，复用LC84
class Solution85 {
public:
    int maximalRectangle(vector<vector<char>>& M) {
        if (M.empty()) return 0;
        int m = M.size(), n = M[0].size();
        vector<int> h(n, 0);
        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                h[j] = (M[i][j]=='1') ? h[j]+1 : 0;
            }
            ans = max(ans, largestRect(h));
        }
        return ans;
    }
private:
    int largestRect(vector<int>& h) {
        stack<int> st;
        int n = h.size(), ans = 0;
        for (int i = 0; i <= n; i++) {
            int cur = (i==n)?0:h[i];
            while (!st.empty() && h[st.top()] > cur) {
                int height = h[st.top()]; st.pop();
                int width = st.empty() ? i : (i - st.top() - 1);
                ans = max(ans, height * width);
            }
            st.push(i);
        }
        return ans;
    }
};

// ===== LC98: Validate Binary Search Tree =====
// 难度: Medium
// 分类: BST + 递归
// 核心: 传上下界，左子树界(lo,根)，右子树界(根,hi)
class Solution98 {
public:
    bool isValidBST(TreeNode* root) {
        return dfs(root, LONG_MIN, LONG_MAX);
    }
private:
    bool dfs(TreeNode* n, long lo, long hi) {
        if (!n) return true;
        if (n->val <= lo || n->val >= hi) return false;
        return dfs(n->left, lo, n->val) && dfs(n->right, n->val, hi);
    }
};

// ===== LC230: Kth Smallest in BST =====
// 难度: Medium
// 分类: BST + 中序遍历
// 核心: BST中序是升序，第k个访问的就是kth最小
class Solution230 {
public:
    int kthSmallest(TreeNode* root, int k) {
        stack<TreeNode*> st;
        TreeNode* cur = root;
        while (cur || !st.empty()) {
            while (cur) { st.push(cur); cur = cur->left; }
            cur = st.top(); st.pop();
            if (--k == 0) return cur->val;
            cur = cur->right;
        }
        return -1;
    }
};

// 验证编译
int main() {
    cout << "LC84/85/98/230 模板已就绪" << endl;
    return 0;
}
