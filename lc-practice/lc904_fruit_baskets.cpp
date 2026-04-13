// LC904 - Fruit Into Baskets
// Sliding Window: 找最多包含2种水果的连续子数组长度
// O(n) time, O(1) space
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// 翻译: 找包含最多2种石子（用数字表示）的最长子数组
int totalFruit(vector<int>& fruits) {
    unordered_map<int, int> cnt;
    int left = 0, maxLen = 0;
    
    for (int right = 0; right < fruits.size(); right++) {
        cnt[fruits[right]]++;
        
        // 收缩左指针直到只有2种水果
        while (cnt.size() > 2) {
            cnt[fruits[left]]--;
            if (cnt[fruits[left]] == 0) cnt.erase(fruits[left]);
            left++;
        }
        
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

int main() {
    vector<int> t1 = {1,2,1};
    cout << "Test1: " << totalFruit(t1) << " (expected 3)" << endl;
    
    vector<int> t2 = {0,1,2,2};
    cout << "Test2: " << totalFruit(t2) << " (expected 3)" << endl;
    
    vector<int> t3 = {1,2,3,2,2};
    cout << "Test3: " << totalFruit(t3) << " (expected 4)" << endl;
    
    vector<int> t4 = {3,3,3,1,2,1,1,2,3,3,4};
    cout << "Test4: " << totalFruit(t4) << " (expected 5)" << endl;
    
    vector<int> t5 = {1,0,1,4,1,4,1,2,3};
    cout << "Test5: " << totalFruit(t5) << " (expected 5)" << endl;
    
    return 0;
}
