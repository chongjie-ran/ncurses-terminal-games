#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <string>
using namespace std;

bool oneDiff(const string& a, const string& b) {
    int diff = 0;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) diff++;
        if (diff > 1) return false;
    }
    return diff == 1;
}

int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
    unordered_set<string> dict(wordList.begin(), wordList.end());
    if (!dict.count(endWord)) return 0;
    
    unordered_set<string> visited;
    queue<pair<string,int>> q;
    q.push({beginWord, 1});
    visited.insert(beginWord);
    
    while (!q.empty()) {
        auto [word, len] = q.front(); q.pop();
        if (word == endWord) return len;
        
        string next = word;
        for (int i = 0; i < word.size(); i++) {
            char orig = next[i];
            for (char c = 'a'; c <= 'z'; c++) {
                if (c == orig) continue;
                next[i] = c;
                if (dict.count(next) && !visited.count(next)) {
                    visited.insert(next);
                    q.push({next, len+1});
                }
            }
            next[i] = orig;
        }
    }
    return 0;
}

int main() {
    vector<string> wl = {"hot","dot","dog","lot","log","cog"};
    cout << ladderLength("hit", "cog", wl) << endl; // 5
    return 0;
}
