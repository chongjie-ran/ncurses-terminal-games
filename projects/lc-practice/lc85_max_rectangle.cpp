/*
LC85 - Maximal Rectangle
分类: 单调栈 / DP
难度: Hard
日期: 2026-03-31 01:40

思路:
  将二维矩阵逐行处理，每一行以上的所有列形成一个直方图 heights[j]。
  heights[j] = 连续1的当前高度。
  每行用 LC84 的单调栈求最大矩形。
  转化: 2D → 多行 LC84

时间: O(m*n), 空间: O(n)
*/

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

class Solution {
public:
    // 复用 LC84 的单调栈解法
    int largestRectangleInHistogram(vector<int>& heights) {
        int n = heights.size();
        stack<int> st;
        int maxArea = 0;
        for (int i = 0; i <= n; i++) {
            int curH = (i == n) ? 0 : heights[i];
            while (!st.empty() && heights[st.top()] > curH) {
                int h = heights[st.top()];
                st.pop();
                int width = st.empty() ? i : (i - st.top() - 1);
                maxArea = max(maxArea, h * width);
            }
            st.push(i);
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
                if (matrix[i][j] == '1') {
                    heights[j] += 1;
                } else {
                    heights[j] = 0;
                }
            }
            maxArea = max(maxArea, largestRectangleInHistogram(heights));
        }
        return maxArea;
    }
    
    // DP 解法（直接用整数矩阵）
    int maximalRectangleDP(vector<vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return 0;
        int m = matrix.size(), n = matrix[0].size();
        vector<int> heights(n, 0);
        int maxArea = 0;
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (matrix[i][j] == 1) heights[j] += 1;
                else heights[j] = 0;
            }
            maxArea = max(maxArea, largestRectangleInHistogram(heights));
        }
        return maxArea;
    }
};

int main() {
    Solution sol;
    
    // Test 1: 最大矩形为 6 (2行3列)
    vector<vector<char>> t1 = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    
    // Test 2: 全1
    vector<vector<char>> t2 = {
        {'1','1'},
        {'1','1'}
    };
    
    // Test 3: 无1
    vector<vector<char>> t3 = {
        {'0','0'},
        {'0','0'}
    };
    
    // Test 4: 单行
    vector<vector<char>> t4 = {
        {'1','0','1','1','1'}
    };
    
    cout << "Test 1 (expect 6): " << sol.maximalRectangle(t1) << endl;
    cout << "Test 2 (expect 4): " << sol.maximalRectangle(t2) << endl;
    cout << "Test 3 (expect 0): " << sol.maximalRectangle(t3) << endl;
    cout << "Test 4 (expect 5): " << sol.maximalRectangle(t4) << endl;
    
    return 0;
}
