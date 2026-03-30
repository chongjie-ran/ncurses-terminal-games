// LC146 LRU Cache
// Design: HashMap + Doubly Linked List
// O(1) get and put using unordered_map + list
// 2026-03-30 23:40

#include <list>
#include <unordered_map>
#include <cassert>
#include <iostream>
using namespace std;

class LRUCache {
public:
    LRUCache(int capacity) : cap(capacity) {}

    int get(int key) {
        auto it = mp.find(key);
        if (it == mp.end()) return -1;
        // Move to front (most recently used)
        lst.splice(lst.begin(), lst, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = mp.find(key);
        if (it != mp.end()) {
            it->second->second = value;
            lst.splice(lst.begin(), lst, it->second);
        } else {
            if ((int)lst.size() == cap) {
                // Evict LRU (back of list)
                auto last = lst.back();
                mp.erase(last.first);
                lst.pop_back();
            }
            lst.emplace_front(key, value);
            mp[key] = lst.begin();
        }
    }

private:
    int cap;
    list<pair<int,int>> lst; // (key, value), front = MRU, back = LRU
    unordered_map<int, list<pair<int,int>>::iterator> mp;
};

int main() {
    LRUCache c(2);
    c.put(1,1);
    c.put(2,2);
    assert(c.get(1)==1);  // returns 1
    c.put(3,3);            // evicts key 2
    assert(c.get(2)==-1); // returns -1 (not found)
    c.put(4,4);            // evicts key 1
    assert(c.get(1)==-1);  // returns -1
    assert(c.get(3)==3);   // returns 3
    assert(c.get(4)==4);  // returns 4
    cout << "LC146 LRU Cache: All tests passed!" << endl;

    // Test 2: capacity 1
    LRUCache c2(1);
    c2.put(2,1);
    assert(c2.get(2)==1);
    c2.put(3,2);
    assert(c2.get(2)==-1);
    assert(c2.get(3)==2);
    cout << "Test 2 passed!" << endl;

    cout << "✅ All tests passed!" << endl;
    return 0;
}
