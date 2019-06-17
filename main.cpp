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

inline bool read_operation(fstream &fin, point *&root, bool isclip);
void find_intersect(operation, operation);

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int number_id = 0;
    // for (unsigned int number = 'a'; number < 'a' + 25; ++number)
    // {
    //     string filename = string("special/case") + char(number) + string(".txt");
    string filename = string("OpenCase_1.txt");
    fstream fin(filename.c_str(), fstream::in);
    if (!fin)
    {
        cerr << filename << endl;
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

    littlemerge *LM = new littlemerge();
    for (unsigned int i = 0; i < operations.size(); ++i)
    {
        if (operations[i][0] == 'S')
        {
            SplitMgr *SM = new SplitMgr();
            LM->output("split/total4_S.out");
            if (operations[i][1] == 'H')
                SM->splitH(LM->get_list());
            else if (operations[i][1] == 'V')
                SM->splitH(LM->get_list());
            else
                SM->splitO(LM->get_list());
            SM->output_rect("split/split4_S.out");
            delete SM;
        }
        else
        {
            operation &oper = mapping[operations[i]];
            // if (i > 2)
            //     continue;
            for (unsigned int j = 0; j < oper.root_list.size(); ++j)
            {
                cout << operations[i] << ' ' << i << ' ' << j << endl;
                // LM->output(string("ALL") + char(j + 65) + ".txt", oper.root_list[j]);

                // if (j % 1000 == 0)
                // {
                //     cerr << operations[i] << ' ' << i << ' ' << j << endl;
                // LM->start_print();
                // }
                // char buffer[3];
                // if (i == 2 && j == 85)
                // LM->output("AllA.txt", oper.root_list[j]);
                // if (i == 2 && j == 86)
                // {
                //     LM->output("AllB.txt", oper.root_list[j]);
                //     LM->start_print();
                // }
                // assert(i != 3);
                // if (i == 3)
                // {
                //     itoa(j, buffer, 10);
                //     LM->output(string("BUG/All") + buffer + ".txt", oper.root_list[j]);
                // }
                // if (i == 3 && j == 13)
                // {
                //     //     assert(j != 46);
                //     // if (j == 45)
                //     LM->start_print();
                // }
                LM->insert(oper.root_list[j], operations[i][0] == 'M');
            }
            cerr << "total points: " << point_cnt << " & total intersects: " << intersect_cnt << endl;
            LM->print();
            // LM->output(string("result/Merge") + char(i + 49) + ".txt");
            // LM->output(string("result/Merge") + char(i + 49) + ".txt");
        }
    }
    // LM->print();
    // LM->output(string("special_res/res") + char(number) + string(".txt"));
    delete LM;
    // }
}

inline bool read_operation(fstream &fin, point *&root, bool isclip)
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
    // root->print_poly();
    return true;
}