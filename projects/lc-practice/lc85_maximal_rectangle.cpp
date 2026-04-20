// LC85 Maximal Rectangle - 逐行转直方图 + 单调栈
// 核心：每一行计算 heights，然后调用 LC84 的 largestRectangleArea
#include <vector>
#include <stack>
#include <iostream>
#include <string>
using namespace std;

class Solution {
    int largestRectangleArea(vector<int>& h) {
        vector<int> heights(h.size() + 2, 0);
        for (int i = 0; i < h.size(); i++) heights[i + 1] = h[i];
        int n = heights.size();
        long long ans = 0;
        stack<int> st;
        for (int i = 0; i < n; i++) {
            while (!st.empty() && heights[i] < heights[st.top()]) {
                int height = heights[st.top()];
                st.pop();
                int width = i - st.top() - 1;
                ans = max(ans, 1LL * height * width);
            }
            st.push(i);
        }
        return (int)ans;
    }

public:
    int maximalRectangle(vector<vector<char>>& matrix) {
        if (matrix.empty()) return 0;
        int m = matrix.size(), n = matrix[0].size();
        vector<int> heights(n, 0);
        int maxArea = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                heights[j] = (matrix[i][j] == '1') ? heights[j] + 1 : 0;
            }
            maxArea = max(maxArea, largestRectangleArea(heights));
        }
        return maxArea;
    }
};

int main() {
    Solution sol;
    // Case 0: 4×5 matrix, max=6
    vector<vector<char>> m0 = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    // Case 1: all zeros
    vector<vector<char>> m1 = {{'0'}};
    // Case 2: all ones 2×2
    vector<vector<char>> m2 = {{'1','1'},{'1','1'}};
    // Case 3: single '1'
    vector<vector<char>> m3 = {{'1'}};
    // Case 4: single '0'
    vector<vector<char>> m4 = {{'0'}};
    // Case 5: 3×3 with max area=4 (row of 3 ones at bottom)
    vector<vector<char>> m5 = {
        {'0','1','0'},
        {'1','1','1'},
        {'0','1','0'}
    };

    vector<int> expects = {6, 0, 4, 1, 0, 3};

    auto result = sol.maximalRectangle(m0);
    cout << "Case 0: " << result << " (expect " << expects[0] << ") " << (result == expects[0] ? "✅" : "❌") << "\n";
    cout << "Case 1: " << sol.maximalRectangle(m1) << " (expect " << expects[1] << ") " << (sol.maximalRectangle(m1) == expects[1] ? "✅" : "❌") << "\n";
    cout << "Case 2: " << sol.maximalRectangle(m2) << " (expect " << expects[2] << ") " << (sol.maximalRectangle(m2) == expects[2] ? "✅" : "❌") << "\n";
    cout << "Case 3: " << sol.maximalRectangle(m3) << " (expect " << expects[3] << ") " << (sol.maximalRectangle(m3) == expects[3] ? "✅" : "❌") << "\n";
    cout << "Case 4: " << sol.maximalRectangle(m4) << " (expect " << expects[4] << ") " << (sol.maximalRectangle(m4) == expects[4] ? "✅" : "❌") << "\n";
    cout << "Case 5: " << sol.maximalRectangle(m5) << " (expect " << expects[5] << ") " << (sol.maximalRectangle(m5) == expects[5] ? "✅" : "❌") << "\n";

    return 0;
}
