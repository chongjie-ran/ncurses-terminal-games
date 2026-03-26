// LeetCode 48 - Rotate Image
// 原地旋转 n×n 矩阵 90° 顺时针
// 思路：先转置，再镜像翻转每一行
// 时间: O(n²), 空间: O(1)

#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // 1. 转置：对角线（左上→右下）交换
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            swap(matrix[i][j], matrix[j][i]);
        }
    }
    // 2. 镜像翻转：左右对称交换
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n / 2; ++j) {
            swap(matrix[i][j], matrix[i][n - 1 - j]);
        }
    }
}

void printMatrix(const vector<vector<int>>& m) {
    for (const auto& row : m) {
        for (int v : row) cout << setw(3) << v;
        cout << "\n";
    }
}

int main() {
    vector<vector<int>> m1 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    cout << "原始 3×3:\n"; printMatrix(m1);
    rotate(m1);
    cout << "旋转90°后:\n"; printMatrix(m1);

    vector<vector<int>> m2 = {
        { 1, 2, 3, 4},
        { 5, 6, 7, 8},
        { 9,10,11,12},
        {13,14,15,16}
    };
    cout << "\n原始 4×4:\n"; printMatrix(m2);
    rotate(m2);
    cout << "旋转90°后:\n"; printMatrix(m2);
    return 0;
}
