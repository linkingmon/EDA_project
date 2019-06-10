#include "point.h"
// #define DEBUG
static int glob_color = 0;
void list_construct(point *&);
point *construct_new_poly(vector<point *> &);
vector<point *> little_merge(set<point *> &);
// void check_list(operation &, vector<point *> &);

// to construct a link
// 增加 point 的特殊性質
void list_construct(point *&root)
{
    point *temp = root;
    long long minx, miny, maxx, maxy;
    minx = miny = 9223372036854775807;
    maxx = maxy = -9223372036854775808;
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
        if (temp->verti) // 找 straight next
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
        if (temp->x < minx)
            minx = temp->x;
        if (temp->y < miny)
            miny = temp->y;
        if (temp->x > maxx)
            maxx = temp->x;
        if (temp->y > maxy)
            maxy = temp->y;
        temp = temp->next;
    }
    root->minx = minx;
    root->miny = miny;
    root->maxx = maxx;
    root->maxy = maxy;
    if (root->verti == root->prev->verti)
    {
        point *p = root->next;
        root->prev->next = root->next;
        root->next->prev = root->prev;
        root->prev->s_next = root->next;
        p->len = root->len - 1;
        delete root;
        root = p;
    }
}

// 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
point *construct_new_poly(vector<point *> &point_stream)
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
    // for (unsigned int i = 0; i < point_stream.size(); ++i)
    // cout << "STREAM: " << *point_stream[i] << endl;
    vector<int> vec;
    vec.reserve(point_stream.size()); // 0 水平 1 垂直 2 同一個點
    point_stream.push_back(point_stream[0]);

    for (unsigned int i = 0; i < point_stream.size() - 1; ++i)
    {
        // cout << *point_stream[i];
        if (point_stream[i]->x == point_stream[i + 1]->x)
        {
            if (point_stream[i]->y == point_stream[i + 1]->y)
                vec[i] = 2;
            else
                vec[i] = 1;
        }
        else
            vec[i] = 0;
        // cout << " " << vec[i] << endl;
    }
    vector<point *> point_stream_simple;
    int cnt = 0;
    while (vec[cnt] == 2)
        ++cnt;
    int dir = vec[cnt];
    point_stream_simple.push_back(point_stream[cnt]);
    for (unsigned int i = cnt + 1; i < point_stream.size() - 1; ++i)
    {
        if (vec[i] == 2)
            continue;
        if (vec[i] != dir)
        {
            // cout << "PUSH " << *(point_stream[i]) << endl;
            point_stream_simple.push_back(point_stream[i]);
            dir = vec[i];
        }
    }

    point *root = new point(*(point_stream_simple[0]));
    point *p_prev = root;
    for (unsigned int i = 1; i < point_stream_simple.size(); ++i)
    {
        point *p = new point(*(point_stream_simple[i]));
        p_prev->next = p;
        p->prev = p_prev;
        p_prev = p;
    }
    p_prev->next = root;
    root->prev = p_prev;
    root->len = point_stream_simple.size();
    // 壓縮一下直接走到s_next
    list_construct(root);
    return root;
}

vector<point *> little_merge(set<point *> &out_list)
{
    set<point *>::iterator iter;
    // cout << "OUT LIST size " << out_list.size() << endl;
    // for (iter = out_list.begin(); iter != out_list.end(); ++iter)
    // {
    //     cout << **iter;
    // }
    vector<point *> new_list;
    int cnt = 0;
    ++glob_color;
#ifdef DEBUG
// cout << "-----------merging-----------" << endl;
#endif
    while (out_list.size() > 0)
    {
        vector<point *> poly;
        point *new_poly;
        set<point *>::iterator iter;
        iter = out_list.begin();
        new_poly = *iter;
        point *p = (*iter)->next;
        out_list.erase(*iter);
#ifdef DEBUG
        cout << "WALK " << *new_poly << endl;
#endif
        new_poly->pcolor = glob_color;
        poly.push_back(new_poly);
        while (p != new_poly)
        {
            ++cnt;
#ifdef DEBUG
            cout << "WALK " << *p << endl;
#endif
            p->pcolor = glob_color;
            poly.push_back(p);
            if (!p->ispoint())
            {
                if (!static_cast<intersect_point *>(p)->in)
                {
                    out_list.erase(p);
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
                p->pcolor = glob_color;
                poly.push_back(p);
                if (p == new_poly)
                {
                    break;
                }
                if (!static_cast<intersect_point *>(p)->in)
                {
                    out_list.erase(p);
                }
            }
        back:
#ifdef DEBUG
            cout << "WALK " << *p << endl;
#endif
            p->pcolor = glob_color;
            poly.push_back(p);
            p = p->next;
        }
        // 用新走出的形狀做出多邊形
        // 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
        new_list.push_back(construct_new_poly(poly));
    }
    return new_list;
}

// void check_list(operation &oper, vector<point *> &new_list)
// {
//     // cout << "new list size is " << new_list.size() << endl;
//     // 判斷有些多邊形跟其他是分開的，根本不用何在一起，要直接加進去；感覺有點慢?
//     for (unsigned int k = 0; k < oper.root_list.size(); ++k)
//     {
//         point *p = oper.root_list[k];
//         bool isout = true;
//         // cout << p->len << " " << *p << endl;
//         for (unsigned int z = 0; z < oper.root_list[k]->len; ++z)
//         {
//             if (p->pcolor == glob_color)
//             {
//                 isout = false;
//                 break;
//             }
//             // cout << p->pcolor << *p << endl;
//             p = p->next;
//         }
//         // cout << isout << endl;
//         if (isout)
//         {
//             new_list.push_back(oper.root_list[k]);
//         }
//         else
//         {
//             oper.root_list[k]->delete_poly();
//             delete oper.root_list[k];
//         }
//     }
//     oper.root_list = new_list;
// }