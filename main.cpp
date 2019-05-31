#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include "point.h"
#include <set>
#include <assert.h>
#define DEBUG
using namespace std;

inline bool read_operation(fstream &, point **);
void find_intersect(vector<point *> total_root, vector<point *> root);
void find_intersect(point *a, point *b);
void find_cross(point *, point *);
void list_construct(point *);
void verti_construct(point *);
void add_intersect(point *a, point *b);
bool inside_edge(long long x1, long long x2, long long y);
void two_way_intersect(intersect_point *, intersect_point *);
void new_intersect(point *a, point *b, long long x, long long y);
bool inside_region(point *a, point *b, long long x, long long y);
void insert_intersect(vector<point *>);
void in_out_cross(point *, point *);
bool outside_poly(point *, point *);

int main()
{
    string ang = "\\/X";
    //clock_t t = clock();
    // can choose file: sample_in.txt / OpenCase_1.txt / OpenCase_2.txt
    string filename = "sample_in.txt";
    fstream fin(filename.c_str(), fstream::in);

    // read operation
    vector<string> operation;        // store opertaion strings
    vector<point *> total_root_list; // store current overall polygon list
    string s;
    fin >> s;
    while (1)
    {
        fin >> s;
        if (s == ";")
            break;
        operation.push_back(s);
    }

// print operation
// 順序問題????? 要把list存在operation之下?????
#ifdef DEBUG
    cout << "All operations: ";
    for (int i = 0; i < operation.size(); ++i)
        cout << operation[i] << " ";
    cout << endl;
#endif

    for (int i = 0; i < operation.size(); ++i)
    {
#ifdef DEBUG
        cout << "=============================" << endl;
#endif
        if (operation[i] == "SV" || operation[i] == "SH" || operation[i] == "SO") // 要修正
        {
#ifdef DEBUG
            cout << "Start splitting" << endl;
#endif
            break;
        }
        fin >> s >> s >> s;
#ifdef DEBUG
        cout << "Constructing list of operation: " << s << endl;
#endif
        fin >> s;
        // read points and construct list
        point *root;
        vector<point *> root_list; // 存該 operation 下的所有多邊形
        while (read_operation(fin, &root))
        {
            root_list.push_back(root);
            list_construct(root); // 插入一些點的特殊性質：是否垂直、角的方向
#ifdef DEBUG
            point *p = root;
            cout << "POLYGON" << root_list.size() - 1 << endl;
            for (int i = 0; i < root->len; ++i)
            {
                cout << *p << " DIR" << (p->verti ? '|' : '-') << " Angle" << ang[p->angle] << " Snext" << *(p->s_next) << endl;
                p = p->next;
            }
#endif
        }
        // 對每個 operation 的 list 都做 Merge
        // 從第一個開始到最後一個 每個都跟他後面的做找交點
        // 找完的交點存在 vector<point*> intersection裡面
        for (int i = 0; i < root_list.size(); ++i)
        {
            for (int j = i + 1; j < root_list.size(); ++j)
            {
                find_intersect(root_list[i], root_list[j]);
            }
        }
        insert_intersect(root_list);
#ifdef DEBUG
        cout << "----------intersect----------" << endl;
        for (int j = 0; j < root_list.size(); ++j)
        {
            point *p = root_list[j];
            cout << "POLYGON" << j << endl;
            for (int i = 0; i < root_list[j]->len; ++i)
            {
                p->print();
                p = p->next;
            }
        }
#endif
    }
}
// find cross point in two vector of POLYGON
// 找兩個 root list 的所有焦點
void find_intersect(vector<point *> total_root, vector<point *> root)
{
    for (int i = 0; i < total_root.size(); i++)
    {
        for (int j = 0; j < root.size(); j++)
        {
            find_intersect(total_root[i], root[j]);
        }
    }
}
// find cross point in two POLYGON
// 找兩個 多邊形 的交點
void find_intersect(point *a, point *b)
{
    point *p_k = a;
    point *p_l = b;
    // 每兩個頂點都做 找交點
    for (int k = 0; k < a->len; k++)
    {
        for (int l = 0; l < b->len; l++)
        {
            find_cross(p_k, p_l);
            p_l = p_l->next;
        }
        p_k = p_k->next;
    }
    in_out_cross(a, b);
    in_out_cross(b, a);
}

// cross 來 merge root
// 先判斷 root 是不是在 cross 外面
// root 如果在 cross 的邊上就算是在裡面
void in_out_cross(point *root, point *cross)
{
    bool outward = outside_poly(root, cross);
#ifdef DEBUG
    static bool pr = true;
    if (pr)
    {
        cout << "--------detect in out--------" << endl;
        pr = false;
    }
    cout << "detect that " << *root << " is " << (outward ? "outside" : " inside") << " of POLY" << *cross << endl;
#endif
    point *p = root;
    for (int i = 0; i < root->len; i++)
    {
        if (p->intersection.size() > 0)
        {
            p->sort_intersection();
            for (unsigned int i = 0; i < p->intersection.size(); ++i)
            {
                static_cast<intersect_point *>(p->intersection[i])->in = outward;
                outward = !outward;
            }
        }
        p = p->next;
    }
}

