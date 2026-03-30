/*
LC84 - Largest Rectangle in Histogram
分类: Monotonic Stack
难度: Hard
日期: 2026-03-31 01:37

思路:
  枚举每根柱子的高度，向左向右找第一个比它矮的位置，形成以该高度为高的最大矩形。
  用单调递增栈：
  - 栈存索引，遇到更低的柱子弹栈计算面积
  - 弹栈时: h = heights[stack.pop()], width = 当前索引 - stack.top() - 1
  - 最后清空栈时宽度延伸到 n

时间: O(n), 空间: O(n)
*/

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        int n = heights.size();
        vector<int> left(n), right(n);
        stack<int> st;
        
        // 第一遍：找每个位置左边第一个更矮的索引
        for (int i = 0; i < n; i++) {
            while (!st.empty() && heights[st.top()] >= heights[i]) {
                st.pop();
            }
            left[i] = st.empty() ? -1 : st.top();
            st.push(i);
        }
        
        // 清空栈
        while (!st.empty()) st.pop();
        
        // 第二遍：找每个位置右边第一个更矮的索引
        for (int i = n - 1; i >= 0; i--) {
            while (!st.empty() && heights[st.top()] >= heights[i]) {
                st.pop();
            }
            right[i] = st.empty() ? n : st.top();
            st.push(i);
        }
        
        int maxArea = 0;
        for (int i = 0; i < n; i++) {
            int width = right[i] - left[i] - 1;
            maxArea = max(maxArea, width * heights[i]);
        }
        return maxArea;
    }
    
    // 更简洁的单栈实现（一次遍历）
    int largestRectangleArea_v2(vector<int>& heights) {
        int n = heights.size();
        stack<int> st; // 存索引，单调递增
        int maxArea = 0;
        
        for (int i = 0; i <= n; i++) {
            int curHeight = (i == n) ? 0 : heights[i];
            while (!st.empty() && heights[st.top()] > curHeight) {
                int h = heights[st.top()];
                st.pop();
                int width = st.empty() ? i : (i - st.top() - 1);
                maxArea = max(maxArea, h * width);
            }
            st.push(i);
        }
        return maxArea;
    }
};

int main() {
    Solution sol;
    
    vector<vector<int>> tests = {
        {2,1,5,6,2,3},      // 10 (5*2)
        {2,4},               // 4
        {1,1},               // 2
        {1,2,2,3,1},        // 6
        {0,0,0,0},           // 0
        {1,2,3,4,5},        // 9
        {5,4,3,2,1},        // 9
    };
    
    for (int t = 0; t < tests.size(); t++) {
        int ans = sol.largestRectangleArea(tests[t]);
        int ans2 = sol.largestRectangleArea_v2(tests[t]);
        cout << "Test " << t+1 << ": " << ans << " / v2=" << ans2 
             << (ans==ans2 ? " ✅" : " ❌") << endl;
    }
    return 0;
}
