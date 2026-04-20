#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;
    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b){
        return a[1] < b[1];
    });
    int cnt = 0;
    int end = intervals[0][1];
    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] < end) {
            cnt++;
        } else {
            end = intervals[i][1];
        }
    }
    return cnt;
}

int main() {
    vector<vector<int>> t1 = {{1,2},{2,3},{3,4},{1,3}};
    cout << "Test1: " << eraseOverlapIntervals(t1) << " (expected 1)" << (eraseOverlapIntervals(t1)==1?" PASS":" FAIL") << endl;

    vector<vector<int>> t2 = {{1,2},{1,2},{1,2}};
    cout << "Test2: " << eraseOverlapIntervals(t2) << " (expected 2)" << (eraseOverlapIntervals(t2)==2?" PASS":" FAIL") << endl;

    vector<vector<int>> t3 = {{1,2},{2,3}};
    cout << "Test3: " << eraseOverlapIntervals(t3) << " (expected 0)" << (eraseOverlapIntervals(t3)==0?" PASS":" FAIL") << endl;

    vector<vector<int>> t4 = {};
    cout << "Test4: " << eraseOverlapIntervals(t4) << " (expected 0)" << (eraseOverlapIntervals(t4)==0?" PASS":" FAIL") << endl;
    return 0;
}