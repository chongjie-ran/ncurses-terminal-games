// LC218 The Skyline Problem — Hard
// Sweep line with max-heap + cleanup
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
using namespace std;

vector<vector<int>> getSkyline(vector<vector<int>>& buildings) {
    // events: (x, height, isStart)
    vector<tuple<int,int,int>> events;
    for (auto& b : buildings) {
        events.emplace_back(b[0], b[2], 1);  // start
        events.emplace_back(b[1], b[2], 0);  // end
    }
    sort(events.begin(), events.end(), [](const auto& a, const auto& b) {
        int x1 = get<0>(a), x2 = get<0>(b);
        int s1 = get<2>(a), s2 = get<2>(b);
        if (x1 != x2) return x1 < x2;
        if (s1 != s2) return s1 > s2; // start before end
        return false;
    });

    vector<vector<int>> result;
    // max-heap: (height, end_x)
    // use map as "sorted multiset" with count: height -> count
    map<int,int> active; // height -> count (negative means removal pending)
    active[0] = 1;
    int prevMax = 0;
    size_t i = 0;

    while (i < events.size()) {
        int x = get<0>(events[i]);

        // Process all events at this x
        size_t j = i;
        while (j < events.size() && get<0>(events[j]) == x) ++j;

        // Apply events
        for (size_t k = i; k < j; ++k) {
            int h = get<1>(events[k]);
            if (get<2>(events[k]) == 1) { // start
                active[h]++;
            } else { // end
                active[h]--;
                if (active[h] == 0) active.erase(h);
            }
        }

        int currMax = active.rbegin()->first;
        if (currMax != prevMax) {
            result.push_back({x, currMax});
            prevMax = currMax;
        }
        i = j;
    }
    return result;
}

void printSkyline(const vector<vector<int>>& sky) {
    cout << "[";
    for (auto& p : sky) cout << "(" << p[0] << "," << p[1] << ")";
    cout << "]" << endl;
}

int main() {
    vector<vector<int>> buildings = {{2,9,10},{3,7,15},{5,12,12},{15,20,10},{19,24,8}};
    printSkyline(getSkyline(buildings));
    // Expected: (2,10)(3,15)(7,12)(12,0)(15,10)(20,8)(24,0)

    buildings = {{1,2,1},{1,2,2},{1,2,3}};
    printSkyline(getSkyline(buildings));
    // Expected: (1,3)(2,0)

    buildings = {{0,2,3},{2,5,3}};
    printSkyline(getSkyline(buildings));
    // Expected: (0,3)(5,0)

    buildings = {};
    printSkyline(getSkyline(buildings));

    cout << "Done!" << endl;
    return 0;
}
