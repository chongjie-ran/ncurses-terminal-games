// LC240 搜索二维矩阵II
// 思路: 从右上角开始，左边都是小，下边都是大，类似BST搜索
// 时间O(m+n) 空间O(1)
#include <iostream>
#include <vector>
using namespace std;
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int r = 0, c = n-1;
    while (r < m && c >= 0) {
        if (matrix[r][c] == target) return true;
        else if (matrix[r][c] > target) c--;
        else r++;
    }
    return false;
}
int main() {
    vector<vector<int>> m = {{1,4,7,11,15},{2,5,8,12,19},{3,6,9,16,22},{10,13,14,17,24},{18,21,23,26,30}};
    cout << searchMatrix(m, 5) << endl;  // true
    cout << searchMatrix(m, 20) << endl; // false
    return 0;
}
