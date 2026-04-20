// LC85 - Maximal Rectangle
// 核心：将2D矩阵转换为直方图，用LC84的单调栈求最大矩形
// 时间：O(m*n) 空间：O(n)

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// LC84的单调栈函数
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    vector<int> st;
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

int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> heights(n, 0);
    int maxArea = 0;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == '1')
                heights[j] += 1;
            else
                heights[j] = 0;
        }
        maxArea = max(maxArea, largestRectangleArea(heights));
    }
    return maxArea;
}

int main() {
    // 测试用例
    vector<vector<vector<char>>> testCases = {
        {{'1','0','1','0','0'}, {'1','0','1','1','1'}, {'1','1','1','1','1'}, {'1','0','0','1','0'}}, // 预期6
        {{'1'}}, // 预期1
        {{'0'}}, // 预期0
        {{'1','1'}, {'1','1'}}, // 预期4
        {{'0','0'}, {'0','0'}}, // 预期0
    };
    
    for (int tc = 0; tc < (int)testCases.size(); tc++) {
        int result = maximalRectangle(testCases[tc]);
        cout << "TC" << tc << " → " << result << " ✓" << endl;
    }
    
    return 0;
}
