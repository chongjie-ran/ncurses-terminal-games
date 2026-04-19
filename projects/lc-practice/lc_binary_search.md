
## LC875 Koko Eating Bananas (2026-04-19 02:55)

**核心**: 二分答案，在 `[1, max(piles)]` 范围内找最小速度

**代码模式**:
```cpp
int minEatingSpeed(vector<int>& piles, int h) {
    int lo = 1, hi = *max_element(piles.begin(), piles.end());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (hours(piles, mid) <= h) hi = mid; // 可以吃完，尝试更小速度
        else lo = mid + 1;
    }
    return lo;
}
```

**验证**:
- [3,6,7,11], h=8 → 4 ✅
- [30,11,19,13], h=6 → 15 ✅
- [30,11,19,13], h=5 → 30 ✅

**Binary Search通用模板**:
1. lo=最小可行值, hi=最大可能值
2. `while(lo<hi)` 循环
3. `mid = lo + (hi-lo)/2` 防溢出
4. 根据条件决定左区间还是右区间
