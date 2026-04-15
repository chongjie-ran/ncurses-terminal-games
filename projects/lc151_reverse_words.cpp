// LC151: 反转字符串中的单词
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

string reverseWords(string s) {
    stringstream ss(s);
    string word, result;
    while (ss >> word) {
        if (!result.empty()) result = word + " " + result;
        else result = word;
    }
    return result;
}

int main() {
    cout << reverseWords("the sky is blue") << endl;    // "blue is sky the"
    cout << reverseWords("  hello world  ") << endl;   // "world hello"
    cout << reverseWords("a good   example") << endl;  // "example good a"
    return 0;
}
