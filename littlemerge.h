#ifndef LITTLEMERGE_H
#define LITTLEMERGE_H
#include "glob_func.h"
#include "point.h"

class littlemerge
{
public:
    littlemerge();
    ~littlemerge();
    void insert(point *&);
    void merge(point *&);
    bool find_intersect(point *&);
    int find_intersect(point *, point *);
    bool outside_poly(point *, point *);
    void find_cross(point *, point *);
    void insert_intersect(point *&);
    void add_intersect(point *a, point *b);
    bool inside_edge(long long x1, long long x2, long long y);
    void two_way_intersect(intersect_point *, intersect_point *);
    void new_intersect(point *a, point *b, long long x, long long y);
    bool inside_region(point *a, point *b, long long x, long long y);
    bool in_out_cross(point *, point *);
    void check_list(vector<point *> &, point *&);
    void clear()
    {
        root_list.clear();
        out_list.clear();
        out_list_buf.clear();
    };
    void output(string);
    void print();

private:
    vector<point *> root_list;
    set<point *> out_list;
    vector<point *> out_list_buf;
};

littlemerge::littlemerge()
{
}
littlemerge::~littlemerge()
{
}
void littlemerge::output(string filename)
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
void littlemerge::print()
{
    cout << "OPERATION M1 ;\n\n";
    cout << "DATA MERGE M1 ;" << endl;
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *p = root_list[i];
        cout << "POLYGON ";
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            cout << p->x << " " << p->y << " ";
            p = p->next;
        }
        cout << root_list[i]->x << " " << root_list[i]->y << " ;" << endl;
    }
    cout << "END DATA" << endl;
}
void littlemerge::insert(point *&root)
{
    ::list_construct(root);
    // root->print_poly();
    // root_list.push_back(root);
    out_list.clear();
    out_list_buf.clear();
    merge(root);
}

void littlemerge::merge(point *&root)
{
    if (root_list.size() == 0)
    {
        root_list.push_back(root);
        return;
    }
    bool del = find_intersect(root);
    if (!del)
        return;
    insert_intersect(root);
    vector<point *> new_list;
    int cnt = 0;
    ++glob_color;
    while (out_list.size() > 0)
    {
        vector<point *> poly;
        point *new_poly;
        set<point *>::iterator iter;
        iter = out_list.begin();
        new_poly = *iter;
        point *p = (*iter)->next;
        out_list.erase(*iter);
        // cout << "WALK " << *new_poly << endl;
        new_poly->pcolor = glob_color;
        poly.push_back(new_poly);
        while (p != new_poly)
        {
            ++cnt;
            // cout << "WALK " << *p << endl;
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
                    goto back;
                }
                p = static_cast<intersect_point *>(p)->cross_point;
                // cout << "cross " << *p << endl;
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
            // cout << "WALK " << *p << endl;
            p->pcolor = glob_color;
            poly.push_back(p);
            p = p->next;
        }
        // 用新走出的形狀做出多邊形
        // 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
        new_list.push_back(construct_new_poly(poly));
    }
    check_list(new_list, root);
    root_list = new_list;
}

bool littlemerge::find_intersect(point *&root)
{
    for (int i = 0; i < root_list.size(); ++i)
    {
        int state = find_intersect(root_list[i], root);
        switch (state)
        {
        case 1:
            root_list[i] = root_list[root_list.size() - 1];
            root_list.pop_back();
            --i;
            continue;
            break;
        case 2:
            return false;
            break;
        default:
            continue;
            break;
        }
    }
    return true;
}

