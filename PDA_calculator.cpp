# define PI_2       1.57

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <sstream>
#include <cctype>
#include <stack>
#include <cstdlib>
#include <cmath>
#include <cstdio>

using namespace std;

template <class T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <int N>
class Grammar {
private:
    string rawProdRules[N];
    int n;
    map < string, vector < string > > prodRules;
    string stV;
public:
    void display();
    bool removeNullable();
    bool removeUnitProd();
    bool removeUnreachable();
    bool removeUnterminatable();
    void removeDuplicate();
    void removeInvalidProdRules();
    void simplify();
    void toCNF();
    const bool CYKMembership(const string& w);

    template <int M>
    friend stringstream& operator >> (stringstream& in, Grammar < M > &g);
};

template <int N>
void Grammar<N>::display() {
    cout << "\nGrammar Display for Debug Perposes:\n";
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            cout << it -> first << " -> " << it -> second[i] << endl;
        }
    }
    cout << endl;
}

template <int N>
bool Grammar<N>::removeNullable() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            if (it -> second[i] == "#") {
                it -> second.erase(it -> second.begin() + i);

                map < string, vector < string > >::iterator it2;
                for (it2 = prodRules.begin(); it2 != prodRules.end(); it2++) {
                    for (int j = 0; j < it2 -> second.size(); j++) {
                        for (int k = it2 -> second[j].size() - 3; k >= 0; k--) {
                            if (it2 -> second[j].substr(k, 3) == it -> first) {
                                it2 -> second.push_back(it2 -> second[j]);
                                it2 -> second.back().erase(k, 3);
                                if (it2 -> second.back() == it2 -> first) {
                                    it2 -> second.pop_back();
                                } else if (it2 -> second.back().empty()) {
                                    it2 -> second.back() = "#";
                                    if (it2 -> first == it -> first) {
                                        it2 -> second.pop_back();
                                     }
                                }
                                break;
                            }
                        }
                    }
                }
                return true;
            }
        }
    }
    return false;
}

template <int N>
bool Grammar<N>::removeUnitProd() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = it -> second.size() - 1; i >= 0 ; i--) {
            if (it -> second[i].size() == 3 && it -> second[i][0] == '<' && it -> second[i][2] == '>') {
                if (it -> second[i][1] == it -> first[1]) {
                    it -> second.erase(it -> second.begin() + i);
                } else {
                    for (int j = 0; j < prodRules[it -> second[i]].size(); j++) {
                        it -> second.push_back(prodRules[it -> second[i]][j]);
                    }
                    it -> second.erase(it -> second.begin() + i);
                }
                return true;
            }
        }
    }
    return false;
}

template <int N>
bool Grammar<N>::removeUnreachable() {
    queue < string > q;
    set < string > visited;
    q.push(stV);
    visited.insert(stV);
    while (!q.empty()) {
        string top = q.front();
        q.pop();
        for (int i = 0; i < prodRules[top].size(); i++) {
            if (prodRules[top][i].size() < 3) continue;
            for (int j = 0; j < prodRules[top][i].size() - 2; j++) {
                if (prodRules[top][i][j] == '<' && prodRules[top][i][j + 2] == '>') {
                    if (visited.find(prodRules[top][i].substr(j, 3)) == visited.end()) {
                        visited.insert(prodRules[top][i].substr(j, 3));
                        q.push(prodRules[top][i].substr(j, 3));
                    }
                }
            }

        }
    }
    map < string, vector < string > >::iterator it;
    int i = 0;
    vector < string > toDel;
    bool res = false;
    for (it = prodRules.begin(); it != prodRules.end(); it++, i++) {
        if (visited.find(it -> first) == visited.end()) {
            toDel.push_back(it -> first);
            res = true;
        }
    }
    for (int i = 0; i < toDel.size(); i++) {
        prodRules.erase(prodRules.find(toDel[i]));
    }
    return res;
}

