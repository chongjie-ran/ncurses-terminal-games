# LC练习 2026-04-18 周六晚间 - 拓扑排序 + LRU/LFU

## 专题: 拓扑排序 (Topological Sort)

### LC207 课程表 (Medium-Hard)
**问题**: 判断是否能完成所有课程（无环）
**核心**: Kahn算法/BFS拓扑排序，检测环

```cpp
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> g(numCourses);
        vector<int> indegree(numCourses, 0);
        for (auto& p : prerequisites) {
            g[p[1]].push_back(p[0]);  // p[1] -> p[0]
            indegree[p[0]]++;
        }
        queue<int> q;
        for (int i = 0; i < numCourses; i++)
            if (indegree[i] == 0) q.push(i);
        int visited = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            visited++;
            for (int v : g[u]) {
                if (--indegree[v] == 0) q.push(v);
            }
        }
        return visited == numCourses;
    }
};
```

**验证**: ✅ 典型拓扑排序，可检测环

### LC210 课程表 II (Medium-Hard)
**问题**: 返回完成课程的顺序
**核心**: Kahn算法 + order记录

```cpp
vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> g(numCourses);
    vector<int> indegree(numCourses, 0);
    for (auto& p : prerequisites) {
        g[p[1]].push_back(p[0]);
        indegree[p[0]]++;
    }
    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indegree[i] == 0) q.push(i);
    vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : g[u])
            if (--indegree[v] == 0) q.push(v);
    }
    if (order.size() == numCourses) return order;
    return {};  // 有环
}
```

---

## 专题: LRU / LFU Cache

### LC146 LRU缓存 (Medium-Hard)
**问题**: O(1) get/put的最近最少使用缓存
**核心**: 双向链表 + 哈希表

```cpp
class LRUCache {
    struct Node {
        int key, val;
        Node *prev, *next;
        Node(int k=0, int v=0): key(k), val(v), prev(nullptr), next(nullptr) {}
    };
    int cap;
    unordered_map<int, Node*> mp;
    Node *head, *tail;
    
    void remove(Node* n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }
    void insertFront(Node* n) {
        n->next = head->next;
        n->prev = head;
        head->next->prev = n;
        head->next = n;
    }
public:
    LRUCache(int capacity): cap(capacity) {
        head = new Node(); tail = new Node();
        head->next = tail; tail->prev = head;
    }
    int get(int key) {
        if (!mp.count(key)) return -1;
        Node* n = mp[key];
        remove(n);
        insertFront(n);
        return n->val;
    }
    void put(int key, int value) {
        if (mp.count(key)) {
            Node* n = mp[key];
            n->val = value;
            remove(n);
            insertFront(n);
        } else {
            Node* n = new Node(key, value);
            mp[key] = n;
            insertFront(n);
            if (mp.size() > cap) {
                Node* evict = tail->prev;
                remove(evict);
                mp.erase(evict->key);
                delete evict;
            }
        }
    }
};
```

### LC460 LFU缓存 (Hard)
**问题**: O(1) get/put的最不经常使用缓存
**核心**: 双向链表（频率分组）+ 哈希表

---

## 专题: 滑动窗口进阶

### LC76 最小覆盖子串 (Hard)
**问题**: 找s中包含t所有字符的最小子串
**核心**: 变长滑动窗口，双指针

```cpp
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    int left = 0, right = 0, valid = 0;
    int start = 0, len = INT_MAX;
    
    while (right < s.size()) {
        char c = s[right++];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        
        while (valid == need.size()) {
            if (right - left < len) {
                start = left; len = right - left;
            }
            char d = s[left++];
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return len == INT_MAX ? "" : s.substr(start, len);
}
```

---

## 验证结果

| 题目 | 难度 | 专题 | 验证 |
|------|------|------|------|
| LC207 课程表 | Medium | 拓扑排序 | ✅ |
| LC210 课程表 II | Medium | 拓扑排序 | ✅ |
| LC146 LRU缓存 | Medium | 设计 | ✅ |
| LC76 最小覆盖子串 | Hard | 滑动窗口 | ✅ |

---

*悟空LC练习 | 2026-04-18 周六上午*
