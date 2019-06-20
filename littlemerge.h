#ifndef LITTLEMERGE_H
#define LITTLEMERGE_H
#include <float.h>
#include "glob_func.h"
#include "operation.h"
#include "point.h"
#include "TimeMgr.h"

static TimeMgr *Time = new TimeMgr();

class littlemerge
{
public:
    littlemerge();
    ~littlemerge();
    void insert(point *&, bool);
    void merge(point *&);
    void clip(point *&);
    void find_intersect(point *&);
    bool find_intersect(point *, point *);
    bool outside_poly(point *, point *);
    void find_cross(point *, point *);
    void insert_intersect();
    void add_intersect(point *a, point *b);
    bool inside_edge(long long x1, long long x2, long long y);
    bool inside_edgenhead(long long x1, long long x2, long long y);
    void two_way_intersect(intersect_point *, intersect_point *);
    void new_intersect(point *a, point *b, long long x, long long y, bool &a_is_in, bool &b_is_in);
    bool inside_region(point *a, point *b, long long x, long long y);
    bool in_out_cross(point *, point *);
    void check_list(vector<point *> &, point *&);
    void clear();
    void memory_check();
    void output(string);
    void output(string, point *root);
    void print();
    void print_outlist();
    void print_outbuf();
    void start_print() { printon = true; };
    void close_print() { printon = false; };
    bool check_point(point *&a, point *&b, long long int &x, long long int &y, bool &a_is_in, bool &b_is_in);
    short cal_state(point *&a, long long int &x, long long int &y);
    bool isleft(point *&p, point *&line);
    bool head2head(point *&a, point *&line);
    bool tail2head(point *&a, point *&line);
    bool head2tail(point *&a, point *&line);
    bool mid2head(point *&a, point *&line);
    bool mid2tail(point *&a, point *&line);
    void set_oper(operation &oper);
    bool check_mid2head(point *&a, point *&b);
    bool check_head2head(point *&a, point *&b);
    bool check_tail2mid(point *&a, point *&b);
    vector<point *> &get_list() { return root_list; };

private:
    vector<point *> root_list;
    set<point *> out_list;
    vector<point *> out_list_buf;
    vector<point *> intersect_point_list;
    vector<point *> intersect_list;
    int cross_cnt2;
    bool printon;
    bool ismerge;
    bool have_cross;
    double min_hole_area;
    double min_poly_area;
};

