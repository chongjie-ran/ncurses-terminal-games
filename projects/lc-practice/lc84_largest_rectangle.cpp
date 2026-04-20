// LC84 Largest Rectangle in Histogram
// 核心：单调递增栈 + 哨兵技巧（左右各加0）
// 时间O(n)，空间O(n)
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        vector<int> h(heights.size() + 2, 0);
        for (int i = 0; i < heights.size(); i++) h[i + 1] = heights[i];

        int n = h.size();
        long long maxArea = 0;
        stack<int> st;  // 存索引

        for (int i = 0; i < n; i++) {
            while (!st.empty() && h[i] < h[st.top()]) {
                int height = h[st.top()];
                st.pop();
                int width = i - st.top() - 1;
                maxArea = max(maxArea, 1LL * height * width);
            }
            st.push(i);
        }
        return (int)maxArea;
    }
};

int main() {
    Solution sol;
    vector<vector<int>> cases = {
        {2, 1, 5, 6, 2, 3},  // 10
        {2, 4},              // 4
        {1, 2, 3, 4, 5},    // 9
        {5, 4, 1, 2},       // 8
        {1, 1, 1, 1},       // 4
        {1, 2, 3, 2, 1},    // 6
        {1, 0, 1, 0, 1, 0, 1}, // 1
    };
    vector<int> expects = {10, 4, 9, 8, 4, 6, 1};

    for (int i = 0; i < cases.size(); i++) {
        int result = sol.largestRectangleArea(cases[i]);
        cout << "Case " << i << ": " << result << " (expect " << expects[i] << ") "
             << (result == expects[i] ? "✅" : "❌") << "\n";
    }
    return 0;
}
