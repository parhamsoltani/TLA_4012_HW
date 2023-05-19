#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

vector<string> ScanSet() {
    string Line;
    getline(cin, Line);
    vector<string> Set;

    string substr = Line.substr(1, Line.length() - 2);
    size_t pos = 0;
    string token;

    while ((pos = substr.find(',')) != string::npos) {
        token = substr.substr(0, pos);
        Set.push_back(token);
        substr.erase(0, pos + 1);
    }

    Set.push_back(substr);

    return Set;
}

tuple<string, string, string> ScanEdge() {
    string Line;
    getline(cin, Line);

    size_t pos = Line.find(',');
    string first = Line.substr(0, pos);
    Line.erase(0, pos + 1);

    pos = Line.find(',');
    string second = Line.substr(0, pos);
    Line.erase(0, pos + 1);

    string third = Line;

    return make_tuple(first, second, third);
}

int main() {
    vector<string> States = ScanSet();
    unordered_map<string, int> StateId;

    for (int i = 0; i < States.size(); ++i) {
        StateId[States[i]] = i;
    }

    int statesCount = States.size();

    vector<string> Alphabet = ScanSet();
    unordered_map<string, int> AlphabetId;

    for (int i = 0; i < Alphabet.size(); ++i) {
        AlphabetId[Alphabet[i]] = i;
    }

    int alphabetCount = Alphabet.size();

    vector<string> Targets = ScanSet();

    vector<vector<int>> T(statesCount, vector<int>(alphabetCount));
    int m;
    cin >> m;
    cin.ignore();

    for (; m > 0; --m) {
        auto [src, val, dst] = ScanEdge();
        int srcId = StateId[src];
        int valId = AlphabetId[val];
        int dstId = StateId[dst];
        T[srcId][valId] = dstId;
    }

    vector<int> Name(statesCount, -1);

    vector<int> Q;
    Q.push_back(0);

    for (int i = 0; i < Q.size(); ++i) {
        int v = Q[i];
        Name[v] = 0;

        for (int a = 0; a < alphabetCount; ++a) {
            int u = T[v][a];

            if (Name[u] == -1) {
                Q.push_back(u);
            }
        }
    }

    for (const auto& s : Targets) {
        int id = StateId[s];

        if (Name[id] != -1) {
            Name[id] = 1;
        }
    }

    int sz = 2;

    while (true) {
        vector<int> NewName(statesCount, -1);
        unordered_map<long long, int> NameId;
        int nextNameId = 0;

        for (int s = 0; s < statesCount; ++s) {
            if (Name[s] == -1) {
                NewName[s] = -1;
                continue;
            }

            long long nn = Name[s];

            for (int a = 0; a < alphabetCount; ++a) {
                nn = nn * static_cast<long long>(sz) + static_cast<long long>(Name[T[s][a]]);
            }

            auto it = NameId.find(nn);

            if (it == NameId.end()) {
                NameId[nn] = nextNameId;
                it = NameId.find(nn);
                nextNameId++;
            }

            NewName[s] = it->second;
        }

        if (nextNameId == sz) {
            break;
        }

        Name = NewName;
        sz = nextNameId;
    }

    cout << sz;

    return 0;
}