int littlemerge::find_intersect(point *a, point *b)
{
    out_list_buf.clear();
    if (b->minx > a->maxx || b->miny > a->maxy || b->maxx < a->minx || b->maxy < a->miny)
        return 0;
    point *p_k = a;
    point *p_l = b;
    ++glob_color;
    for (int k = 0; k < a->len; k++)
    {
        for (int l = 0; l < b->len; l++)
        {
            find_cross(p_k, p_l);
            p_l = p_l->next;
        }
        p_k = p_k->next;
    }
    int return_num = 0;
    bool has_out;
    has_out = in_out_cross(a, b);
    if (!has_out)
    {
        return_num += 1;
        out_list_buf.clear();
    }
    has_out = in_out_cross(b, a);
    if (!has_out)
    {
        return_num += 2;
        out_list_buf.clear();
    }
    point *p;
    intersect_point *p_t;
    switch (return_num)
    {
    case 1:
#ifdef DEBUG
        cout << "Delete1 Polygon: " << *a << endl;
#endif
        a->delete_poly_tranf(out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        delete a;
        p = b;
        for (unsigned int i = 0; i < b->len; ++i)
        {
            for (unsigned int j = 0; j < p->intersection.size(); ++j)
            {
                p_t = static_cast<intersect_point *>(p->intersection[j]);
                if (p_t->color == glob_color)
                {
                    p->intersection.erase(p->intersection.begin() + j);
                    delete p_t;
                    --j;
                }
            }
            p = p->next;
        }
        return 1;
        break;
    case 2:
    case 3:
#ifdef DEBUG
        cout << "Delete2 Polygon: " << *b << endl;
#endif
        b->delete_poly_tranf(out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        delete b;
        p = a;
        for (unsigned int i = 0; i < a->len; ++i)
        {
            for (unsigned int j = 0; j < p->intersection.size(); ++j)
            {
                p_t = static_cast<intersect_point *>(p->intersection[j]);
                if (p_t->color == glob_color)
                {
                    p->intersection.erase(p->intersection.begin() + j);
                    delete p_t;
                    --j;
                }
            }
            p = p->next;
        }
        return 2;
        break;
    default:
        for (unsigned int kk = 0; kk < out_list_buf.size(); ++kk)
        {
            out_list.insert(out_list_buf[kk]);
        }
        return 0;
        break;
    }
}

// cross 來 merge root
// 先判斷 root 是不是在 cross 外面
// root 如果在 cross 的邊上就算是在裡面
// 2019/6/6
// root 如果在外面才開始"順時針"遶(by 宥璁)
bool littlemerge::in_out_cross(point *root, point *cross)
{
    // 要一直判斷點是不是在外面，有點多餘
    point *p = root;
    bool has_out = false;
    for (int i = 0; i < root->len; i++)
    {
        bool outside = outside_poly(p, cross);
        if (outside)
        {
            has_out = true;
            if (p->prev->intersection.size() > 0)
            {
                p->prev->sort_intersection();
                for (int j = p->prev->intersection.size() - 1; j >= 0; --j)
                {
                    intersect_point *p_t = static_cast<intersect_point *>(p->prev->intersection[j]);
                    if (p_t->color == glob_color)
                    {
                        p_t->in = false;
                        out_list_buf.push_back(p_t);
                        break;
                    }
                }
            }
        }
        p = p->next;
    }
    return has_out;
}

bool littlemerge::outside_poly(point *root, point *cross)
{
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
    if (cross_cnt % 2 == 1)
        return false;
    else
        return true;
}

void littlemerge::find_cross(point *a, point *b)
{
    long long x, y;
    bool verti_a = a->verti;
    bool verti_b = b->verti;
    point *a_next = a->next;
    point *b_next = b->next;

    if (verti_a != verti_b)
    {
        x = verti_a ? a->x : b->x;
        y = verti_b ? a->y : b->y;
        if (inside_region(a, b, x, y))
        {
            new_intersect(a, b, x, y);
        }
    }
}

// add intersect b after point a
void littlemerge::add_intersect(point *a, point *b)
{
    a->intersection.push_back(b);
}

// check whether x is inside x1 and x2 or not
bool littlemerge::inside_edge(long long x1, long long x2, long long x)
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
bool littlemerge::inside_region(point *a, point *b, long long x, long long y)
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
void littlemerge::two_way_intersect(intersect_point *a, intersect_point *b)
{
    a->cross_point = b;
    a->tran = true;
    b->cross_point = a;
    b->tran = false;
}
// the new intersect (x,y) is right after point a and point b
void littlemerge::new_intersect(point *a, point *b, long long x, long long y)
{
    intersect_point *insert_a = new intersect_point(x, y, glob_color);
    intersect_point *insert_b = new intersect_point(x, y, glob_color);
    add_intersect(a, insert_a);
    add_intersect(b, insert_b);
    bool order = true;
    point *temp_a = a;
    point *temp_b = b;
    if (b->verti)
    {
        temp_a = b;
        temp_b = a;
        swap(insert_a, insert_b);
    }
    if (temp_a->dir)
    {
        if (!temp_b->dir)
        {
            order = false;
        }
    }
    else if (temp_b->dir)
        order = false;

    if (order)
        two_way_intersect(insert_a, insert_b);
    else
        two_way_intersect(insert_b, insert_a);
}
void littlemerge::insert_intersect(point *&root)
{
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *a = root_list[i];
        int len = 0;
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            point *temp = a->next;
            a->connect();
            len += a->intersection.size();
            a->intersection.clear();
            a = temp;
        }
        root_list[i]->len += len;
    }
    point *a = root;
    int len = 0;
    for (unsigned int j = 0; j < root->len; ++j)
    {
        point *temp = a->next;
        a->connect();
        len += a->intersection.size();
        a->intersection.clear();
        a = temp;
    }
    a->len += len;
}
void littlemerge::check_list(vector<point *> &new_list, point *&root)
{ // 判斷有些多邊形跟其他是分開的，根本不用何在一起，要直接加進去；感覺有點慢?
    for (unsigned int k = 0; k < root_list.size(); ++k)
    {
        point *p = root_list[k];
        bool isout = true;
        for (unsigned int z = 0; z < root_list[k]->len; ++z)
        {
            if (p->pcolor == glob_color)
            {
                isout = false;
                break;
            }
            p = p->next;
        }
        if (isout)
        {
            new_list.push_back(root_list[k]);
        }
        else
        {
            root_list[k]->delete_poly();
            delete root_list[k];
        }
    }
    point *p = root;
    bool isout = true;
    for (unsigned int z = 0; z < root->len; ++z)
    {
        if (p->pcolor == glob_color)
        {
            isout = false;
            break;
        }
        p = p->next;
    }
    if (isout)
    {
        new_list.push_back(root);
    }
    else
    {
        root->delete_poly();
        delete root;
    }
}
#endif
