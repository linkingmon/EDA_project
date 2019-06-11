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
#define DEBUG
using namespace std;

inline bool read_operation(fstream &, point **);
void find_intersect(operation, operation);
void output_result(const vector<point *> &, string);

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    string filename = "OpenCase_1.txt";
    fstream fin(filename.c_str(), fstream::in);
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
#ifdef DEBUG
    cout << "All operations: ";
    for (int i = 0; i < operations.size(); ++i)
        cout << operations[i] << " ";
    cout << endl;
    map<string, operation>::iterator iter;
    for (iter = mapping.begin(); iter != mapping.end(); ++iter)
        cout << "Have construct operation class with name " << iter->first << endl;
#endif

    for (int i = 0; i < mapping.size(); ++i)
    {
#ifdef DEBUG
        cout << "=============================" << endl;
#endif
        fin >> s >> s >> s;
#ifdef DEBUG
        cout << "Constructing list of operation: " << s << endl;
#endif
        operation &oper = mapping[s];
        fin >> s;
        // read points and construct list
        point *root;
        int cnt = 0;
        littlemerge *LM = new littlemerge;
        while (read_operation(fin, &root))
        {
            cout << cnt++ << endl;
            LM->insert(root);
            if (cnt == 232)
                LM->output("TEST.txt");
        }
        LM->print();
        LM->output(string("result/Merge") + char(i + 49) + ".txt");
        LM->clear();
    }
}

inline bool read_operation(fstream &fin, point **root)
{
    // read points and construct polygon
    string s, s2;

    // read dummy POLYGON
    fin >> s;
    if (s == "END" && fin >> s)
        return false;

    // read first 2 dummy int
    // 文喬老大說有可能最後不會出現
    fin >> s >> s;

    // read root
    fin >> s >> s2;
    point *p;
    point *prev = new point((long long)(atoi(s.c_str())), (long long)(atoi(s2.c_str())));
    *root = prev;
    int cnt = 1;

    // area for detecting if clockwise
    float area = 0;
    while (fin >> s && s != ";" && fin >> s2)
    {
        p = new point((long long)(atoi(s.c_str())), (long long)(atoi(s2.c_str())));
        p->prev = prev;
        // prev->straight_next = p;
        prev->next = p;
        area += prev->area(p);
        prev = p;
        ++cnt;
    }
    // p->straight_next = *root;
    p->next = *root;
    (*root)->prev = p;
    (*root)->len = cnt;
    area += p->area(*root);

    // see if it is clock wise or counter clockwise
    // cout << ((area > 0) ? "Counter-clockwise" : "Clockwise") << endl;

    // if it is clockwise, reverse it
    if (area < 0)
    {
        point *p = *root;
        for (int i = 0; i < (*root)->len; ++i)
        {
            p->swap_dir();
            p = p->prev;
        }
    }
    return true;
}

// 輸出txt黨 給python吃 看對不對
void output_result(const vector<point *> &root_list, string filename)
{
    fstream fout(filename.c_str(), fstream::out);
    fout << "OPERATION M1 ;\n\n";
    fout << "DATA MERGE M1 ;" << endl;
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *p = root_list[i];
        fout << "POLYGON ";
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            fout << p->x << " " << p->y << " ";
            p = p->next;
        }
        fout << root_list[i]->x << " " << root_list[i]->y << " ;" << endl;
    }
    fout << "END DATA" << endl;
}