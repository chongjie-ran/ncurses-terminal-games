// LC208 Implement Trie (Prefix Tree)
// 2026-04-18 01:50 SAT
#include <iostream>
#include <string>
using namespace std;

class TrieNode {
public:
    TrieNode* children[26];
    bool isWord;
    TrieNode() {
        isWord = false;
        for (int i = 0; i < 26; i++) children[i] = nullptr;
    }
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    
    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isWord = true;
    }
    
    bool search(string word) {
        TrieNode* node = findNode(word);
        return node && node->isWord;
    }
    
    bool startsWith(string prefix) {
        return findNode(prefix) != nullptr;
    }
    
private:
    TrieNode* findNode(string s) {
        TrieNode* node = root;
        for (char c : s) {
            int idx = c - 'a';
            if (!node->children[idx]) return nullptr;
            node = node->children[idx];
        }
        return node;
    }
};

// Test
int main() {
    Trie t;
    t.insert("apple");
    cout << (t.search("apple") ? "T" : "F") << endl;   // T
    cout << (t.search("app") ? "T" : "F") << endl;     // F
    cout << (t.startsWith("app") ? "T" : "F") << endl; // T
    t.insert("app");
    cout << (t.search("app") ? "T" : "F") << endl;     // T
    return 0;
}