littlemerge::littlemerge() { printon = false; }
littlemerge::~littlemerge() {}
void littlemerge::set_oper(operation &oper) { oper.root_list = root_list; }
void littlemerge::clear()
{
    out_list.clear();
    out_list_buf.clear();
}
void littlemerge::output(string filename)
{
    fstream fout(filename.c_str(), fstream::out);
    fout << "OPERATION X1 ;\n\n";
    fout << "DATA MERGE X1 ;" << endl;
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *p = root_list[i];
        p->setcounterclockwise();
        fout << (p->counterclockwise ? "POLYGON " : "HOLE ");
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            fout << p->x << " " << p->y << " ";
            p = p->next;
        }
        fout << root_list[i]->x << " " << root_list[i]->y << " ;" << endl;
    }
    fout << "END DATA" << endl;
}
void littlemerge::memory_check()
{
    cerr << "===========================================================" << endl;
    cerr << "total points: " << point_cnt << " & total intersects: " << intersect_cnt << endl;
    int sum = 0;
    for (unsigned int i = 0; i < root_list.size(); ++i)
        sum += root_list[i]->len;
    cerr << "There are total " << sum << " points in list." << endl;
    cerr << "-----------------------------------------------------------" << endl;
    if (point_cnt == sum && intersect_cnt == 0)
    {
        cerr << "----------------There are no memory leak!!!----------------" << endl;
    }
    else
    {
        cerr << "----------------There are memory leak!!!-------------------" << endl;
    }
    cerr << "-----------------------------------------------------------" << endl;
    cerr << "===========================================================" << endl;
}
void littlemerge::output(string filename, point *root)
{
    fstream fout(filename.c_str(), fstream::out);
    fout << "OPERATION X1 ;\n\n";
    fout << "DATA MERGE X1 ;" << endl;
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *p = root_list[i];
        p->setcounterclockwise();
        fout << (p->counterclockwise ? "POLYGON " : "HOLE ");
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            fout << p->x << " " << p->y << " ";
            p = p->next;
        }
        fout << root_list[i]->x << " " << root_list[i]->y << " ;" << endl;
    }
    fout << "END DATA" << endl;
    fout << endl;
    fout << "DATA CLIPPING C1 ;" << endl;
    fout << "POLYGON ";
    point *p = root;
    for (unsigned int i = 0; i < root->len; ++i)
    {
        fout << p->x << ' ' << p->y << ' ';
        p = p->next;
    }
    fout << root->x << ' ' << root->y << ' ' << ';' << endl;
    fout << "END DATA" << endl;
}
void littlemerge::print()
{
    cout << "================RESULT=================" << endl;
    for (unsigned int i = 0; i < root_list.size(); ++i)
    {
        point *p = root_list[i];
        cout << "POLYGON " << i;
        string ang = "\\/X";
        for (unsigned int j = 0; j < root_list[i]->len; ++j)
        {
            cout << *p << " DIR" << (p->verti ? '|' : '-') << " Angle" << ang[p->angle] << endl;
            p = p->next;
        }
    }
    cout << "=======================================" << endl;
}
void littlemerge::print_outlist()
{
    set<point *>::iterator iter;
    cout << "================OUTLIST================" << endl;
    for (iter = out_list.begin(); iter != out_list.end(); ++iter)
        (*iter)->print();
    cout << "=======================================" << endl;
}
void littlemerge::print_outbuf()
{
    cout << "=================OUTBUF================" << endl;
    for (unsigned int i = 0; i < out_list_buf.size(); ++i)
        out_list_buf[i]->print();
    cout << "=======================================" << endl;
}
void littlemerge::insert(point *&root, bool ismerge_t)
{
    ismerge = ismerge_t;
    list_construct(root);
    clear();
    if (ismerge)
        merge(root);
    else
        clip(root);
}

void littlemerge::merge(point *&root)
{
    if (root_list.size() == 0)
    {
        root_list.push_back(root);
        return;
    }
    min_hole_area = DBL_MAX;
    min_poly_area = DBL_MAX;
    Time->tic();
    find_intersect(root);
    Time->toc("Find intersect");
    if (printon)
        print_outlist();
    Time->tic();
    insert_intersect();
    Time->toc("Insert intersect");
    if (printon)
    {
        cerr << "POLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLY" << endl;
        root->print_poly();
        cerr << "POLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLYPOLY" << endl;
    }
    vector<point *> new_list;
    ++glob_color;
    Time->tic();
    while (out_list.size() > 0)
    {
        vector<point *> poly;
        point *new_poly;
        set<point *>::iterator iter;
        iter = out_list.begin();
        new_poly = *iter;
        point *p = (*iter)->next;
        out_list.erase(*iter);
        if (printon)
            cout << "WALK " << *new_poly << endl;
        new_poly->pcolor = glob_color;
        new_poly->root->iscolored = true;
        poly.push_back(new_poly);
        while (p != new_poly)
        {
            if (printon)
                cout << "WALK " << *p << endl;
            p->pcolor = glob_color;
            p->root->iscolored = true;
            poly.push_back(p);
            if (!p->ispoint())
            {
                intersect_point *p_cross = static_cast<intersect_point *>(p)->cross_point;
                if (!static_cast<intersect_point *>(p)->in)
                    out_list.erase(p);
                if (!p_cross->in)
                {
                    p = p_cross;
                    if (printon)
                        cout << "cross " << *p << ' ' << p << endl;
                    p->pcolor = glob_color;
                    p->root->iscolored = true;
                    poly.push_back(p);
                    if (p == new_poly)
                        break;
                    out_list.erase(p);
                }
                // else
                //     p_cross->pcolor = glob_color;
            }
            p = p->next;
        }
        // 用新走出的形狀做出多邊形
        // 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
        new_list.push_back(construct_new_poly(poly));
    }
    Time->toc("Walk");
    Time->tic();
    check_list(new_list, root);
    Time->toc("Check list");
    for (unsigned int i = 0; i < new_list.size(); ++i)
        new_list[i]->setcounterclockwise();
    root_list = new_list;
}

