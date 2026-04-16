# LC练习 2026-04-16 下午

## 14:00 树形DP专题

### LC124 二叉树中的最大路径和
**问题**: 二叉树中找最大路径和（路径：任意节点连接）
**核心**: 后序遍历，每个节点返回"单臂最大贡献"；全局max更新所有情况

**代码模式**:
```cpp
int maxPathSum(TreeNode* root, int& maxAns) {
    if (!root) return 0;
    int left = max(0, maxPathSum(root->left, maxAns));
    int right = max(0, maxPathSum(root->right, maxAns));
    maxAns = max(maxAns, root->val + left + right);
    return root->val + max(left, right);
}
```

### LC337 打家劫舍III - 二叉树版
**问题**: 树形结构，相邻节点不能同时偷，返回最大值
**核心**: 每个节点返回(不偷最大值, 偷最大值)，后序合并

**代码模式**:
```cpp
pair<int,int> rob(TreeNode* u) {
    if (!u) return {0,0};
    auto [l0,l1] = rob(u->left);
    auto [r0,r1] = rob(u->right);
    // 不偷：左右子树各取最大值
    // 偷：当前值 + 左右不偷
    return {max(l0,l1)+max(r0,r1), u->val+l0+r0};
}
```

## 16:00 回溯算法专题

### LC78 子集 - 无重复元素
**问题**: 返回数组所有子集（升序）
**模式**: 标准回溯，每个位置选或不选

```cpp
void backtrack(vector<int>& nums, int start, vector<int>& path, vector<vector<int>>& res) {
    res.push_back(path);
    for (int i=start; i<nums.size(); i++) {
        path.push_back(nums[i]);
        backtrack(nums, i+1, path, res);
        path.pop_back();
    }
}
```

### LC90 子集II - 有重复元素需去重
**问题**: 数组有重复，返回不重复子集
**模式**: 先排序，DFS时 `i>start && nums[i]==nums[i-1]` 跳过

### LC47 全排列II - 有重复数字的全排列
**问题**: 有重复数字，返回所有不重复排列
**模式**: 排序 + used[]标记 + `i>0 && nums[i]==nums[i-1] && !used[i-1]` 跳过

### LC39 组合总和 - 可重复选元素
**问题**: candidates中元素可重复选，和为target
**模式**: 排序后DFS，`i`从start开始（可重复），不回头

### LC40 组合总和II - 每个元素只能用一次
**问题**: candidates每个元素只能用一次
**模式**: 排序后DFS，`i`从start开始，跳过重复 `i>start && nums[i]==nums[i-1]`

## 18:00 字符串算法专题

### LC5 最长回文子串
**问题**: 找最长回文子串
**中心扩展法 O(n²)**:
```cpp
string longestPalindrome(string s) {
    int n=s.size(), best=0; string ans;
    for (int c=0; c<n; c++) {
        // 奇数
        for (int l=c,r=c; l>=0&&r<n&&s[l]==s[r]; l--,r++) {
            if (r-l+1>best) best=r-l+1, ans=s.substr(l,r-l+1);
        }
        // 偶数
        for (int l=c,r=c+1; l>=0&&r<n&&s[l]==s[r]; l--,r++) {
            if (r-l+1>best) best=r-l+1, ans=s.substr(l,r-l+1);
        }
    }
    return ans;
}
```

### LC647 回文子串
**问题**: 统计回文子串数量
**模式**: 中心扩展，每个中心扩展计数

## 20:00 区间DP专题

### LC516 最长回文子序列
**问题**: 不要求连续，返回最长回文子序列长度
**区间DP**: dp[i][j] = dp[i+1][j-1]+2 (s[i]==s[j]) : max(dp[i+1][j], dp[i][j-1])

### LC131 分割回文串
**问题**: 字符串分割，使每段都是回文，返回所有方案
**模式**: DFS+中心扩展判断回文

## 04-16 累计
| 专题 | 题数 |
|------|------|
| Floyd判圈 | 2 |
| LFU缓存 | 1 |
| 图论-拓扑 | 3 |
| 岛屿BFS | 2 |
| 树形DP | 2 |
| 回溯算法 | 5 |
| 字符串 | 2 |
| 区间DP | 2 |
| **总计** | **19题** |

## 技术收获
- 树形DP：后序遍历+返回值设计
- 回溯：去重三要素（排序+used/skip条件）
- 区间DP：外层倒序遍历i，内层顺序遍历j
- 中心扩展：奇偶两种情况同时处理
