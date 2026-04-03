// LC211 Design Add and Search Words Data Structure
// 211. Design Add and Search Words Data Structure - Medium
// 在Trie基础上支持通配符.搜索任意字符

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

class WordDictionary {
    TrieNode* root;
    
    bool searchNode(const string& word, int idx, TrieNode* node) {
        if (!node) return false;
        if (idx == word.size()) return node->isEnd;
        
        char c = word[idx];
        if (c == '.') {
            // 通配符：搜索所有子节点
            for (auto p : node->children) {
                if (searchNode(word, idx + 1, p.second)) return true;
            }
            return false;
        } else {
            if (node->children.find(c) == node->children.end()) return false;
            return searchNode(word, idx + 1, node->children[c]);
        }
    }
    
public:
    WordDictionary(): root(new TrieNode()) {}
    
    void addWord(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        return searchNode(word, 0, root);
    }
};

// 测试
int main() {
    WordDictionary wd;
    wd.addWord("bad");
    wd.addWord("dad");
    wd.addWord("mad");
    cout << wd.search("pad") << endl;  // false
    cout << wd.search("bad") << endl; // true
    cout << wd.search(".ad") << endl; // true
    cout << wd.search("b..") << endl; // true
    cout << wd.search("b.d") << endl; // false
    return 0;
}
