#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <utility>
using namespace std;

vector<double> calcEquation(vector<vector<string>>& equations, vector<double>& values, vector<vector<string>>& queries) {
    unordered_map<string, int> idx;
    int id = 0;
    for (auto& eq : equations) {
        if (idx.find(eq[0]) == idx.end()) idx[eq[0]] = id++;
        if (idx.find(eq[1]) == idx.end()) idx[eq[1]] = id++;
    }
    vector<vector<pair<int,double>>> adj(id);
    for (int i = 0; i < equations.size(); i++) {
        int u = idx[equations[i][0]], v = idx[equations[i][1]];
        adj[u].push_back({v, values[i]});
        adj[v].push_back({u, 1.0/values[i]});
    }
    vector<double> result;
    for (auto& q : queries) {
        if (idx.find(q[0]) == idx.end() || idx.find(q[1]) == idx.end()) {
            result.push_back(-1.0);
        } else if (q[0] == q[1]) {
            result.push_back(1.0);
        } else {
            vector<double> visited(id, -1.0);
            queue<pair<int,double>> que;
            que.push({idx[q[0]], 1.0});
            visited[idx[q[0]]] = 1.0;
            bool found = false;
            while (!que.empty() && !found) {
                auto [node, val] = que.front(); que.pop();
                if (node == idx[q[1]]) {
                    result.push_back(val);
                    found = true;
                    break;
                }
                for (auto& [nbr, w] : adj[node]) {
                    if (visited[nbr] < 0) {
                        visited[nbr] = val * w;
                        que.push({nbr, val * w});
                    }
                }
            }
            if (!found) result.push_back(-1.0);
        }
    }
    return result;
}

int main() {
    vector<vector<string>> equations = {{"a","b"},{"b","c"}};
    vector<double> values = {2.0, 3.0};
    vector<vector<string>> queries = {{"a","c"},{"b","a"},{"a","e"},{"a","a"},{"x","x"}};
    vector<double> res = calcEquation(equations, values, queries);
    vector<double> exp = {6.0, 0.5, -1.0, 1.0, -1.0};
    cout << "LC399: ";
    bool ok = true;
    for (int i = 0; i < res.size(); i++) {
        cout << res[i] << " ";
        if (abs(res[i] - exp[i]) > 0.001) ok = false;
    }
    cout << (ok ? "✓" : "✗ FAIL") << endl;
    // a/c=6, b/a=0.5, a/e=-1, a/a=1, x/x=-1
    return 0;
}
