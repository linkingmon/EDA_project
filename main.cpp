#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <set>
#include <assert.h>
#include <map>
#include "operation.h"
#include "littlemerge.h"
#include "SplitMgr.h"

#define DEBUG
using namespace std;

inline bool read_operation(istream &fin, point *&root, bool isclip);
void find_intersect(operation, operation);

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int number_id = 0;
    Time->tic();
    istream &fin = cin;
    if (!fin)
    {
        cerr << "Error reading file" << endl;
        return 0;
    }
    vector<string> operations;      // store opertaion strings 按照順序存操作的次序
    map<string, operation> mapping; // map string to its operation
    string s;
    fin >> s;

    while (1)
    {
        fin >> s;
        if (s == ";")
            break;
        operations.push_back(s);
        if (s != "SV" && s != "SH" && s != "SO")
        {
            operation oper(s);
            mapping.insert(make_pair(s, oper));
        }
    }

    for (unsigned int i = 0; i < mapping.size(); ++i)
    {
        fin >> s >> s >> s;
        operation &oper = mapping[s];
        bool isclip = (s[0] == 'C'); // 如果是 Clip 就存順時針
        fin >> s;
        point *root;
        while (read_operation(fin, root, isclip))
            oper.root_list.push_back(root);
    }
    Time->toc("Read file");
    cerr << "start operation" << endl;
    littlemerge *LM = new littlemerge();
    for (unsigned int i = 0; i < operations.size(); ++i)
    {
        if (operations[i][0] == 'S')
        {
            cerr << "Split start" << endl;
            Time->tic();
            SplitMgr *SM = new SplitMgr();
            if (operations[i][1] == 'H')
                SM->splitH(LM->get_list());
            else if (operations[i][1] == 'V')
                SM->splitV(LM->get_list());
            else
                SM->splitO(LM->get_list());
            SM->output_rect();
            delete SM;
            Time->toc("Split");
        }
        else
        {
            operation &oper = mapping[operations[i]];
            char buffer[3];
            for (unsigned int j = 0; j < oper.root_list.size(); ++j)
            {
                if (j % 1000 == 0)
                    cerr << operations[i] << ' ' << i << ' ' << j << endl;
                LM->insert(oper.root_list[j], operations[i][0] == 'M');
            }
        }
    }
    Time->print();
    LM->memory_check();
    delete LM;
}

inline bool read_operation(istream &fin, point *&root, bool isclip)
{
    // read points and construct polygon
    string s, s2;

    // read dummy POLYGON
    fin >> s;
    if (s == "END" && fin >> s)
        return false;

    // read first 2 dummy int
    // 第一個座標在最後有可能不會給，要先存起來
    fin >> s >> s2;
    long long first_x = (long long)(atoi(s.c_str()));
    long long first_y = (long long)(atoi(s2.c_str()));

    // read root
    fin >> s >> s2;
    point *p;
    point *prev = new point((long long)(atoi(s.c_str())), (long long)(atoi(s2.c_str())));
    root = prev;
    int cnt = 1;

    // area for detecting if clockwise
    double area = 0;
    while (fin >> s && s != ";" && fin >> s2)
    {
        p = new point((long long)(atoi(s.c_str())), (long long)(atoi(s2.c_str())));
        p->prev = prev;
        prev->next = p;
        area += prev->area(p);
        prev = p;
        ++cnt;
    }
    if (p->x != first_x || p->y != first_y)
    {
        ++cnt;
        p = new point(first_x, first_y);
        p->prev = prev;
        prev->next = p;
        area += prev->area(p);
        prev = p;
    }
    p->next = root;
    root->prev = p;
    root->len = cnt;
    area += p->area(root);

    // see if it is clock wise or counter clockwise
    // cout << ((area > 0) ? "Counter-clockwise" : "Clockwise") << endl;

    // if it is clockwise, reverse it
    if (!isclip && area < 0)
    {
        point *p = root;
        for (int i = 0; i < root->len; ++i)
        {
            p->swap_dir();
            p = p->prev;
        }
    }
    else if (isclip && area > 0)
    {
        point *p = root;
        for (int i = 0; i < root->len; ++i)
        {
            p->swap_dir();
            p = p->prev;
        }
    }
    list_construct(root);
    return true;
}