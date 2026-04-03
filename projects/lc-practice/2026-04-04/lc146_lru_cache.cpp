// LC146 LRU Cache
// 146. LRU Cache - Medium
// 双向链表 + HashMap 实现 O(1) LRU Cache
// 核心：最近使用的放在链表头，最久未使用的在链表尾

#include <iostream>
#include <unordered_map>
using namespace std;

// 双向链表节点
struct Node {
    int key, val;
    Node *prev, *next;
    Node(int k=0, int v=0): key(k), val(v), prev(nullptr), next(nullptr) {}
};

class LRUCache {
    int capacity;
    unordered_map<int, Node*> cache;
    Node *head, *tail; // 哑头(最近使用)、哑尾(最久未使用)
    
    // 从链表中移除节点
    void remove(Node* n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }
    
    // 插入到链表头部(最近使用)
    void insertHead(Node* n) {
        n->next = head->next;
        n->prev = head;
        head->next->prev = n;
        head->next = n;
    }
    
public:
    LRUCache(int cap): capacity(cap) {
        head = new Node(); // dummy head
        tail = new Node(); // dummy tail
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        if (cache.find(key) == cache.end()) return -1;
        Node* n = cache[key];
        remove(n);
        insertHead(n);
        return n->val;
    }
    
    void put(int key, int value) {
        if (cache.find(key) != cache.end()) {
            Node* n = cache[key];
            n->val = value;
            remove(n);
            insertHead(n);
        } else {
            if (cache.size() == capacity) {
                // 驱逐最久未使用的(链表尾的前一个)
                Node* lru = tail->prev;
                cache.erase(lru->key);
                remove(lru);
                delete lru;
            }
            Node* n = new Node(key, value);
            cache[key] = n;
            insertHead(n);
        }
    }
    
    ~LRUCache() {
        for (auto p : cache) delete p.second;
        delete head;
        delete tail;
    }
};

// 测试
int main() {
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    cout << cache.get(1) << endl; // 1
    cache.put(3, 3);              // 驱逐key=2
    cout << cache.get(2) << endl; // -1
    cache.put(4, 4);              // 驱逐key=1
    cout << cache.get(1) << endl; // -1
    cout << cache.get(3) << endl; // 3
    cout << cache.get(4) << endl; // 4
    return 0;
}
