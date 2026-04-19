// LC212 Word Search II - Trie + DFS回溯
// 2026-04-18 01:55 SAT
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct TrieNode {
    TrieNode* children[26];
    string word;
    TrieNode() {
        word = "";
        for (int i = 0; i < 26; i++) children[i] = nullptr;
    }
};

class Solution {
    int m, n;
    vector<string> result;
    
    TrieNode* buildTrie(vector<string>& words) {
        TrieNode* root = new TrieNode();
        for (string& w : words) {
            TrieNode* node = root;
            for (char c : w) {
                int idx = c - 'a';
                if (!node->children[idx]) node->children[idx] = new TrieNode();
                node = node->children[idx];
            }
            node->word = w;
        }
        return root;
    }
    
    void dfs(vector<vector<char>>& board, int i, int j, TrieNode* node) {
        char c = board[i][j];
        if (c == '#' || !node->children[c-'a']) return;
        
        node = node->children[c-'a'];
        if (!node->word.empty()) {
            result.push_back(node->word);
            node->word = ""; // 去重
        }
        
        board[i][j] = '#';
        if (i > 0) dfs(board, i-1, j, node);
        if (i+1 < m) dfs(board, i+1, j, node);
        if (j > 0) dfs(board, i, j-1, node);
        if (j+1 < n) dfs(board, i, j+1, node);
        board[i][j] = c;
    }
    
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        m = board.size(); n = board[0].size();
        TrieNode* root = buildTrie(words);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                dfs(board, i, j, root);
        return result;
    }
};

int main() {
    Solution s;
    vector<vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    vector<string> words = {"oath","pea","eat","rain"};
    vector<string> r = s.findWords(board, words);
    for (auto& w : r) cout << w << endl;
    return 0;
}