bool outside_poly(point *root, point *cross)
{
    // minx-1 一定在外面, 設定一個在外面的點P(minx-1,XX) 這個點在該點左上角
    // 找 root 和 P 的連線和 cross多邊形 有幾個交點
    // 分別對每一條邊找
    // 怎麼找?
    // (1) 如果是水平線 水平線y小於該點就不用考慮
    // (2) 如果是水平線 水平線y等於該點 判斷該點如果在上面就是
    // (3) 如果是水平線 水平線y大於該點 先往上走到那個y 然後往左走同樣的x 如果有碰到線就是有交點
    //     如果碰到線的"尾"端點 判斷他是哪一種角
    //     如果是左上右下角或不是角 就是+1 ； 如果是右上左下角就+0
    // (4) 如果是垂直線 垂直線x大於該點就不用考慮
    // (5) 如果是垂直線 垂直線x等於該點 判斷該點如果在上面就是
    // (6) 如果是垂直線 垂直線x小於該點 先往左走到那個x 然後往上走同樣的y 如果有碰到線就是有交點
    //     如果碰到線的"尾"端點 判斷他是哪一種角
    //     如果是左上右下角或不是角 就是+1 ； 如果是右上左下角就+0
    point *p = cross;
    int cross_cnt = 0;
    for (unsigned int i = 0; i < cross->len; ++i)
    {
        if (p->verti)
        {
            if (root->x > p->x)
            { // (6)
                long long inter_y = root->y + (root->x - p->x);
                if ((p->y < inter_y) && (inter_y < p->next->y) || (p->y > inter_y) && (inter_y > p->next->y))
                    cross_cnt += 1;
                else if (inter_y == p->y && p->angle != 1)
                    cross_cnt += 1;
            }
            else if (root->x == p->x)
                if ((root->y >= p->y) && (root->y <= p->s_next->y) || (root->y <= p->y) && (root->y >= p->s_next->y))
                    return false;
        }
        else
        {
            if (root->y < p->y)
            { // (3)
                long long inter_x = root->x - (p->y - root->y);
                if ((p->x < inter_x) && (inter_x < p->next->x) || (p->x > inter_x) && (inter_x > p->next->x))
                    cross_cnt += 1;
                else if (inter_x == p->x && p->angle != 1)
                    cross_cnt += 1;
            }
            else if (root->y == p->y)
                if ((root->x >= p->x) && (root->x <= p->s_next->x) || (root->x <= p->x) && (root->x >= p->s_next->x))
                    return false;
        }
        p = p->next;
    }
    if (cross_cnt % 2)
        return false;
    else
        return true;
}
//insert intersect in one POLYGON
void insert_intersect(vector<point *> list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        point *a = list[i];
        int len = 0;
        for (int j = 0; j < list[i]->len; ++j)
        {
            a->connect();
            len += a->intersection.size();
            a = a->next;
        }
        list[i]->len += len;
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
    fin >> s >> s;

    // read root
    fin >> s >> s2;
    point *p;
    p = new point();
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

// given two point a, b ; find cross point between a, a->next ,b ,b->bext
void find_cross(point *a, point *b)
{
    long long x, y;
    bool verti_a = a->verti;
    bool verti_b = b->verti;
    point *a_next = a->next;
    point *b_next = b->next;

    // if(a_next->x == a->x)
    //     verti_a = true;

    // if(b_next->x == b->x)
    //     verti_b = true;

    if (verti_a != verti_b)
    {
        x = verti_a ? a->x : b->x;
        y = verti_b ? a->y : b->y;
        if (inside_region(a, b, x, y))
        {
            // cout << "before intersect" << endl;
            // a->print();
            // b->print();
            new_intersect(a, b, x, y);
            // cout << "after intersect" << endl;
            // a->print();
            // b->print();
        }
        // new_intersect(a, b, x, y);
    }
}

// add intersect b after point a
void add_intersect(point *a, point *b)
{
    a->intersection.push_back(b);
}

// check whether x is inside x1 and x2 or not
bool inside_edge(long long x1, long long x2, long long x)
{
    if (x >= x1 && x <= x2)
    {
        return true;
    }
    else if (x <= x1 && x >= x2)
    {
        return true;
    }
    else
        return false;
}

// to check (x,y) is inside the region a , a->next , b , b->next
// if (x,y) is on the (a->next and b->next) return false
bool inside_region(point *a, point *b, long long x, long long y)
{
    if (a->verti)
    {
        if (y == a->next->y && x == b->next->x)
            return false;
        bool b_b = inside_edge(b->x, b->next->x, x);
        bool b_c = inside_edge(a->y, a->next->y, y);
        return (b_b && b_c);
    }
    else
    {
        if (x == a->next->x && y == b->next->y)
            return false;
        bool b_a = inside_edge(a->x, a->next->x, x);
        bool b_d = inside_edge(b->y, b->next->y, y);
        return (b_a && b_d);
    }
}

// two the same coordinate intersect point connect other
void two_way_intersect(intersect_point *a, intersect_point *b)
{
    a->cross_point = b;
    b->cross_point = a;
}
// the new intersect (x,y) is right after point a and point b
void new_intersect(point *a, point *b, long long x, long long y)
{
    intersect_point *insert_a = new intersect_point(x, y);
    intersect_point *insert_b = new intersect_point(x, y);
    add_intersect(a, insert_a);
    add_intersect(b, insert_b);
    two_way_intersect(insert_a, insert_b);
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
            while (s_next->next->x == temp->x)
                s_next = s_next->next;
        else
            while (s_next->next->y == temp->y)
                s_next = s_next->next;
        temp->s_next = s_next;
        temp = temp->next;
    }
}
