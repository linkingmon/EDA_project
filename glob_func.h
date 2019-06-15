#ifndef GLOB_FUNC_H
#define GLOB_FUNC_H
#include "point.h"
// #define DEBUG
static int glob_color = 0;
void list_construct(point *&);
point *construct_new_poly(vector<point *> &);

// to construct a link
// 增加 point 的特殊性質
void list_construct(point *&root)
{
    static fstream lfout("list_construct.txt", fstream::out);
    point *temp = root;
    long long minx, miny, maxx, maxy;
    minx = miny = 9223372036854775807;
    maxx = maxy = -9223372036854775808;
    for (int i = 0; i < root->len; ++i)
    {
        lfout << "OLD" << *(temp) << *(temp->next) << temp->verti << ' ' << temp->dir << endl;
        if (temp->next->x == temp->x)
        { // 判斷是否是垂直線
            temp->verti = true;
        }
        else
        {
            temp->verti = false;
        }
        lfout << "NEW" << *(temp) << *(temp->next) << temp->verti << ' ' << temp->dir << endl;
        if (temp->prev->x != temp->next->x && temp->prev->y != temp->next->y) // 判斷是哪種角
        {                                                                     //是角
            if ((temp->prev->x - temp->next->x < 0) != (temp->prev->y - temp->next->y < 0))
            { //一正一負 負斜率 左上右下角
                temp->angle = 1;
            }
            else
            {
                temp->angle = 0;
            }
        }
        else
        {
            temp->angle = 2;
        }
        point *s_next = temp->next;
        if (temp->verti) // 找 straight next
        {
            while (s_next->next->x == temp->x)
                s_next = s_next->next;
            if (s_next->y > temp->y)
            {
                temp->dir = true;
            }
            else
                temp->dir = false;
        }
        else
        {
            while (s_next->next->y == temp->y)
                s_next = s_next->next;
            if (s_next->x > temp->x)
            {
                temp->dir = true;
            }
            else
                temp->dir = false;
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
        lfout << "END" << *(temp) << *(temp->next) << temp->verti << ' ' << temp->dir << endl;
        temp = temp->next;
    }
    if (root->verti == root->prev->verti)
    {
        point *p = root->next;
        root->prev->next = root->next;
        root->next->prev = root->prev;
        root->prev->s_next = root->next;
        root->next->minx = minx;
        root->next->miny = miny;
        root->next->maxx = maxx;
        root->next->maxy = maxy;
        p->len = root->len - 1;
        delete root;
        root = p;
    }
    else
    {
        root->minx = minx;
        root->miny = miny;
        root->maxx = maxx;
        root->maxy = maxy;
    }
}

// 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
point *construct_new_poly(vector<point *> &point_stream)
{
    vector<int> vec;
    vec.reserve(point_stream.size()); // 0 水平 1 垂直 2 同一個點
    point_stream.push_back(point_stream[0]);

    for (unsigned int i = 0; i < point_stream.size() - 1; ++i)
    {
        if (point_stream[i]->x == point_stream[i + 1]->x)
        {
            if (point_stream[i]->y == point_stream[i + 1]->y)
                vec.push_back(2);
            else
                vec.push_back(1);
        }
        else
            vec.push_back(0);
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
    // cout << "LLLLLLLLLLLLLLLLLLLLListLLLLLLLLLLLLLL" << endl;
    // root->print_poly();
    list_construct(root);
    // root->print_poly();
    return root;
}
#endif