template <int N>
bool Grammar<N>::removeUnterminatable() {
    bool flg = true;
    bool res = false;
    set < string > terminatable;
    while (flg) {
        flg = false;
        map < string, vector < string > >::iterator it;
        for (it = prodRules.begin(); it != prodRules.end(); it++) {
            if (terminatable.find(it -> first) != terminatable.end()) {
                continue;
            }
            for (int i = 0; i < it -> second.size(); i++) {

                if (it -> second[i].size() < 3) {
                    terminatable.insert(it -> first);
                    flg = true;
                    break;
                }
                bool isTerminatable = true;
                for (int j = 0; j < it -> second[i].size() - 2; j++) {
                    if (it -> second[i][j] == '<' && it -> second[i][j + 2] == '>') {
                        if (terminatable.find(it -> second[i].substr(j, 3)) == terminatable.end()) {
                            isTerminatable = false;
                        }
                    }
                }
                if (isTerminatable) {
                    terminatable.insert(it -> first);
                    flg = true;
                    break;
                }

            }
        }
    }
    map < string, vector < string > >::iterator it;
    vector < string > toDel;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        if (terminatable.find(it -> first) == terminatable.end()) {
            toDel.push_back(it -> first);
            res = true;
        }
    }
    for (int i = 0; i < toDel.size(); i++) {
        prodRules.erase(prodRules.find(toDel[i]));
    }
    return res;
}

template <int N>
void Grammar<N>::removeDuplicate() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {

        vector < string >::iterator it2;
        vector < string >::iterator itEnd = it -> second.end();
        for (it2 = it -> second.begin(); it2 != itEnd; it2++) {
            itEnd = remove(it2 + 1, itEnd, *it2);
        }
        it -> second.erase(itEnd, it -> second.end());
    }
}

template <int N>
void Grammar<N>::removeInvalidProdRules() {
    set < string > validVars;
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        if (it -> second.size() > 0) {
            validVars.insert(it -> first);
        }
    }
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = it -> second.size() - 1; i >= 0; i--) {

            if (it -> second[i].size() < 3) {
                continue;
            }
            for (int j = 0; j < it -> second[i].size() - 2; j++) {
                if (it -> second[i][j] == '<' && it -> second[i][j + 2] == '>') {
                    if (validVars.find(it -> second[i].substr(j, 3)) == validVars.end()) {
                        it -> second.erase(it -> second.begin() + i);
                        break;
                    }
                }
            }

        }
    }
}

template <int N>
void Grammar<N>::simplify() {
    bool flg = true;
    while (flg) {
        flg = false;
        /*this -> display();
        string tmp;
        getline(cin, tmp);*/
        if (this -> removeNullable() || this -> removeUnitProd() || this -> removeUnreachable() || this -> removeUnterminatable()) {
            flg = true;
            this -> removeDuplicate();
            this -> removeInvalidProdRules();
            continue;
        }
    }
}

template <int N>
void Grammar<N>::toCNF() {

    map < char, string > terVar;
    string newVar; newVar += '<'; newVar += (char)33; newVar += '>';
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) { // Assign production rules for each terminal
        for (int i = 0; i < it -> second.size(); i++) {
            if (it -> second[i].size() == 1) {
                continue;
            }
            for (int j = 0; j < it -> second[i].size();) {
                if (it -> second[i][j] == '<') {
                    j += 3;
                } else {
                    if (terVar.find(it -> second[i][j]) == terVar.end()) {
                        terVar[it -> second[i][j]] = newVar; //cout << "terVar " << it -> second[i][j] << ' ' << newVar << endl;
                        newVar[1]++;
                    }
                    j++;
                }
            }
        }
    }
    map < char, string >::iterator it2;
    for (it2 = terVar.begin(); it2 != terVar.end(); it2++) {
        string rightSide; rightSide += it2 -> first;
        for (it = prodRules.begin(); it != prodRules.end(); it++) {
            for (int i = 0; i < it -> second.size(); i++) {
                if (it -> second[i].size() == 1) {
                    continue;
                }
                vector < int > insertPos;
                for (int j = 0; j < it -> second[i].size();) {
                    if (it -> second[i][j] == '<') {
                        j += 3;
                    } else {
                        if (it2 -> first == it -> second[i][j]) {
                            insertPos.push_back(j);
                        }
                        j++;
                    }
                }
                for (int j = insertPos.size() - 1; j >= 0; j--) {
                    it -> second[i].erase(insertPos[j], 1);
                    it -> second[i].insert(insertPos[j], it2 -> second);
                }
            }
        }
        prodRules[it2 -> second].push_back(rightSide);
    }

    // Reduce right side of production rules to just two Variables (not less not more)
    map < string, string > newProd;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            while (it -> second[i].size() > 6) {
                if (newProd.find(it -> second[i].substr(0, 6)) == newProd.end()) {
                    newProd[it -> second[i].substr(0, 6)] = newVar;
                    prodRules[newVar].push_back(it -> second[i].substr(0, 6));
                    newVar[1]++;
                }
                string tmp = it -> second[i].substr(0, 6);
                it -> second[i].erase(0, 6);
                it -> second[i].insert(0, newProd[tmp]);
            }
        }
    }

}

