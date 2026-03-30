#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;
    int m = matrix.size(), n = matrix[0].size();
    vector<int> left(n, 0), right(n, n), height(n, 0);
    int best = 0;
    for (int i = 0; i < m; ++i) {
        int cur_left = 0, cur_right = n;
        for (int j = 0; j < n; ++j)
            height[j] = (matrix[i][j]=='1') ? height[j]+1 : 0;
        for (int j = 0; j < n; ++j)
            if (matrix[i][j]=='1') left[j]=max(left[j],cur_left);
            else { left[j]=0; cur_left=j+1; }
        for (int j = n-1; j >=0; --j)
            if (matrix[i][j]=='1') right[j]=min(right[j],cur_right);
            else { right[j]=n; cur_right=j; }
        for (int j = 0; j < n; ++j)
            best = max(best, (right[j]-left[j])*height[j]);
    }
    return best;
}

int main() {
    vector<vector<char>> m1 = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    cout << maximalRectangle(m1) << endl; // 6
    vector<vector<char>> m2 = {{'0'}};
    cout << maximalRectangle(m2) << endl; // 0
    vector<vector<char>> m3 = {{'1'}};
    cout << maximalRectangle(m3) << endl; // 1
    return 0;
}
