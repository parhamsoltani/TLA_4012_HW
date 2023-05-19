#include <bits/stdc++.h>
using namespace std;

vector<string> parseStr(string statesStr)
{
    vector<string> states;
    stringstream ss(statesStr.substr(1, statesStr.size() - 2));
    string state;
    while(getline(ss, state, ','))
    {
        states.push_back(state);
    }
    return states;
}

bool acc(string pt, string c_st, unordered_map<string, vector<string>> tr, vector<string> end){

    if(pt.size() == 0)
    {
        for(string fin : end)
        {
            if(fin == c_st)
            {
                return true;
            }
        }
    }

    if (pt=="$")
    {
        pt = "";
    }

    for(auto n_st : tr[c_st + pt[0]])
    {
        if(acc(pt.substr(1), n_st, tr, end))
        {
            return true;
        }
    }

    for(auto n_st : tr[c_st + "$"])
    {
        if(acc(pt, n_st, tr, end))
        {
            return true;
        }
    }

    return false;
}

int main()
{
    string st_str, l_str, f_st_str;
    cin >> st_str >> l_str >> f_st_str;

    int tr_rule_c;
    cin >> tr_rule_c;


    unordered_map<string, vector<string>> tr_m;
    for(int i = 0; i < tr_rule_c; i++)
    {
        string trn;
        cin >> trn;

        stringstream ss(trn);
        vector<string> prt;
        string pr;
        int j = 0;
        while(getline(ss, pr, ','))
        {
            prt.push_back(pr);
            j++;
        }
        tr_m[prt[0] + prt[1]].push_back(prt[2]);
    }

    string pt;
    cin >> pt;

    string ans = acc(pt, parseStr(st_str)[0], tr_m, parseStr(f_st_str)) ? "Accepted" : "Rejected";
    cout << ans << endl;
    return 0;
}