template <int N>
const bool Grammar<N>::CYKMembership(const string& w) {

    // Construct the table
    set < string >** dp = new set < string >* [w.size()];
    for (int i = 0; i < w.size(); i++) {
        dp[i] = new set < string > [w.size()];
    }

    // Fill the table
    for (int l = 1; l <= w.size(); l++) { // l = length of the range
        for (int i = 0; i < 1 + w.size() - l; i++) { // range [i, i + l - 1]
            int j = i + l - 1; // rightside position

            // Base case: rightside includes of just terminals
            map < string, vector < string > >::iterator it;
            for (it = prodRules.begin(); it != prodRules.end(); it++) {
                for (int k = 0; k < it -> second.size(); k++) {
                    if (it -> second[k] == w.substr(i, l)) {
                        dp[i][j].insert(it -> first);
                        //cout << "Found a terminate! " << "i:" << i << " j:" << j << " var:" << it -> first << endl;
                    }
                }
            }

            for (int k = i; k < j; k++) { // dp[i][k], dp[k + 1][j]
                set < string > targetProds;
                set < string >::iterator it2;
                set < string >::iterator it3;
                for (it2 = dp[i][k].begin(); it2 != dp[i][k].end(); it2++) {
                    for (it3 = dp[k + 1][j].begin(); it3 != dp[k + 1][j].end(); it3++) {
                        string target1 = *it2;
                        target1 += *it3;
                        string target2 = *it2;
                        target2 += w.substr(k + 1, j - k);
                        string target3 = w.substr(i, k - i + 1);
                        target3 += *it3;
                        targetProds.insert(target1);
                        targetProds.insert(target2);
                        targetProds.insert(target3);
                    }
                }

                // In the search of target productions among available production rules
                map < string, vector < string > >::iterator it4;
                for (it4 = prodRules.begin(); it4 != prodRules.end(); it4++) {
                    for (int p = 0; p < it4 -> second.size(); p++) {
                        if (targetProds.find(it4 -> second[p]) != targetProds.end()) {
                            dp[i][j].insert(it4 -> first);
                        }
                    }
                }

            }
        }
    }

    return dp[0][w.size() - 1].find(stV) != dp[0][w.size() - 1].end();
}

template <int N>
stringstream& operator >> (stringstream& in, Grammar < N > &g) {
    int n;
    in >> n;
    g.n = n;
    in.ignore();
    for (int i = 0; i < n; i++) {
        getline(in, g.rawProdRules[i]);

        int j = 0;
        string left;
        for (;j < g.rawProdRules[i].size(); j++) {
            if (g.rawProdRules[i][j] == '-' && g.rawProdRules[i][j + 1] == '>') {
                left = g.rawProdRules[i].substr(0, j - 1);
                break;
            }
        } j += 3;

        string right;
        g.rawProdRules[i] += '|';
        for (;j < g.rawProdRules[i].size(); j++) {
            if (g.rawProdRules[i][j] == ' ') {
                continue;
            }
            if (g.rawProdRules[i][j] == '|') {
                g.prodRules[left].push_back(right);
                right.clear();
                continue;
            }
            right += g.rawProdRules[i][j];
        }
    }
    g.stV = g.rawProdRules[0].substr(0, 3);
}

