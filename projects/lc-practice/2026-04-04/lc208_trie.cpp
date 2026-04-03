// LC208 Implement Trie
// 208. Implement Trie (Prefix Tree) - Medium
// 前缀树实现：insert O(m), search O(m), startsWith O(m), m=word length

#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEnd;
    TrieNode(): isEnd(false) {}
};

class Trie {
    TrieNode* root;
public:
    Trie(): root(new TrieNode()) {}
    
    // 插入单词
    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEnd = true;
    }
    
    // 搜索完整单词
    bool search(string word) {
        TrieNode* node = findNode(word);
        return node && node->isEnd;
    }
    
    // 搜索前缀
    bool startsWith(string prefix) {
        return findNode(prefix) != nullptr;
    }
    
private:
    TrieNode* findNode(string s) {
        TrieNode* node = root;
        for (char c : s) {
            if (node->children.find(c) == node->children.end()) {
                return nullptr;
            }
            node = node->children[c];
        }
        return node;
    }
};

// 测试
int main() {
    Trie trie;
    trie.insert("apple");
    cout << trie.search("apple") << endl;   // true
    cout << trie.search("app") << endl;     // false
    cout << trie.startsWith("app") << endl; // true
    trie.insert("app");
    cout << trie.search("app") << endl;     // true
    return 0;
}
