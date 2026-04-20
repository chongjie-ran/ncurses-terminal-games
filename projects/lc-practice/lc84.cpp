#include <iostream>
#include <vector>
#include <stack>
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

int main() {
    vector<int> h1 = {2,1,5,6,2,3};
    cout << "h1: " << largestRectangleArea(h1) << " (expect 10)" << endl;
    vector<int> h2 = {2,4};
    cout << "h2: " << largestRectangleArea(h2) << " (expect 4)" << endl;
    vector<int> h3 = {1};
    cout << "h3: " << largestRectangleArea(h3) << " (expect 1)" << endl;
    return 0;
}
