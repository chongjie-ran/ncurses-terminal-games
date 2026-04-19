#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool canJump(vector<int>& nums) {
    int farthest = 0;
    for (int i = 0; i < nums.size(); i++) {
        if (i > farthest) return false;
        farthest = max(farthest, i + nums[i]);
    }
    return true;
}
int main() {
    vector<int> t1 = {2,3,1,1,4};
    cout << canJump(t1) << endl; // 1
    vector<int> t2 = {3,2,1,0,4};
    cout << canJump(t2) << endl; // 0
    vector<int> t3 = {0};
    cout << canJump(t3) << endl; // 1
}
