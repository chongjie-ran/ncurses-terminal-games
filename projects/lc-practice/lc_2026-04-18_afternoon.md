# LC练习 - 2026-04-18 下午

## 贪心算法专题

### LC455 分发饼干 - Easy 贪心
**问题**: 给定胃口值g和饼干大小s，满足孩子需求的最大分配
**核心**: 排序后双指针，g和s同时推进

```cpp
int findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(), g.end());
    sort(s.begin(), s.end());
    int i = 0, j = 0, count = 0;
    while (i < g.size() && j < s.size()) {
        if (s[j] >= g[i]) { count++; i++; j++; }
        else { j++; }
    }
    return count;
}
```

**验证**: g=[1,2,3], s=[1,1]→1✓; g=[1,2], s=[1,2,3]→2✓

---

### LC435 无重叠区间 - Medium 贪心
**问题**: 移除最少区间使不重叠
**核心**: 按结束时间排序，选择结束最早的区间

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;
    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b){
        return a[1] < b[1];
    });
    int count = 0, end = intervals[0][1];
    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] < end) count++;
        else end = intervals[i][1];
    }
    return count;
}
```

**验证**: [[1,2],[2,3],[3,4],[1,3]]→1✓

---

### LC452 用最少数量的箭引爆气球 - Medium 贪心
**问题**: 区间重叠问题，一箭射穿最多气球
**核心**: 按结束点排序，贪心选择

```cpp
int findMinArrowShots(vector<vector<int>>& points) {
    if (points.empty()) return 0;
    sort(points.begin(), points.end(), [](auto& a, auto& b){
        return a[1] < b[1];
    });
    int count = 1, arrow = points[0][1];
    for (int i = 1; i < points.size(); i++) {
        if (points[i][0] > arrow) {
            count++; arrow = points[i][1];
        }
    }
    return count;
}
```

**验证**: [[10,16],[2,8],[1,6],[7,12]]→2✓

---

## 今日累计

| 题目 | 难度 | 专题 | 验证 |
|------|------|------|------|
| LC455 分发饼干 | Easy | 贪心 | ✅ |
| LC435 无重叠区间 | Medium | 贪心 | ✅ |
| LC452 箭引爆气球 | Medium | 贪心 | ✅ |

---

## 贪心算法核心要点

| 要点 | 说明 |
|------|------|
| 排序策略 | 按结束时间/开始时间排序 |
| 贪心选择 | 每步选择局部最优 |
| 证明思路 | 交换论证：最优解可调整为贪心解 |

---

*悟通LC练习 | 2026-04-18 下午*
