// LC85: Maximal Rectangle - convert rows to histogram + LC84
#include <iostream>
#include <vector>
using namespace std;

int largestRectangleArea(vector<int>& h) {
    int n = h.size(), ans = 0;
    vector<int> st;
    for (int i = 0; i <= n; i++) {
        int cur = (i == n ? 0 : h[i]);
        while (!st.empty() && h[st.back()] > cur) {
            int height = h[st.back()]; st.pop_back();
            int width = st.empty() ? i : (i - st.back() - 1);
            ans = max(ans, height * width);
        }
        st.push_back(i == n ? i : i);
    }
    return ans;
}

int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    int m = matrix.size(), n = matrix[0].size(), ans = 0;
    vector<int> heights(n, 0);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            heights[j] = (matrix[i][j] == '1') ? heights[j] + 1 : 0;
        }
        ans = max(ans, largestRectangleArea(heights));
    }
    return ans;
}

int main() {
    vector<vector<char>> m1 = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << "m1: " << maximalRectangle(m1) << " (expect 6)" << endl;
    vector<vector<char>> m2 = {{'0'}};
    cout << "m2: " << maximalRectangle(m2) << " (expect 0)" << endl;
    vector<vector<char>> m3 = {{'1'}};
    cout << "m3: " << maximalRectangle(m3) << " (expect 1)" << endl;
    return 0;
}
