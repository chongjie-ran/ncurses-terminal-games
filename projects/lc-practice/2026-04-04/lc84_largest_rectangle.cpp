// LC84. Largest Rectangle in Histogram
// Hard - Monotonic Stack
// O(n) time, O(n) space using stack
// 核心: 维护递增栈,遇到更小的bar时弹出计算矩形高度

#include <iostream>
#include <vector>
#include <stack>
using namespace std;

// 解法1: Monotonic Stack (推荐)
// 思想: 对于每个bar,找到左右最近比它小的bar,之间的宽度*高度即为以该bar为高的最大矩形
// 关键技巧: 在heights末尾加0,强制弹出所有剩余bar
int largestRectangleArea(vector<int>& heights) {
    stack<int> st; // 存下标,栈内高度递增
    int maxArea = 0;
    for (int i = 0; i <= heights.size(); i++) {
        int cur = (i == heights.size()) ? 0 : heights[i];
        while (!st.empty() && heights[st.top()] > cur) {
            int h = heights[st.top()];
            st.pop();
            int left = st.empty() ? 0 : st.top() + 1;
            int width = st.empty() ? i : i - st.top() - 1;
            maxArea = max(maxArea, h * width);
        }
        st.push(i);
    }
    return maxArea;
}

// 解法2: 分治 (慢但有趣)
// 找到最低高度的bar,分割左右递归
int largestRectangleAreaDivide(vector<int>& heights, int left, int right) {
    if (left > right) return 0;
    int minIdx = left;
    for (int i = left; i <= right; i++) {
        if (heights[i] < heights[minIdx]) minIdx = i;
    }
    int area = heights[minIdx] * (right - left + 1);
    int leftArea = largestRectangleAreaDivide(heights, left, minIdx - 1);
    int rightArea = largestRectangleAreaDivide(heights, minIdx + 1, right);
    return max(area, max(leftArea, rightArea));
}

int main() {
    vector<int> h1 = {2,1,5,6,2,3};
    cout << largestRectangleArea(h1) << endl; // 10 (5*2)
    
    vector<int> h2 = {2,4};
    cout << largestRectangleArea(h2) << endl; // 4
    
    vector<int> h3 = {};
    cout << largestRectangleArea(h3) << endl; // 0
    
    return 0;
}

/*
核心模式:
1. Monotonic Stack遍历数组,维护递增下标栈
2. 当遇到更小的bar时,栈顶弹出,计算以该bar为高的矩形
3. 弹出后的新栈顶左边界 + 当前i右边界决定宽度
4. 末尾加0确保所有bar都能弹出计算

复杂度: O(n) time, O(n) space
为什么正确: 每个bar只入栈出栈各一次
*/