string removeSpaces(const string s) {
    string res;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] != ' ') {
            res += s[i];
        }
    }
    return res;
}

map < char, int > precedence;

vector < string > postFix(vector < string > exp) {
    vector < string > res;
    stack < string > stk;
    for (int i = 0; i < exp.size(); i++) {

        // If exp[i] was an operand
        if (isdigit(exp[i][0]) || exp[i].size() > 1 && !isalpha(exp[i][0])) {
            res.push_back(exp[i]);
        }

        if (exp[i][0] == '(') {
            stk.push(exp[i]);
        }

        if (exp[i][0] == ')') {
            while (!stk.empty() && stk.top() != "(") {
                res.push_back(stk.top());
                stk.pop();
            }
            if (!stk.empty()) { // Pop "(" from stack
                stk.pop();
            }
        }

        // If exp[i] was an operator
        if (precedence.find(exp[i][0]) != precedence.end() && (exp[i].size() == 1 || isalpha(exp[i][0]))) {
            if (stk.empty() || stk.top() == "(") {
                stk.push(exp[i]);
            } else {
                while (!stk.empty() && stk.top() != "(" && precedence[exp[i][0]] <= precedence[stk.top()[0]]) {
                    res.push_back(stk.top());
                    stk.pop();
                }
                stk.push(exp[i]);
            }
        }

    }
    while (!stk.empty()) {
        res.push_back(stk.top());
        stk.pop();
    }
    return res;
}

vector < string > f; // for each left parantheses and in order of occurences

const string calc(vector < string > exp) {
    vector < string > PF = postFix(exp);
    stack < string > stk;
    for (int i = 0; i < PF.size(); i++) {

        // If PF[i] was an operand
        if (isdigit(PF[i][0]) || PF[i].size() > 1 && !isalpha(PF[i][0])) {
            stk.push(PF[i]);
        }

        // If exp[i] was an binary operator
        if (precedence.find(PF[i][0]) != precedence.end() && PF[i].size() == 1) {
            double a = strtod(stk.top().data(), NULL);
            stk.pop();
            double b = strtod(stk.top().data(), NULL);
            stk.pop();
            double res;
            switch (PF[i][0]) {
                case '*': {
                    res = b * a;
                } break;
                case '/': {
                    if (a == 0) {
                        return "INVALID";
                    }
                    res = b / a;
                } break;
                case '^': {
                    res = pow(b, a);
                } break;
                case '+': {
                    res = b + a;
                } break;
                case '-': {
                    res = b - a;
                } break;
            }
            stringstream strRes;
            strRes << res;
            stk.push(strRes.str());
        }
        // If exp[i] was an unary operator
        if (precedence.find(PF[i][0]) != precedence.end() && isalpha(PF[i][0])) {
            double a = strtod(stk.top().data(), NULL);
            stk.pop();
            double res;
            if (PF[i] == "sqrt") {
                if (a < 0) {
                    return "INVALID";
                }
                res = sqrt(a);
            } else if (PF[i] == "sin") {
                res = sin(a);
            } else if (PF[i] == "cos") {
                res = cos(a);
            } else if (PF[i] == "tan") {
                double b = 0;
                while (b > a) {
                    b -= PI_2;
                }
                while (b < a) {
                    b += PI_2;
                }
                if (b == a) {
                    return "INVALID";
                }
                res = tan(a);
            } else if (PF[i] == "ln") {
                if (a <= 0) {
                    return "INVALID";
                }
                res = log(a);
            } else if (PF[i] == "exp") {
                res = std::exp(a);
            } else if (PF[i] == "asin") {
                if (a < -1 || a > 1) {
                    return "INVALID";
                }
                res = asin(a);
            } else if (PF[i] == "acos") {
                if (a < -1 || a > 1) {
                    return "INVALID";
                }
                res = acos(a);
            } else if (PF[i] == "atan") {
                res = atan(a);
            } else if (PF[i] == "sgn") {
                res = sgn(a);
            } else if (PF[i] == "abs") {
                res = abs(a);
            } else if (PF[i] == "sinh") {
                res = sinh(a);
            } else if (PF[i] == "cosh") {
                res = cosh(a);
            } else if (PF[i] == "tanh") {
                res = tanh(a);
            }
            stringstream strRes;
            strRes << res;
            stk.push(strRes.str());
        }
    }
    return stk.top();
}

