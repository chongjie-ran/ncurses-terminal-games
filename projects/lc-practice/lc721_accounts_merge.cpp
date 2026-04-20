#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

vector<int> parent, sz;

int find(int x) {
    if (parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
}

void unite(int x, int y) {
    int px = find(x), py = find(y);
    if (px == py) return;
    if (sz[px] < sz[py]) swap(px, py);
    parent[py] = px;
    sz[px] += sz[py];
}

vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
    unordered_map<string, int> email_to_id;
    unordered_map<string, string> email_to_name;
    int id = 0;
    for (auto& acc : accounts) {
        string name = acc[0];
        for (int i = 1; i < (int)acc.size(); i++) {
            string email = acc[i];
            if (email_to_id.find(email) == email_to_id.end()) {
                email_to_id[email] = id++;
            }
            email_to_name[email] = name;
        }
    }
    parent.resize(id);
    sz.assign(id, 1);
    for (int i = 0; i < id; i++) parent[i] = i;
    
    for (auto& acc : accounts) {
        for (int i = 2; i < (int)acc.size(); i++) {
            unite(email_to_id[acc[1]], email_to_id[acc[i]]);
        }
    }
    
    unordered_map<int, vector<string>> groups;
    for (auto& kv : email_to_id) {
        int root = find(kv.second);
        groups[root].push_back(kv.first);
    }
    
    vector<vector<string>> result;
    for (auto& kv : groups) {
        vector<string> acc;
        string name = email_to_name[kv.second[0]];
        acc.push_back(name);
        sort(kv.second.begin(), kv.second.end());
        for (auto& e : kv.second) acc.push_back(e);
        result.push_back(acc);
    }
    return result;
}

int main() {
    vector<vector<string>> accounts = {
        {"John","johnsmith@mail.com","john_newyork@mail.com"},
        {"John","johnsmith@mail.com","john00@mail.com"},
        {"Mary","mary@mail.com"},
        {"John","johnnybravo@mail.com"}
    };
    auto res = accountsMerge(accounts);
    cout << "LC721: " << res.size() << " accounts" << endl;
    for (auto& acc : res) {
        cout << "  " << acc[0] << ": " << (acc.size()-1) << " emails";
        if (acc[0] == "John" && acc.size()-1 == 3) cout << " ✓";
        if (acc[0] == "Mary" && acc.size()-1 == 1) cout << " ✓";
        cout << endl;
    }
    // Expected: John=3emails(john00,john_newyork,johnsmith), Mary=1email, John=1email(johnnybravo)
    return 0;
}
