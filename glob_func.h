#include "point.h"

static int glob_color = 0;
void list_construct(point *);
point *construct_new_poly(vector<point *> &);
vector<point *> little_merge(set<point *> &);
// void check_list(operation &, vector<point *> &);

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
    root->len = point_stream.size();
    // 壓縮一下直接走到s_next
    list_construct(root);
    return root;
}

vector<point *> little_merge(set<point *> &out_list)
{
    vector<point *> new_list;
    int cnt = 0;
    ++glob_color;
#ifdef DEBUG
    cout << "-----------merging-----------" << endl;
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