// LC84 柱状图中最大的矩形 — 单调栈
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    vector<int> h(n + 1, 0);
    for (int i = 0; i < n; i++) h[i] = heights[i];
    stack<int> st;
    int best = 0;
    for (int i = 0; i <= n; i++) {
        while (!st.empty() && h[st.top()] > h[i]) {
            int height = h[st.top()]; st.pop();
            int left = st.empty() ? 0 : st.top() + 1;
            int right = i - 1;
            best = max(best, height * (right - left + 1));
        }
        st.push(i);
    }
    return best;
}

int main() {
    vector<int> h = {2,1,5,6,2,3};
    cout << largestRectangleArea(h) << endl;
    return 0;
}
