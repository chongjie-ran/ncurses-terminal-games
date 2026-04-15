// LC240: 搜索二维矩阵II
// O(m+n) BST搜索
#include <iostream>
#include <vector>
using namespace std;

bool searchMatrix(vector<vector<int>>& matrix, int target) {
    if (matrix.empty() || matrix[0].empty()) return false;
    int m = matrix.size(), n = matrix[0].size();
    int i = 0, j = n - 1; // 右上角开始
    while (i < m && j >= 0) {
        if (matrix[i][j] == target) return true;
        else if (matrix[i][j] > target) j--; // 左移
        else i++; // 下移
    }
    return false;
}

int main() {
    vector<vector<int>> m = {{1,4,7,11,15},{2,5,8,12,19},{3,6,9,16,22},{10,13,14,17,24},{18,21,23,26,30}};
    cout << searchMatrix(m, 5) << endl;  // 1
    cout << searchMatrix(m, 20) << endl; // 0
    return 0;
}