void littlemerge::clip(point *&root)
{
    if (root_list.size() == 0)
    {
        root->delete_poly();
        delete root;
        return;
    }
    min_hole_area = DBL_MAX;
    min_poly_area = DBL_MAX;
    find_intersect(root);
    // printon = true;
    if (printon)
        print_outlist();
    insert_intersect();
    vector<point *> new_list;
    ++glob_color;
    while (out_list.size() > 0)
    {
        bool cycle = false;
        vector<point *> poly;
        point *new_poly;
        set<point *>::iterator iter;
        iter = out_list.begin();
        new_poly = *iter;
        point *p = (*iter)->next;
        out_list.erase(*iter);
        if (printon)
            cout << "WALK " << *new_poly << endl;
        new_poly->pcolor = glob_color;
        new_poly->root->iscolored = true;
        poly.push_back(new_poly);
        while (true)
        {
            if (p == new_poly)
            {
                poly.push_back(p);
                break;
            }
            if (p->pcolor == glob_color)
            {
                cycle = true;
                poly.push_back(p);
                break;
            }
            if (printon)
            {
                cout << "WALK " << *p << endl;
                cout << "IT is point/intersect " << p->ispoint() << endl;
            }
            p->pcolor = glob_color;
            p->root->iscolored = true;
            poly.push_back(p);
            if (!p->ispoint())
            {
                if (p->isclip) //對clip的交點來說，in一定走，out一定換
                {
                    // cout << "CLIP " << p->isclip << endl;
                    intersect_point *p_cross = static_cast<intersect_point *>(p)->cross_point;
                    if (static_cast<intersect_point *>(p)->in)
                        out_list.erase(p);
                    else
                    {
                        p = p_cross;
                        if (printon)
                            cout << "cross " << *p << ' ' << p << endl;
                        if (p == new_poly)
                        {
                            poly.push_back(p);
                            // cerr << "WW" << endl;
                            break;
                        }
                        if (p->pcolor == glob_color)
                        {
                            // cout << "PUSH " << p << endl;
                            cycle = true;
                            // cout << "CYCLE" << endl;
                            poly.push_back(p);
                            break;
                        }
                        p->pcolor = glob_color;
                        p->root->iscolored = true;
                        poly.push_back(p);
                        out_list.erase(p);
                    }
                }
                else
                { // 不是clip的多邊形，如果對面是in一定換
                    //兩個in不知道怎麼辦
                    // cout << "NOTCLIP " << p->isclip << endl;
                    intersect_point *p_cross = static_cast<intersect_point *>(p)->cross_point;
                    if (p_cross->in)
                    {
                        p = p_cross;
                        if (printon)
                            cout << "cross " << *p << ' ' << p << endl;
                        if (p == new_poly)
                        {
                            poly.push_back(p);
                            // cerr << "WW" << endl;
                            break;
                        }
                        if (p->pcolor == glob_color)
                        {
                            // cout << "CYCLE" << endl;
                            // cout << "PUSH " << p << endl;
                            cycle = true;
                            poly.push_back(p);
                            break;
                        }
                        p->pcolor = glob_color;
                        p->root->iscolored = true;
                        poly.push_back(p);
                        out_list.erase(p);
                    }
                }
            }
            p = p->next;
        }
        if (cycle == true)
        {
            int head_cnt = 0;
            while (poly[head_cnt] != p)
                ++head_cnt;
            if (head_cnt != 0)
            {
                if (head_cnt == poly.size() - 1)
                    continue;
                // cout << poly.size() << endl;
                // cout << "P " << p << endl;
                // cout << "CUT " << head_cnt << endl;
                poly.erase(poly.begin(), poly.begin() + head_cnt);
                // cout << poly.size() << endl;
                // for (unsigned t = 0; t < poly.size(); ++t)
                //     cout << (poly[t]) << endl;
            }
        }
        assert(poly[0] == poly[poly.size() - 1]);
        // 用新走出的形狀做出多邊形
        // 需要初始化更種參數：包刮：x, y next, prev, s_next, len, angle, verti, dir
        new_list.push_back(construct_new_poly(poly));
    }
    check_list(new_list, root);
    for (unsigned int i = 0; i < new_list.size(); ++i)
        new_list[i]->setcounterclockwise();
    root_list = new_list;
}

