#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> dpTable;
set<string> lcsSet;
string x, y;

int lcs(string a, string b, int m, int n) {
    dpTable = vector<vector<int>>(m + 1, vector<int>(n + 1));
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            if (i == 0 || j == 0)
                dpTable[i][j] = 0;
            else if (a[i - 1] == b[j - 1])
                dpTable[i][j] = dpTable[i - 1][j - 1] + 1;
            else
                dpTable[i][j] = max(dpTable[i - 1][j], dpTable[i][j - 1]);
        }
    }

    return 0;
}

int getlcs(int i, int j, string str) {
    while (i > 0 && j > 0) {
        if (x[i - 1] == y[j - 1]) {
            str.push_back(x[i - 1]);
            --i;
            --j;
        } else if (dpTable[i - 1][j] > dpTable[i][j - 1])
            --i;
        else if (dpTable[i - 1][j] < dpTable[i][j - 1])
            --j;
        else {
            getlcs(i - 1, j, str);
            getlcs(i, j - 1, str);
            return 0;
        }
    }

    reverse(str.begin(), str.end());
    lcsSet.insert(str);

    return 0;
}

int main() {
    cout << "Please enter two strings (seperate by '\\n'):" << endl;
    cin >> x >> y;
    int m = x.size();
    int n = y.size();
    lcs(x, y, m, n);
    string str;
    getlcs(m, n, str);

    cout << "The longest common substrings are:" << endl;
    for (set<string>::iterator iter = lcsSet.begin(); iter != lcsSet.end();
         ++iter)
        cout << *iter << endl;

    return 0;
}
