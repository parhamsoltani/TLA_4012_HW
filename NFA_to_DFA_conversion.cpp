#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

int code_inp(const string& txt) {
    return stoi(txt.substr(1));
}

int code_alph(const string& txt, const vector<string>& alph) {
    auto it = find(alph.begin(), alph.end(), txt);
    return distance(alph.begin(), it);
}

vector<string> inp() {
    string tmp;
    getline(cin, tmp);
    vector<string> res;
    string resu;

    for (int i = 1; i < tmp.length() - 1; ++i) {
        resu += tmp[i];
    }

    size_t pos = 0;
    string token;

    while ((pos = resu.find(',')) != string::npos) {
        token = resu.substr(0, pos);
        res.push_back(token);
        resu.erase(0, pos + 1);
    }

    res.push_back(resu);

    return res;
}

void addNewState(unordered_map<string, vector<string>>& DFA, const vector<string>& tmp, const vector<string>& alph, const string& title, const vector<vector<vector<string>>>& board, const unordered_map<string, vector<string>>& neighbors) {
DFA[title] = vector<string>(alph.size());

for (int i = 0; i < alph.size(); ++i) {
vector<string> temp;

for (int j = 0; j < tmp.size(); ++j) {
if (tmp[j] == "") {
continue;
}

for (const string& x : board[stoi(tmp[j])][i]) {
if (find(temp.begin(), temp.end(), x) == temp.end()) {
temp.push_back(x);

for (const string& k : neighbors.at(x)) {
if (find(temp.begin(), temp.end(), k) == temp.end()) {
temp.push_back(k);
}
}
}
}
}

sort(temp.begin(), temp.end());
DFA[title][i] = "";

for (const string& t : temp) {
DFA[title][i] += t;
}
}
}

int main() {
    vector<string> st = inp();
    vector<vector<vector<string>>> board(st.size(), vector<vector<string>>());
    vector<string> alph = inp();
    string f_st;
    getline(cin, f_st);
    unordered_map<string, vector<string>> neighbors;
    int sz = st.size();
    int n;
    cin >> n;
    cin.ignore();

    string st_name;

    for (int i = 0; i < sz; ++i) {
        st_name = "q" + to_string(i);
        neighbors[st_name] = vector<string>();
    }

    for (int i = 0; i < sz; ++i) {
        board[i] = vector<vector<string>>(alph.size());
    }

    unordered_map<string, string> reformat;

    for (int i = 0; i < st.size(); ++i) {
        reformat[st[i]] = "q" + to_string(i);
    }

    for (int i = 0; i < n; ++i) {
        string line;
        getline(cin, line);
        vector<string> tmp;
        size_t pos = 0;

        while ((pos = line.find(',')) != string::npos) {
            tmp.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }

        tmp.push_back(line);
        string name_st = reformat[tmp[0]];
        int code = code_inp(reformat[tmp[0]]);

        if (tmp[1] == "$") {
            st[code] += reformat[tmp[2]];
            int code2 = code_inp(reformat[tmp[2]]);
            neighbors["q" + to_string(code)].push_back("q" + to_string(code2));
        } else {
            int CodeAl = code_alph(tmp[1], alph);
            board[code][CodeAl].push_back(reformat[tmp[2]]);
        }
    }

    unordered_map<string, vector<string>> DFA;
    vector<string> tmp = st[0].substr(1).split('q');
    addNewState(DFA, tmp, alph, "q0", board, neighbors);
    string title = "q0";
    bool condition = true;
    int i = 0;

    while (i < (sz * 2)) {
        i++;

        for (const string& x : DFA[title]) {
            if (x == "") {
                continue;
            }

            vector<string> tmp = x.substr(1).split('q');
            addNewState(DFA, tmp, alph, x, board, neighbors);
            title = x;
        }
    }

    cout << DFA.size() + 1;

    return 0;
}