void littlemerge::find_intersect(point *&root)
{
    for (int i = 0; i < root_list.size(); ++i)
    {
        if (find_intersect(root_list[i], root))
        {
            root_list[i] = root_list[root_list.size() - 1];
            root_list.pop_back();
            --i;
        }
    }
}

bool littlemerge::find_intersect(point *a, point *b)
{
    out_list_buf.clear();
    if (b->minx > a->maxx || b->miny > a->maxy || b->maxx < a->minx || b->maxy < a->miny)
        return 0;
    point *p_k = a;
    point *p_l = b;
    ++glob_color;
    cross_cnt2 = 0;
    intersect_list.clear();
    have_cross = false;
    for (int k = 0; k < a->len; k++)
    {
        for (int l = 0; l < b->len; l++)
        {
            find_cross(p_k, p_l);
            p_l = p_l->next;
        }
        p_k = p_k->next;
    }
    bool return_type = false;
    bool has_out;
    // a包在b裡面，a刪掉
    has_out = in_out_cross(a, b);
    if (printon)
        cerr << "HAS OUT is " << has_out << endl;
    if (!has_out && !have_cross)
    {
        return_type = true;
        out_list_buf.clear();
    }
    // b包在a裡面，存最小b面積
    has_out = in_out_cross(b, a);
    if (!has_out && !have_cross && (cross_cnt2 == 0))
    {
        if (return_type == true)
        {
            min_hole_area = min_poly_area = DBL_MIN;
        }
        else
        {
            a->setcounterclockwise();
            if (a->counterclockwise)
            {
                if (min_poly_area > a->areas)
                    min_poly_area = a->areas;
            }
            else
            {
                if (min_hole_area > -a->areas)
                    min_hole_area = -a->areas;
            }
            if (printon)
            {
                cerr << a->len << endl;
                cerr << *a << endl;
                cerr << a->counterclockwise << endl;
                cerr << "Poly is inside of something" << endl;
                cerr << ((min_poly_area > min_hole_area) ? "POLY" : "HOLE") << endl;
            }
            out_list_buf.clear();
        }
    }
    point *p;
    intersect_point *p_t;
    if (return_type)
    {
        // 如果b是洞那一樣
        a->delete_poly(); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        delete a;
        for (unsigned int i = 0; i < intersect_list.size(); ++i)
            intersect_list[i]->intersection.pop_back();
    }
    else
    {
        for (unsigned int kk = 0; kk < out_list_buf.size(); ++kk)
        {
            out_list.insert(out_list_buf[kk]);
        }
    }
    return return_type;
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
    if (printon)
    {
        static fstream myfout("intersect.txt", fstream::out);
        myfout << *(a) << *(a->next) << a->verti << a->dir << *b << *(b->next) << b->verti << b->dir << endl;
    }
    if (verti_a != verti_b)
    {
        x = verti_a ? a->x : b->x;
        y = verti_b ? a->y : b->y;
        if (inside_region(a, b, x, y))
        {
            bool a_is_in, b_is_in;
            if (check_point(a, b, x, y, a_is_in, b_is_in))
            {
                if (printon)
                    cout << *a << *b << x << ' ' << y << ' ' << a_is_in << ' ' << b_is_in << endl;
                new_intersect(a, b, x, y, a_is_in, b_is_in);
            }
        }
    }
}

