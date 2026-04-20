// LC455 Assign Cookies - Greedy
// 思路：对g和s排序，双指针贪心匹配
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(), g.end());
    sort(s.begin(), s.end());
    int i = 0, j = 0, cnt = 0;
    while (i < g.size() && j < s.size()) {
        if (s[j] >= g[i]) {
            cnt++; i++; j++;
        } else {
            j++;
        }
    }
    return {cnt};
}

int main() {
    vector<int> g1 = {1,2,3}, s1 = {1,1};
    auto r1 = findContentChildren(g1, s1);
    cout << "Test1: " << r1[0] << " (expected 1) " << (r1[0]==1?"✓":"✗") << endl;

    vector<int> g2 = {1,2}, s2 = {1,2,3};
    auto r2 = findContentChildren(g2, s2);
    cout << "Test2: " << r2[0] << " (expected 2) " << (r2[0]==2?"✓":"✗") << endl;

    vector<int> g3 = {1,2,3}, s3 = {3};
    auto r3 = findContentChildren(g3, s3);
    cout << "Test3: " << r3[0] << " (expected 1) " << (r3[0]==1?"✓":"✗") << endl;
    return 0;
}

