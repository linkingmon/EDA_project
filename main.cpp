#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <set>
#include <assert.h>
#include <map>
#include "operation.h"
#define DEBUG
using namespace std;

inline bool read_operation(fstream &, point **);
void find_intersect(vector<point *> total_root, vector<point *> root);
void list_construct(point *);
point *construct_new_poly(vector<point *>);

int main()
{
    string ang = "\\/X";
    //clock_t t = clock();
    // can choose file: sample_in.txt / OpenCase_1.txt / OpenCase_2.txt
    string filename = "sample_in.txt";
    fstream fin(filename.c_str(), fstream::in);

    // read operation
    vector<string> operations;       // store opertaion strings
    map<string, operation> mapping;  // map string to its operation
    vector<point *> total_root_list; // store current overall polygon list
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
// print operation
// 順序問題????? 要把list存在operation之下?????
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
        operation oper = mapping.find(s)->second;
        fin >> s;
        // read points and construct list
        point *root;
        while (read_operation(fin, &root))
        {
            oper.root_list.push_back(root);
            list_construct(root); // 插入一些點的特殊性質：是否垂直、角的方向
#ifdef DEBUG
            point *p = root;
            cout << "POLYGON" << oper.root_list.size() - 1 << endl;
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
#endif
        vector<point *> new_list;
        int cnt = 0;
#ifdef DEBUG
        cout << "-----------merging-----------" << endl;
#endif
        while (oper.out_list.size() > 0)
        {
            vector<point *> poly;
            point *new_poly;
            set<point *>::iterator iter;
            iter = oper.out_list.begin();
            new_poly = *iter;
            point *p = (*iter)->next;
            oper.out_list.erase(*iter);
#ifdef DEBUG
            cout << "WALK " << *new_poly << endl;
#endif
            poly.push_back(new_poly);
            while (p != new_poly)
            {
                ++cnt;
#ifdef DEBUG
                cout << "WALK " << *p << endl;
#endif
                poly.push_back(p);
                if (!p->ispoint())
                {
                    if (!static_cast<intersect_point *>(p)->in)
                    {
                        oper.out_list.erase(p);
                    }
                    if (!static_cast<intersect_point *>(p)->tran)
                    {
                        // cout << "No tran in intersect" << *p << endl;
                        goto back;
                    }
                    p = static_cast<intersect_point *>(p)->cross_point;
#ifdef DEBUG
                    cout << "cross " << *p << endl;
#endif
                    poly.push_back(p);
                    if (p == new_poly)
                    {
                        break;
                    }
                    if (!static_cast<intersect_point *>(p)->in)
                    {
                        oper.out_list.erase(p);
                    }
                }
            back:
#ifdef DEBUG
                cout << "WALK " << *p << endl;
#endif
                poly.push_back(p);
                p = p->next;
            }
            // 用新走出的形狀做出多邊形
            // 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
            new_list.push_back(construct_new_poly(poly));
        }
        for (unsigned int k = 0; k < oper.root_list.size(); ++k)
        {
            oper.root_list[k]->delete_poly();
            delete oper.root_list[k];
        }
        oper.root_list = new_list;
#ifdef DEBUG
        cout << "----memory leak checking-----" << endl;
        cout << "Total points: " << point_cnt << " & intersects: " << intersect_cnt << endl;
        cout << "Total polygons after merging in operation: " << oper.root_list.size() << endl;
#endif
    }
}
// find cross point in two vector of POLYGON
// 找兩個 root list 的所有焦點
// void find_intersect(vector<point *> total_root, vector<point *> root)
// {
//     for (int i = 0; i < total_root.size(); i++)
//     {
//         for (int j = 0; j < root.size(); j++)
//         {
//             find_intersect(total_root[i], root[j]);
//         }
//     }
// }

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

// to construct a link
// 增加 point 的特殊性質
void list_construct(point *root)
{
    point *temp = root;
    for (int i = 0; i < root->len; ++i)
    {
        if (temp->next->x == temp->x) // 判斷是否是垂直線
            temp->verti = true;
        if (temp->prev->x != temp->next->x && temp->prev->y != temp->next->y) // 判斷是哪種角
        {                                                                     //是角
            if ((temp->prev->x - temp->next->x < 0) ^ (temp->prev->y - temp->next->y < 0))
            { //一正一負 負斜率 左上右下角
                temp->angle = 1;
            }
            else
            {
                temp->angle = 0;
            }
        }
        else
            temp->angle = 2;
        point *s_next = temp->next;
        if (temp->verti)
        {
            while (s_next->next->x == temp->x)
                s_next = s_next->next;
            if (s_next->y > temp->y)
            {
                temp->dir = true;
            }
        }
        else
        {
            while (s_next->next->y == temp->y)
                s_next = s_next->next;
            if (s_next->x > temp->x)
            {
                temp->dir = true;
            }
        }
        temp->s_next = s_next;
        temp = temp->next;
    }
}

// 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
point *construct_new_poly(vector<point *> point_stream)
{
    //     point *root = new point(point_stream[0]);
    // #ifdef DEBUG
    //     if (point_stream.size() < 4)
    //     {
    //         cout << "POLY has only " << point_stream.size() << " vertices" << endl;
    //         assert(0);
    //     }
    // #endif
    //     point *p = point_stream[0];
    //     point *p_next = point_stream[1];
    //     for (unsigned int i = 2; i < point_stream.size(); ++i)
    //     {
    //         p_next = point_stream[i];
    //     }
    point *root = new point(*(point_stream[0]));
    point *p_prev = root;
    for (unsigned int i = 1; i < point_stream.size(); ++i)
    {
        point *p = new point(*(point_stream[i]));
        p_prev->next = p;
        p->prev = p_prev;
        p_prev = p;
    }
    p_prev->next = root;
    root->prev = p_prev;
    // 壓縮一下直接走到s_next
    return root;
}