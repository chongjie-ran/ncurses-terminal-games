#include <iostream>
#include <vector>
#include <unordered_set>
using namespace std;
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> s(nums.begin(), nums.end());
    int best = 0;
    for (int x : s) {
        if (s.find(x-1) == s.end()) {
            int y = x;
            while (s.find(y) != s.end()) y++;
            best = max(best, y - x);
        }
    }
    return best;
}
int main() {
    vector<int> t1 = {100,4,200,1,3,2};
    cout << longestConsecutive(t1) << endl;
    vector<int> t2 = {0,3,7,2,5,8,4,6,0,1};
    cout << longestConsecutive(t2) << endl;
    return 0;
}
