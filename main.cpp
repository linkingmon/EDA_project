#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <set>
#include <assert.h>
#include <map>
#include "operation.h"
#define DEBUG
using namespace std;

inline bool read_operation(fstream &, point **);
void find_intersect(operation, operation);
void output_result(const vector<point *> &, string);

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    string ang = "\\/X";
    // can choose file: sample_in.txt / OpenCase_1.txt / OpenCase_2.txt
    // string filename = "sample_in.txt";
    // string filename = "test_merge.txt";
    // string filename = "bug1.txt";
    string filename = "OpenCase_1.txt";
    // string filename = "poly_with_hole.txt";
    // string filename = "sample_in.txt";
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
        while (read_operation(fin, &root))
        {
            list_construct(root); // 插入一些點的特殊性質：是否垂直、角的方向
            oper.root_list.push_back(root);
            // cout << "ROOTT" << *root << " " << root->len << " " << *(root->next) << endl;
#ifdef DEBUG
            point *p = root;
            cout << "POLYGON" << oper.root_list.size() - 1 << endl;
            // cout << "Range: (" << root->minx << ',' << root->miny << ") to (" << root->maxx << ',' << root->maxy << ")" << endl;
            for (int k = 0; k < root->len; ++k)
            {
                cout << *p << " DIR" << (p->verti ? '|' : '-') << " Angle" << ang[p->angle] << " Snext" << *(p->s_next) << endl;
                p = p->next;
            }
#endif
        }
        // 對每個 operation 的 list 都做 Merge
        // 從第一個開始到最後一個 每個都跟他後面的做找交點
        // 找完的交點存在 vector<point*> intersection裡面
        oper.find_intersect();
        oper.insert_intersect();
#ifdef DEBUG
        cout << "------------result-----------" << endl;
        for (int j = 0; j < oper.root_list.size(); ++j)
        {
            point *p = oper.root_list[j];
            cout << "POLYGON" << j << endl;
            for (int i = 0; i < oper.root_list[j]->len; ++i)
            {
                p->print();
                p = p->next;
            }
        }
#endif
        // Merge per operation
#ifdef DEBUG
        cout << "----------operation----------" << endl;
        cout << "There are " << oper.out_list.size() << " out intersects" << endl;
        set<point *>::iterator iter;
        for (iter = oper.out_list.begin(); iter != oper.out_list.end(); ++iter)
            cout << **iter << endl;
#endif
#ifdef DEBUG
        cout << "----memory leak checking-----" << endl;
        cout << "Total points: " << point_cnt << " & intersects: " << intersect_cnt << endl;
        cout << "-----------------------------" << endl;
#endif
        // 小的 merge(每個operation裡面自己merge)
        vector<point *> new_list = ::little_merge(oper.out_list);
        oper.check_list(new_list);
        oper.root_list = new_list;
        // new_list = little_merge(oper.out_list);
        // check_list(oper, new_list);
        // #ifdef DEBUG
        cout << "There are " << oper.root_list.size() << " polygons after little merging" << endl;
        for (unsigned int jjj = 0; jjj < oper.root_list.size(); ++jjj)
        {
            point *p = oper.root_list[jjj];
            for (int k = 0; k < oper.root_list[jjj]->len; ++k)
            {
                cout << *p << " DIR" << (p->verti ? '|' : '-') << " Angle" << ang[p->angle] << " Snext" << *(p->s_next) << endl;
                p = p->next;
            }
        }
        cout << "----memory leak checking-----" << endl;
        cout << "Total points: " << point_cnt << " & intersects: " << intersect_cnt << endl;
        cout << "-----------------------------" << endl;
        cout << "Total polygons after merging in operation: " << oper.root_list.size() << endl;
        char c = i + 49;
        output_result(oper.root_list, string("result/Merge") + c + ".txt");
        // #endif
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
    cout << "==========================================================" << endl;
    cout << "==========================================================" << endl;
    cout << "===================Merge / Clip / Split===================" << endl;
    cout << "==========================================================" << endl;
    cout << "==========================================================" << endl;
#endif
    // 這邊開始做 Merge Clip Split
    // operation都存在Mapping裡面
    // vector<point *> total_root_list; // store current overall polygon list
    operation total; // 儲存完成surrent operation的結果
    // total.root_list = total_root_list;
    for (unsigned int i = 0; i < operations.size(); ++i)
    {
        if (operations[i][0] == 'M')
        {
            operation &oper = mapping[operations[i]];
            // cout << "OPRRRRRRRRRR" << endl;
            for (int h = 0; h < oper.root_list.size(); ++h)
            {
                point *p = oper.root_list[h];
                for (int k = 0; k < oper.root_list[h]->len; ++k)
                {
                    cout << *p << " DIR" << (p->verti ? '|' : '-') << " Angle" << ang[p->angle] << " Snext" << *(p->s_next) << endl;
                    p = p->next;
                }
            }
            total += oper;
#ifdef DEBUG
            cout << "There are " << total.root_list.size() << " polygons after operator " << operations[i] << endl
                 << endl;
            for (unsigned int jjj = 0; jjj < total.root_list.size(); ++jjj)
            {
                cout << "POLYGON" << jjj << endl;
                total.root_list[jjj]->print_poly();
            }
            cout << "----memory leak checking-----" << endl;
            cout << "Total points: " << point_cnt << " & intersects: " << intersect_cnt << endl;
            cout << "-----------------------------" << endl;
            char c = i + 49;
            output_result(total.root_list, string("result/Total") + c + ".txt");
#endif
        }
        else if (operations[i][0] == 'C')
        {
            operation &oper = mapping[operations[i]];
            total -= oper;
        }
        else
        { // operations[i] = "SV" "SH" "SO"
            cout << "Splitting" << endl;
            output_result(total.root_list, string("result/Merge_all.txt"));
            // Interval_Mgr IM;
            // IM.build_interval(total.root_list[0], 1);
            // IM.split_polygon();
            // if(operations[i][1] == 'V')
            // else if(operations[i][1] == 'H')
            // else
        }
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