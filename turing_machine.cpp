#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Turing {
private:
    map < pair < string, string > , pair < pair < string, string >, string > > delta;
    string finalState;
public:
    const string proc(const string raw);

    friend istream& operator >> (istream& in, Turing &t);
};

const string Turing::proc(const string raw) {
    vector < string > w;
    if (raw.size() > 0) {
        string tmp;
        for (int i = 0; i < raw.size(); i++) {
            if (raw[i] == '0') {
                w.push_back(tmp);
                tmp.clear();
            } else {
                tmp += raw[i];
            }
        } w.push_back(tmp);
    }

    map < int, string > outOfPos;

    int pos = 0;
    int l = 0;
    int r = w.size() - 1;
    string cur;
    string state = "1";
    while (true) {
        if (pos < l || pos > r) {
            cur = "1";
        } else if (pos < 0 || pos >= w.size()) {
            if (outOfPos.find(pos) == outOfPos.end()) {
                cur = "1";
            } else {
                cur = outOfPos[pos];
            }
        } else {
            cur = w[pos];
        }
        pair < string, string > curPr = make_pair(state, cur);
        if (state != finalState && delta.find(curPr) == delta.end()) {
            return "Rejected";
        }
        if (pos < 0 || pos >= w.size()) {
            l = min(l, pos);
            r = max(r, pos);
            outOfPos[pos] = delta[curPr].first.second;
        } else {
            w[pos] = delta[curPr].first.second;
        }
        if (delta[curPr].second == "1") {
            pos--;
        } else {
            pos++;
        }
        state = delta[curPr].first.first;
        if (state == finalState) {
            return "Accepted";
        }
    }
}

istream& operator >> (istream& in, Turing &t) {
    string encoded;
    in >> encoded;
    for (int i = 0; i < encoded.size(); i++) {
        string q1;
        for (; encoded[i] != '0'; i++) {
            q1 += encoded[i];
        } i++;

        string a; // a -> b
        for (; encoded[i] != '0'; i++) {
            a += encoded[i];
        } i++;

        string q2;
        for (; encoded[i] != '0'; i++) {
            q2 += encoded[i];
        } i++;

        string b; // a -> b
        for (; encoded[i] != '0'; i++) {
            b += encoded[i];
        } i++;

        string mov;
        for (; encoded[i] != '0' && i < encoded.size(); i++) {
            mov += encoded[i];
        } i++;

        t.delta[make_pair(q1, a)] = make_pair(make_pair(q2, b), mov);
        if (t.finalState.size() < q1.size()) {
            t.finalState = q1;
        }
        if (t.finalState.size() < q2.size()) {
            t.finalState = q2;
        }
    }
}

int main() {
    Turing TM;
    cin >> TM;
    int n;
    cin >> n;
    cin.ignore();
    for (int i = 0; i < n; i++) {
        string s;
        getline(cin, s);
        cout << TM.proc(s) << endl;
    }
}