vector < string > split(string exp) {
    vector < string > res;
    string part;
    bool flg = false; // In case of negative unary sign
    for (int i = 0; i < exp.size(); i++) {
        string fTmp;
        while(isalpha(exp[i])) {
            fTmp += exp[i++];
        }
        if (!fTmp.empty()) {
            if (!part.empty()) {
                res.push_back(part);
            }
            part.clear();
            part += fTmp;
        }
        if (isdigit(exp[i])) {
            if (i > 0 && (isdigit(exp[i - 1]) || exp[i - 1] == '.' || flg)) {
                part += exp[i];
                flg = false;
            } else {
                if (!part.empty()) {
                    res.push_back(part);
                }
                part.clear();
                part += exp[i];
            }
        } else if (exp[i] == '-' && (i == 0 || precedence.find(exp[i - 1]) != precedence.end() || exp[i - 1] == '(')) {
            flg = true;
            if (!part.empty()) {
                res.push_back(part);
            }
            part.clear();
            part += exp[i];
        } else if (exp[i] != '.') {
            if (!part.empty()) {
                res.push_back(part);
            }
            part.clear();
            part += exp[i];
        } else {
            part += '.';
        }
    } if (!part.empty()) { res.push_back(part); }
    return res;
}

const bool checkBadSpaces(const string exp) {
    bool flg = false;
    for (int i = 0; i < exp.size(); i++) {
        if (flg && i > 0 && exp[i - 1] == ' ' && isdigit(exp[i])) {
            return true;
        }
        if (isdigit(exp[i])) {
            flg = true;
        } else if (exp[i] != ' ') {
            flg = false;
        }
    }
    return false;
}

int main() {
    precedence['+'] = 1;
    precedence['-'] = 1;
    precedence['*'] = 2;
    precedence['/'] = 2;
    precedence['^'] = 3;
    precedence['s'] = 4;
    precedence['c'] = 4;
    precedence['t'] = 4;
    precedence['l'] = 4;
    precedence['a'] = 4;
    precedence['e'] = 4;

    Grammar < 10000 > g; // 10000 means maximum number of production rules
    stringstream in;
    in << 5 << '\n';
    in << "<S> -> <X> | <S>+<S> | <S>-<S> | <S>*<S> | <S>/<S> | <S>^<S> | (<S>) | sqrt(<S>) | sin(<S>) | cos(<S>) | tan(<S>) | asin(<S>) | acos(<S>) | atan(<S>) | sgn(<S>) | abs(<S>) | exp(<S>) | ln(<S>) | sinh(<S>) | cosh(<S>) | tanh(<S>)\n";
    in << "<X> -> -<A> | <A>\n";
    in << "<A> -> 1<B> | 2<B> | 3<B> | 4<B> | 5<B> | 6<B> | 7<B> | 8<B> | 9<B> | 0 | 0.<C>\n";
    in << "<B> -> 0<B> | 1<B> | 2<B> | 3<B> | 4<B> | 5<B> | 6<B> | 7<B> | 8<B> | 9<B> | .<C> | #\n";
    in << "<C> -> 0<C> | 1<C> | 2<C> | 3<C> | 4<C> | 5<C> | 6<C> | 7<C> | 8<C> | 9<C> | #\n";
    in >> g;
    g.simplify();
    g.toCNF();

    string exp;
    getline(cin, exp);
    if (checkBadSpaces(exp)) {
        puts("INVALID");
        return 0;
    }
    exp = removeSpaces(exp);
    if (!g.CYKMembership(exp)) {
        puts("INVALID");
        return 0;
    }

    const string ans = calc(split(exp));
    if (ans == "INVALID") {
        puts("INVALID");
    } else {
        printf("%.2f", strtod(ans.data(), NULL));
    }
}