// add intersect b after point a
void littlemerge::add_intersect(point *a, point *b) { a->intersection.push_back(b); }

// check whether x is inside x1 and x2 or not
bool littlemerge::inside_edge(long long x1, long long x2, long long x)
{
    // cout << "LLL" << x1 << ' ' << x2 << ' ' << x << endl;
    if (x >= x1 && x <= x2)
        return true;
    else if (x <= x1 && x >= x2)
        return true;
    else
        return false;
}
bool littlemerge::inside_edgenhead(long long x1, long long x2, long long x)
{
    // cout << "LLL" << x1 << ' ' << x2 << ' ' << x << endl;
    if (x > x1 && x < x2)
        return true;
    else if (x < x1 && x > x2)
        return true;
    else
        return false;
}
bool littlemerge::inside_region(point *a, point *b, long long x, long long y)
{
    if (a->verti)
    {
        // if (y == a->next->y && x == b->next->x)
        //     return false;
        // if (y == a->next->y && x == b->x)
        //     return false;
        bool b_b = inside_edge(b->x, b->next->x, x);
        bool b_c = inside_edge(a->y, a->next->y, y);
        return (b_b && b_c);
    }
    else
    {
        // if (x == a->next->x && y == b->next->y)
        //     return false;
        // if (x == a->next->x && y == b->y)
        //     return false;
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
    b->tran = true;
}
// the new intersect (x,y) is right after point a and point b
void littlemerge::new_intersect(point *a, point *b, long long x, long long y, bool &a_is_in, bool &b_is_in)
{
    if (printon)
    {
        cout << "NEW intersect" << *a << *b << x << ' ' << y << ' ' << a_is_in << b_is_in << ismerge << endl;
    }
    cross_cnt2 += 2;
    intersect_point *insert_a = new intersect_point(x, y, glob_color, a_is_in);
    insert_a->root = a->root;
    a->root->has_intersect = true;
    intersect_point_list.push_back(a);

    intersect_point *insert_b = new intersect_point(x, y, glob_color, b_is_in);
    insert_b->root = b->root;
    b->root->has_intersect = true;
    intersect_point_list.push_back(b);

    if (!ismerge)
    {
        insert_a->isclip = false;
        insert_b->isclip = true;
    }
    intersect_list.push_back(b);
    // cout << out_list_buf.size() << endl;
    if (b_is_in != ismerge)
        out_list_buf.push_back(insert_b);
    if (printon)
    {
        cout << insert_a << ' ' << insert_b << endl;
    }
    add_intersect(a, insert_a);
    add_intersect(b, insert_b);
    two_way_intersect(insert_a, insert_b);
    // cout << out_list_buf.size() << endl;
}
void littlemerge::insert_intersect()
{
    for (unsigned int i = 0; i < intersect_point_list.size(); ++i)
    {
        point *&a = intersect_point_list[i];
        a->connect(ismerge);
        int len = a->intersection.size();
        a->intersection.clear();
        a->root->len += len;
    }
    intersect_point_list.clear();
}
void littlemerge::check_list(vector<point *> &new_list, point *&root)
{ // 判斷有些多邊形跟其他是分開的，根本不用何在一起，要直接加進去；感覺有點慢? 已更改(6/19)
    for (unsigned int k = 0; k < root_list.size(); ++k)
    {
        if (root_list[k]->iscolored)
        {
            root_list[k]->delete_poly();
            delete root_list[k];
        }
        else
        {
            if (root_list[k]->has_intersect)
            {
                int cut_len = 0;
                point *p = root_list[k];
                for (unsigned int z = 0; z < root_list[k]->len; ++z)
                {
                    point *p_next = p->next;
                    if (!p->ispoint())
                    {
                        cut_len += 1;
                        p->prev->next = p->next;
                        p->next->prev = p->prev;
                        delete p;
                    }
                    p = p_next;
                }
                root_list[k]->has_intersect = false;
                root_list[k]->len -= cut_len;
                list_construct(root_list[k]);
            }
            else
                new_list.push_back(root_list[k]);
        }
    }
    if (root->iscolored)
    {
        root->delete_poly();
        delete root;
    }
    else
    {
        if (min_hole_area != DBL_MAX || min_poly_area != DBL_MAX)
        {
            if (ismerge)
            {
                if (min_hole_area == DBL_MIN)
                {
                    root->delete_poly();
                    delete root;
                }
                else if (min_poly_area < min_hole_area)
                {
                    root->delete_poly();
                    delete root;
                }
                else
                {
                    new_list.push_back(root);
                }
            }
            else
            {
                if (min_poly_area == DBL_MIN)
                {
                    root->delete_poly();
                    delete root;
                }
                else if (min_poly_area > min_hole_area)
                {
                    root->delete_poly();
                    delete root;
                }
                else
                {
                    new_list.push_back(root);
                }
            }
        }
        else
        {
            if (ismerge)
                new_list.push_back(root);
            else
            {
                root->delete_poly();
                delete root;
            }
        }
    }
}
// 傳入：a和b為頭的邊和交點的x、y座標
// 已經確定有交點了
bool littlemerge::check_point(point *&a, point *&b, long long int &x, long long int &y, bool &a_is_in, bool &b_is_in)
{ // return false則沒有交點
    // 左邊是裡面
    // [1] a尾碰到b(交點在a尾)，(1) b尾：無交點 (2) b中間：對a來說交點是in；對b來說交點看在a的左邊還是右邊
    // (3)b頭；b看在a的左邊是裡面；右邊看狀況；如果A轉B的方向就是in反方向out [2] a中間碰到b(交點在a中間)， (1)b尾：對b來中是in，對a來說看在b的左右邊
    // (2)b中間：看在對方的左右邊 (3)b頭：看往對方的哪一邊 [3] a頭碰到b(交點在a頭)，(1)b尾：無 (2)b中間：左右邊
    // (3)b頭：對a來說：b在左邊就是in，在右邊要看b是不是從右邊來
    int state_a = cal_state(a, x, y);
    int state_b = cal_state(b, x, y);
    if (printon)
    {
        cout << "Egde of " << *a << *b << " has state " << state_a << ' ' << state_b << endl;
        cout << *a << *(a->next) << endl;
        cout << *b << *(b->next) << endl;
    }
    if (state_a == 1)
    {
        if (state_b == 2)
        {
            // if (check_tail2mid(a, b))
            // {
            a_is_in = !isleft(a, b); // a的下一個如果在b線段的左邊就是in
            b_is_in = mid2tail(b, a);
            // }
            // return false;
        }
        else if (state_b == 3)
        {
            if (!a->next->dir == b->dir != a->dir == b->prev->dir)
                return false;
            a_is_in = tail2head(a, b);
            b_is_in = head2tail(b, a);
            // cout << "YY" << a_is_in << b_is_in << endl;
        }
        else
            return false;
    }
    else if (state_a == 2)
    {
        if (state_b == 1)
        {
            // if (check_tail2mid(b, a))
            // {
            a_is_in = mid2tail(a, b);
            b_is_in = !isleft(b, a);
            // }
            // else
            // return false;
        }
        else if (state_b == 2)
        {
            have_cross = true;
            a_is_in = isleft(a->next, b);
            b_is_in = isleft(b->next, a);
        }
        else
        {
            // if (check_mid2head(a, b))
            // {
            a_is_in = mid2head(a, b);
            b_is_in = isleft(b->next, a);
            // }
            // else
            //     return false;
        }
    }
    else
    {
        if (state_b == 1)
        {
            if (!(b->next->dir == a->dir) != (b->dir == a->prev->dir))
                return false;
            a_is_in = head2tail(a, b);
            b_is_in = tail2head(b, a);
        }
        else if (state_b == 3)
        {
            // if ((!a->counterclockwise || !b->counterclockwise) && check_head2head(a, b))
            // {
            if (!(b->prev->dir == a->dir) != (b->dir == a->prev->dir))
                return false;
            a_is_in = head2head(a, b);
            b_is_in = head2head(b, a);
            // }
            // else
            //     return false;
        }
        else
        {
            // if (check_mid2head(b, a))
            // {
            a_is_in = isleft(a->next, b);
            b_is_in = mid2head(b, a);
            // }
            // else
            //     return false;
        }
    }
    if (ismerge)
    {
        // assert(a_is_in != 1 || b_is_in != 1);
        if (a_is_in == 0 && b_is_in == 0)
            return false;
        else if (a_is_in == 1 && b_is_in == 1)
        {
            cerr << "There are two in. " << endl;
            b_is_in = 0;
        }
    }
    else
    {
        // if (a_is_in == 0 && b_is_in == 0)
        // return false;
    }
    return true;
}
short littlemerge::cal_state(point *&a, long long int &x,
                             long long int &y)
{ // 交點一定在a上面，RETURN三種狀況：1：交點在A尾；2：交點在A中間；3：交點在A頭
    if (a->verti)
    {
        // a是垂直線的狀況
        if (a->y == y)
            return 3;
        else if (a->next->y == y)
            return 1;
        else
            return 2;
    }
    else
    {
        if (a->x == x)
            return 3;
        else if (a->next->x == x)
            return 1;
        else
            return 2;
    }
}
bool littlemerge::isleft(point *&p, point *&line)
{ //判斷p點是不是在line的左邊
    if (line->verti)
        return ((p->x > line->x) != line->dir);
    else
        return ((p->y < line->y) != line->dir);
}
bool littlemerge::head2head(point *&a, point *&line)
{ //判斷a點是不是在line的裡面
    if (line->verti)
    {
        if (line->dir)
        {
            if (!a->dir)
                return true;
            else if (!line->prev->dir)
            {
                if (!ismerge)
                    return (true == line->counterclockwise);
                else
                    return true;
            }
            else
            {
                if (!ismerge)
                    return (false == line->counterclockwise);
                else
                    return false;
            }
        }
        else
        {
            if (a->dir)
                return true;
            else if (!line->prev->dir)
            {
                if (!ismerge)
                    return (false == line->counterclockwise);
                else
                    return false;
            }
            else
            {
                if (!ismerge)
                    return (true == line->counterclockwise);
                else
                    return true;
            }
        }
    }
    else
    {
        if (line->dir)
        {
            if (a->dir)
                return true;
            else if (!line->prev->dir)
            {
                if (!ismerge)
                    return (false == line->counterclockwise);
                else
                    return false;
            }
            else
            {
                if (!ismerge)
                    return (true == line->counterclockwise);
                else
                    return true;
            }
        }
        else
        {
            if (!a->dir)
                return true;
            else if (!line->prev->dir)
            {
                if (!ismerge)
                    return (true == line->counterclockwise);
                else
                    return true;
            }
            else
            {
                if (!ismerge)
                    return (false == line->counterclockwise);
                else
                    return false;
            }
        }
    }
}
bool littlemerge::tail2head(point *&a, point *&line)
{ // a尾在line頭上，判斷a是不是在line裡面
    if (line->verti)
    {
        if (line->dir)
        {
            if (a->next->dir)
                return true;
            if (line->prev->dir)
                return false;
            else
                return true;
        }
        else
        {
            if (!a->next->dir)
                return true;
            if (line->prev->dir)
                return true;
            else
                return false;
        }
    }
    else
    {
        if (line->dir)
        {
            if (a->next->dir)
                return true;
            if (line->prev->dir)
                return true;
            else
                return false;
        }
        else
        {
            if (!a->next->dir)
                return true;
            if (line->prev->dir)
                return false;
            else
                return true;
        }
    }
}
bool littlemerge::head2tail(point *&a, point *&line)
{ // a尾在line頭上，判斷a是不是在line裡面
    if (line->verti)
    {
        if (line->dir)
        {
            if (!a->dir)
                return true;
            if (line->next->dir)
                return true;
            else
                return false;
        }
        else
        {
            if (a->dir)
                return true;
            if (!line->next->dir)
                return true;
            else
                return false;
        }
    }
    else
    {
        if (line->dir)
        {
            if (a->dir)
                return true;
            if (!line->next->dir)
                return true;
            else
                return false;
        }
        else
        {
            if (!a->dir)
                return true;
            if (line->next->dir)
                return true;
            else
                return false;
        }
    }
}
bool littlemerge::mid2head(point *&a, point *&line)
{ // line從a的中間射出
    if (line->verti)
    {
        if (line->dir)
        {
            if (!a->dir)
                return true;
            else if (line->prev->dir)
                return (false == line->counterclockwise);
            else
                return (true == line->counterclockwise);
        }
        else
        {
            if (a->dir)
                return true;
            else if (line->prev->dir)
                return (true == line->counterclockwise);
            else
                return (false == line->counterclockwise);
        }
    }
    else
    {
        if (line->dir)
        {
            if (a->dir)
                return true;
            else if (line->prev->dir)
                return (true == line->counterclockwise);
            else
                return (false == line->counterclockwise);
        }
        else
        {
            if (!a->dir)
                return true;
            else if (line->prev->dir)
                return (false == line->counterclockwise);
            else
                return (true == line->counterclockwise);
        }
    }
}
bool littlemerge::mid2tail(point *&a, point *&line)
{ // line從a的中間涉入
    if (line->verti)
    {
        if (line->dir)
        {
            if (!a->dir)
                return true;
            else if (!line->next->dir)
                return false;
            else
                return true;
        }
        else
        {
            if (a->dir)
                return true;
            else if (!line->next->dir)
                return true;
            else
                return false;
        }
    }
    else
    {
        if (line->dir)
        {
            if (a->dir)
                return true;
            else if (line->next->dir)
                return false;
            else
                return true;
        }
        else
        {
            if (!a->dir)
                return true;
            else if (line->next->dir)
                return true;
            else
                return false;
        }
    }
}
bool littlemerge::check_mid2head(point *&a, point *&b)
{
    // a中間 交 b頭
    // 看b前一個來的方向：如果和a同向而且b前一個比a來的外面(和a點比)，就不用算 return false
    if (a->dir == b->prev->dir)
    {
        if (a->verti)
        {
            if (inside_edgenhead(b->prev->y, b->y, a->y))
                return false;
        }
        else
        {
            if (inside_edgenhead(b->prev->x, b->x, a->x))
                return false;
        }
    }
    else if (b->prev->prev->dir == b->dir)
    {
        if (a->verti)
        {
            if (inside_edgenhead(b->y, a->y, b->prev->y) || inside_edgenhead(b->y, a->next->y, b->prev->y))
                return false;
        }
        else
        {
            if (inside_edgenhead(b->x, a->x, b->prev->x) || inside_edgenhead(b->x, a->next->x, b->prev->x))
                return false;
        }
    }
    return true;
}
bool littlemerge::check_head2head(point *&a, point *&b)
{
    if (!((a->prev->dir == b->dir) != (b->prev->dir == a->dir)))
        return false;
    return true;
}
bool littlemerge::check_tail2mid(point *&a, point *&b)
{
    if (a->counterclockwise)
    {
        if (b->verti)
        {
            if (inside_edge(a->next->y, a->next->next->y, b->y))
                return false;
        }
        else
        {
            if (inside_edge(a->next->x, a->next->next->x, b->x))
                return false;
        }
    }
    return true;
}
#endif
