# LC练习 2026-04-16 下午

## 09:30 动态规划专题

### LC70 爬楼梯
**问题**: n阶台阶，每次1或2步，几种走法？
**模式**: 斐波那契，dp[i]=dp[i-1]+dp[i-2]，空间O(1)
```cpp
int climbStairs(int n) {
    if (n <= 2) return n;
    int dp1=1, dp2=2;
    for (int i=3; i<=n; i++) { int dp3=dp1+dp2; dp1=dp2; dp2=dp3; }
    return dp2;
}
```

### LC322 零钱兑换
**问题**: 最少硬币数凑成amount
**模式**: 完全背包，dp初始化INF，dp[0]=0
```cpp
vector<int> dp(amount+1, INF); dp[0]=0;
for (int a=1; a<=amount; a++)
    for (int c: coins)
        if (a>=c) dp[a]=min(dp[a], dp[a-c]+1);
```

### LC300 最长递增子序列
**问题**: 非连续最长上升子序列长度
**模式**: O(n²) DP，外层i遍历，内层j< i时更新
```cpp
for (int i=0; i<n; i++)
    for (int j=0; j<i; j++)
        if (nums[j]<nums[i]) dp[i]=max(dp[i], dp[j]+1);
```

### LC64 最小路径和
**问题**: 网格左上到右下路径最小和（只能右/下移动）
**模式**: 2D DP，第一行/列累加，其余取min
```cpp
dp[i][j] = min(dp[i-1][j], dp[i][j-1]) + grid[i][j];
```

### 验证结果
| 题目 | 输入 | 输出 | 期望 |
|------|------|------|------|
| LC70 | n=5 | 8 | ✓ |
| LC322 | [1,2,5], 11 | 3 | ✓ |
| LC322 | [2], 3 | -1 | ✓ |
| LC300 | [10,9,2,5,3,7,101,18] | 4 | ✓ |
| LC64 | [[1,3,1],[1,5,1],[4,2,1]] | 7 | ✓ |

### 04-16 累计
| 专题 | 题数 |
|------|------|
| Floyd判圈 | 2题 |
| LFU缓存 | 1题 |
| 图论-拓扑 | 3题 |
| 回溯-组合 | 4题 |
| BFS/DFS岛屿 | 2题 |
| 位运算 | 7题 |
| DP动态规划 | 4题 |
| **总计** | **23题** |

---

## 10:09 单调栈专题

### 核心模式
**Next Greater Element**: 维护递减栈，找右边第一个比当前大的元素
```cpp
vector<int> nextGreaterElement(vector<int>& nums) {
    vector<int> res(nums.size(), -1);
    stack<int> st; // 存索引，递减栈
    for (int i=0; i<nums.size(); i++) {
        while (!st.empty() && nums[i] > nums[st.top()]) {
            res[st.top()] = nums[i]; st.pop();
        }
        st.push(i);
    }
    return res;
}
```

### LC496 下一个更大元素 I
```cpp
vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    vector<int> nge = nextGreaterElement(nums2);
    unordered_map<int,int> mp;
    for (int i=0; i<nums2.size(); i++) mp[nums2[i]] = nge[i];
    vector<int> res;
    for (int x: nums1) res.push_back(mp[x]);
    return res;
}
```

### LC503 下一个更大元素 II（环形）
```cpp
vector<int> nextGreaterElements(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;
    for (int i=0; i<2*n; i++) {
        while (!st.empty() && nums[i%n] > nums[st.top()]) {
            res[st.top()] = nums[i%n]; st.pop();
        }
        if (i < n) st.push(i);
    }
    return res;
}
```

### LC739 每日温度
```cpp
vector<int> dailyTemperatures(vector<int>& t) {
    vector<int> res(t.size(), 0);
    stack<int> st;
    for (int i=0; i<t.size(); i++) {
        while (!st.empty() && t[i] > t[st.top()]) {
            res[st.top()] = i - st.top(); st.pop();
        }
        st.push(i);
    }
    return res;
}
```

### LC42 接雨水（双指针）
```cpp
int trap(vector<int>& h) {
    int l=0, r=h.size()-1, lmax=0, rmax=0, water=0;
    while(l<r) {
        if(h[l]<=h[r]) {
            if(h[l]>=lmax) lmax=h[l]; else water+=lmax-h[l];
            l++;
        } else {
            if(h[r]>=rmax) rmax=h[r]; else water+=rmax-h[r];
            r--;
        }
    }
    return water;
}
```

### 验证结果
| 题目 | 输入 | 输出 | 期望 |
|------|------|------|------|
| LC496 | [4,1,2],[1,3,4,2] | [-1,3,-1] | ✓ |
| LC503 | [1,2,1] | [2,-1,2] | ✓ |
| LC739 | [73,74,75,71,69,72,76,73] | [1,1,4,2,1,1,0,0] | ✓ |
| LC42 | [0,1,0,2,1,0,1,3,2,1,2,1] | 6 | ✓ |

### 04-16 累计（含本次单调栈）
| 专题 | 题数 |
|------|------|
| Floyd判圈 | 2题 |
| LFU缓存 | 1题 |
| 图论-拓扑 | 3题 |
| 回溯-组合 | 4题 |
| BFS/DFS岛屿 | 2题 |
| 位运算 | 7题 |
| DP动态规划 | 4题 |
| 单调栈 | 4题 |
| **总计** | **27题** |

