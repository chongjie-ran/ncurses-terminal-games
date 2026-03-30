// LC85 最大矩形 — 单调栈
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> heights(n + 1, 0);
    int best = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == '1')
                heights[j] += 1;
            else
                heights[j] = 0;
        }
        stack<int> st;
        for (int j = 0; j <= n; j++) {
            while (!st.empty() && heights[st.top()] > heights[j]) {
                int h = heights[st.top()]; st.pop();
                int left = st.empty() ? 0 : st.top() + 1;
                int right = j - 1;
                best = max(best, h * (right - left + 1));
            }
            st.push(j);
        }
    }
    return best;
}

int main() {
    vector<vector<char>> m = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << maximalRectangle(m) << endl;
    return 0;
}
