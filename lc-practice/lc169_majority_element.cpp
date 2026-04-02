// LC169 Majority Element
// Boyer-Moore Voting Algorithm - O(n) time, O(1) space
#include <iostream>
#include <vector>
using namespace std;

int majorityElement(vector<int>& nums) {
    int candidate = 0, count = 0;
    for (int n : nums) {
        if (count == 0) {
            candidate = n;
            count = 1;
        } else {
            count += (n == candidate) ? 1 : -1;
        }
    }
    return candidate;
}

int main() {
    vector<int> t1 = {3,2,3};
    cout << "Test1: " << majorityElement(t1) << " (expected 3)" << endl;
    
    vector<int> t2 = {2,2,1,1,1,2,2};
    cout << "Test2: " << majorityElement(t2) << " (expected 2)" << endl;
    
    vector<int> t3 = {1};
    cout << "Test3: " << majorityElement(t3) << " (expected 1)" << endl;
    
    vector<int> t4 = {6,5,5};
    cout << "Test4: " << majorityElement(t4) << " (expected 5)" << endl;
    
    return 0;
}
