// LC739 每日温度 — 单调栈
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

vector<int> dailyTemperatures(vector<int>& T) {
    int n = T.size();
    vector<int> ans(n, 0);
    stack<int> st; // 存下标，单调递减温度
    for (int i = 0; i < n; i++) {
        while (!st.empty() && T[i] > T[st.top()]) {
            int prev = st.top(); st.pop();
            ans[prev] = i - prev;
        }
        st.push(i);
    }
    return ans;
}

int main() {
    vector<int> T = {73,74,75,71,69,72,76,73};
    auto r = dailyTemperatures(T);
    for (int x : r) cout << x << " ";
    cout << endl;
    return 0;
}
