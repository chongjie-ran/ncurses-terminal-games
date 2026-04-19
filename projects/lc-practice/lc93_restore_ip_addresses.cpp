// LC93 Restore IP Addresses - 回溯
// 将字符串分割成有效的IP地址段
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    vector<string> restoreIpAddresses(string s) {
        results.clear();
        if (s.size() < 4 || s.size() > 12) return results;
        path.clear();
        this->s = s;
        backtrack(0, 0);
        return results;
    }

private:
    vector<string> results;
    vector<string> path;
    string s;

    bool isValid(const string& seg) {
        if (seg.empty() || seg.size() > 3) return false;
        if (seg.size() > 1 && seg[0] == '0') return false;  // 不允许前导0
        int val = stoi(seg);
        return val >= 0 && val <= 255;
    }

    void backtrack(int start, int segIdx) {
        if (segIdx == 4) {
            if (start == s.size()) {
                string ip;
                for (int i = 0; i < 4; i++) {
                    ip += path[i];
                    if (i < 3) ip += '.';
                }
                results.push_back(ip);
            }
            return;
        }
        // 剩余段数
        int remaining = 4 - segIdx;
        // 剩余字符数
        int remainLen = s.size() - start;
        if (remainLen < remaining || remainLen > remaining * 3) return;

        // 尝试1-3个字符
        for (int len = 1; len <= 3 && start + len <= s.size(); len++) {
            string seg = s.substr(start, len);
            if (!isValid(seg)) continue;
            path.push_back(seg);
            backtrack(start + len, segIdx + 1);
            path.pop_back();
        }
    }
};

int main() {
    Solution sol;

    // Test 1: "25525511135" → ["255.255.11.135","255.255.111.35"]
    string s1 = "25525511135";
    auto r1 = sol.restoreIpAddresses(s1);
    cout << "Test '25525511135': " << r1.size() << " addresses" << endl;
    for (auto& ip : r1) cout << "  " << ip << endl;

    // Test 2: "0000" → ["0.0.0.0"]
    string s2 = "0000";
    auto r2 = sol.restoreIpAddresses(s2);
    cout << "Test '0000': " << r2.size() << " addresses (expected 1)" << endl;
    for (auto& ip : r2) cout << "  " << ip << endl;

    // Test 3: "101023" → ["1.0.10.23","1.0.102.3","10.1.0.23","10.10.2.3","101.0.2.3"]
    string s3 = "101023";
    auto r3 = sol.restoreIpAddresses(s3);
    cout << "Test '101023': " << r3.size() << " addresses (expected 5)" << endl;
    for (auto& ip : r3) cout << "  " << ip << endl;

    // Test 4: "1111" → ["1.1.1.1"]
    string s4 = "1111";
    auto r4 = sol.restoreIpAddresses(s4);
    cout << "Test '1111': " << r4.size() << " addresses (expected 1)" << endl;
    for (auto& ip : r4) cout << "  " << ip << endl;

    return 0;
}
