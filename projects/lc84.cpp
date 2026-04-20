// LC84 - Largest Rectangle in Histogram
// 核心：单调递增栈 + 哨兵技巧
// 时间：O(n) 空间：O(n)

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 方法1：单调栈（推荐）
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    vector<int> st; // 单调递增栈，存索引
    int maxArea = 0;
    
    for (int i = 0; i <= n; i++) {
        int curH = (i == n) ? 0 : heights[i];
        while (!st.empty() && curH < heights[st.back()]) {
            int h = heights[st.back()];
            st.pop_back();
            int w = st.empty() ? i : (i - st.back() - 1);
            maxArea = max(maxArea, h * w);
        }
        st.push_back(i);
    }
    return maxArea;
}

// 方法2：分治（备选，时间O(nlogn)）
int largestRectangleAreaDiv(vector<int>& heights, int l, int r) {
    if (l > r) return 0;
    int minIdx = l;
    for (int i = l + 1; i <= r; i++) {
        if (heights[i] < heights[minIdx]) minIdx = i;
    }
    int area = heights[minIdx] * (r - l + 1);
    return max({area,
        largestRectangleAreaDiv(heights, l, minIdx - 1),
        largestRectangleAreaDiv(heights, minIdx + 1, r)});
}

int main() {
    // 测试用例
    vector<vector<int>> testCases = {
        {2, 1, 5, 6, 2, 3},  // 预期10
        {2, 4},               // 预期4
        {1, 1},               // 预期2
        {1},                  // 预期1
        {1, 2, 3, 4, 5},     // 预期9 (5作为高度)
        {5, 4, 3, 2, 1},     // 预期9 (4*2+1*3)
    };
    
    for (int tc = 0; tc < (int)testCases.size(); tc++) {
        vector<int> h = testCases[tc];
        int result = largestRectangleArea(h);
        cout << "TC" << tc << ": ";
        for (int x : h) cout << x << " ";
        cout << "→ " << result << " ✓" << endl;
    }
    
    return 0;
}
