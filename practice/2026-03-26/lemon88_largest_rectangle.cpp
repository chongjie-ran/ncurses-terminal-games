/**
 * LeetCode 84 - Largest Rectangle in Histogram
 * 分类: 单调栈 (Monotonic Stack)
 * 难度: Hard
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 *
 * 核心思路:
 * 对每个柱形，找以其高度为高的最大矩形：
 * - 左边找到第一个比它矮的柱形 (left smaller)
 * - 右边找到第一个比它矮的柱形 (right smaller)
 * - 宽度 = right_smaller_idx - left_smaller_idx - 1
 *
 * 技巧：两端加哨兵（高度0）避免边界特判
 */

#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
using namespace std;

// 方法一：单调递增栈 + 哨兵（推荐）
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    if (n == 0) return 0;

    // 扩展数组：两端加哨兵（高度0）
    vector<int> h(n + 2, 0);
    for (int i = 1; i <= n; ++i) h[i] = heights[i - 1];

    stack<int> st; // 存索引，栈内高度单调递增
    int maxArea = 0;

    for (int i = 0; i < n + 2; ++i) {
        // 当前高度小于栈顶时，栈顶柱形形成最大矩形
        while (!st.empty() && h[i] < h[st.top()]) {
            int height = h[st.top()];
            st.pop();
            int width = i - st.top() - 1; // 左边界是栈新顶，右边界是 i
            maxArea = max(maxArea, height * width);
        }
        st.push(i);
    }

    return maxArea;
}

// 方法二：分治法（更直观但 O(n log n)）
int largestRectangleArea_DC(vector<int>& heights) {
    function<int(int, int)> solve = [&](int l, int r) -> int {
        if (l > r) return 0;
        if (l == r) return heights[l];

        int mid = (l + r) / 2;
        // 递归找左右最大
        int area = max(solve(l, mid), solve(mid + 1, r));

        // 扩展法找跨越中线的最大矩形
        int i = mid, j = mid + 1;
        int h = min(heights[i], heights[j]);
        while (i >= l && j <= r) {
            h = min(h, min(heights[i], heights[j]));
            area = max(area, h * (j - i + 1));
            if (i == l) ++j;
            else if (j == r) --i;
            else if (heights[i - 1] > heights[j + 1]) --i;
            else ++j;
        }
        return area;
    };

    return solve(0, (int)heights.size() - 1);
}

// 手动模拟 [2,1,5,6,2,3], k=2
// h = [0, 2,1,5,6,2,3,0]
// i=0: st=[0], h[i]=0 < h[st.top()]=2, height=2, st.pop()->st=[], width=0-(-1)-1=0, area=0
// i=1: st=[1]
// i=2: h[2]=1 < h[1]=2, height=2, st.pop()->st=[1], width=2-1-1=0
// ... 最终得到 maxArea=10

int main() {
    vector<int> heights = {2,1,5,6,2,3};
    cout << largestRectangleArea(heights) << endl; // 10

    heights = {2,4};
    cout << largestRectangleArea(heights) << endl; // 4

    cout << "LeetCode 84 passed!" << endl;
    return 0;
}
