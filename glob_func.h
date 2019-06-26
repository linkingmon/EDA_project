#ifndef GLOB_FUNC_H
#define GLOB_FUNC_H
#include "point.h"
#include <climits>
#include "TimeMgr.h"

static int glob_color = 0;
static TimeMgr *Time = new TimeMgr();
void list_construct(point *&root);
// void list_construct(point *&root, int for_new_poly);
point *construct_new_poly(vector<point *> &);

// 增加 point 的特殊性質
void list_construct(point *&root)
{
    point *temp = root;
    long long minx, miny, maxx, maxy;
    minx = miny = LLONG_MAX;
    maxx = maxy = LLONG_MIN;
    for (int i = 0; i < root->len; ++i)
    {
        // 判斷是否是垂直線
        if (temp->next->x == temp->x)
        {
            temp->verti = true;
            temp->dir = (temp->next->y > temp->y);
        }
        else
        {
            temp->verti = false;
            temp->dir = (temp->next->x > temp->x);
        }
        // 判斷是哪種角
        if (temp->prev->x != temp->next->x && temp->prev->y != temp->next->y)
            temp->angle = ((temp->prev->x - temp->next->x < 0) != (temp->prev->y - temp->next->y < 0));
        // 設定dir
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
    // 如果root在邊的中間，把root移動到下一個並delete掉原root(前後接好)
    if (root->verti == root->prev->verti)
    {
        point *p = root->next;
        root->prev->next = root->next;
        root->next->prev = root->prev;
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
    // 設定 root
    point *p = root;
    for (unsigned int i = 0; i < root->len; ++i)
    {
        p->root = root;
        p = p->next;
    }
    root->iscolored = false;
    root->has_intersect = false;
    root->setcounterclockwise();
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
    for (unsigned int i = 0; i < point_stream_simple.size() - 1; ++i)
    {
        assert(point_stream_simple[i]->x != point_stream_simple[i + 1]->x || point_stream_simple[i]->y != point_stream_simple[i + 1]->y);
        if (point_stream_simple[i]->x == point_stream_simple[i + 1]->x && point_stream_simple[i]->y == point_stream_simple[i + 1]->y)
        {
            point_stream_simple.erase(point_stream_simple.begin() + i);
            --i;
        }
    }
    // 不是多邊形直接跳出
    if (point_stream_simple.size() < 4)
        return nullptr;
    if ((point_stream_simple[0]->x == point_stream_simple[1]->x) == (point_stream_simple[point_stream_simple.size() - 1]->x == point_stream_simple[0]->x))
    {
        point_stream_simple[0] = point_stream_simple[point_stream_simple.size() - 1];
        point_stream_simple.pop_back();
    }
    // Assert 沒有相同點出現
    for (unsigned int i = 0; i < point_stream_simple.size() - 1; ++i)
        assert(point_stream_simple[i]->x != point_stream_simple[i + 1]->x || point_stream_simple[i]->y != point_stream_simple[i + 1]->y);
    // 串起link list
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
    list_construct(root);
    return root;
}
#endif