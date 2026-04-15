# LC练习 2026-04-16 晨间追加

## 06:35 回溯专题追加

### LC46 全排列 - 回溯
**问题**: 给定数组，返回所有可能的排列
**模式**: 
- used数组标记已使用
- path记录当前路径
- 终止条件: path.size() == nums.size()
- 选择: 遍历所有未使用的元素

```python
def permute(self, nums):
    res = []
    used = [False] * len(nums)
    def backtrack(path):
        if len(path) == len(nums):
            res.append(path[:])
            return
        for i in range(len(nums)):
            if used[i]: continue
            used[i] = True
            path.append(nums[i])
            backtrack(path)
            path.pop()
            used[i] = False
    backtrack([])
    return res
```

### LC47 全排列II - 去重技巧
**问题**: 有重复数字的全排列
**去重关键**: 
- 先排序
- `if i > 0 and nums[i] == nums[i-1] and not used[i-1]: continue`
- 同一层相同数字只选第一个

### LC40 组合总和II - 去重组合
**问题**: candidates有重复，每个数字只能用一次
**关键**: 排序后 `if i > start and nums[i] == nums[i-1]: continue`

### LC79 单词搜索 - 回溯+剪枝
**问题**: 在二维网格中是否存在单词路径
**模式**:
- 4方向DFS
- visited标记
- 剪枝: 不相等直接返回

```python
def exist(board, word):
    m, n = len(board), len(board[0])
    visited = [[False]*n for _ in range(m)]
    def dfs(i, j, idx):
        if idx == len(word): return True
        if i<0 or i>=m or j<0 or j>=n: return False
        if visited[i][j] or board[i][j] != word[idx]: return False
        visited[i][j] = True
        for di, dj in [(0,1),(0,-1),(1,0),(-1,0)]:
            if dfs(i+di, j+dj, idx+1): return True
        visited[i][j] = False
        return False
    for i in range(m):
        for j in range(n):
            if dfs(i, j, 0): return True
    return False
```

## 今日累计（06:35追加后）
| 专题 | 题数 |
|------|------|
| Floyd判圈 | 2题 |
| LFU缓存 | 1题 |
| 图论-拓扑 | 3题 |
| 回溯-全排列/组合 | 4题 |
| 单词搜索回溯 | 1题 |
| **总计** | **11题** |
