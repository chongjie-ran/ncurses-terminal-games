#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BIT {
    vector<int> bit;
    int n;
    int _sum(int i) const {
        int r = 0;
        for (; i > 0; i -= i & -i) r += bit[i];
        return r;
    }
public:
    BIT(int n): n(n), bit(n + 1, 0) {}
    void add(int i, int delta) { for (; i <= n; i += i & -i) bit[i] += delta; }
    int sum(int i) const { return _sum(i); }
};

vector<int> countSmaller(vector<int>& nums) {
    vector<int> sorted = nums;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    int m = sorted.size();
    BIT bit(m);
    vector<int> res(nums.size());
    for (int i = nums.size() - 1; i >= 0; --i) {
        int idx = lower_bound(sorted.begin(), sorted.end(), nums[i]) - sorted.begin() + 1;
        res[i] = bit.sum(idx - 1);
        bit.add(idx, 1);
    }
    return res;
}

int main() {
    auto test = [](vector<int> nums, vector<int> expected) {
        vector<int> res = countSmaller(nums);
        if (res != expected) {
            cerr << "FAIL: expected [";
            for (auto x : expected) cerr << x << " ";
            cerr << "] got [";
            for (auto x : res) cerr << x << " ";
            cerr << "]" << endl;
        } else {
            cout << "PASS: [";
            for (auto x : res) cout << x << " ";
            cout << "]" << endl;
        }
    };
    test({5,2,6,1}, {2,1,1,0});
    test({1}, {0});
    test({1,2,3,4}, {0,0,0,0});
    test({-1,-1}, {0,0});
    test({2,0,1}, {2,0,0});
    cout << "Done!" << endl;
    return 0;
}
