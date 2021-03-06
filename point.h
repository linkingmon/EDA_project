#ifndef POINT_H
#define POINT_H
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>
using namespace std;
#define DIV 922337203685477580.0
static int point_cnt = 0;
static int intersect_cnt = 0;

class interesect_point;
class point
{
private:
protected:
public:
    point(long long xt, long long yt);
    point(const point &); // copy constructor
    virtual ~point();
    double area(point *&p) { return x / DIV * (p->y) / DIV - y / DIV * (p->x) / DIV; }
    void swap_dir() { swap(next, prev); };
    void connect(bool ismerge);
    void sort_intersection(bool ismerge);
    void sort_asc(bool ismerge);
    void sort_dsc(bool ismerge);
    void print_poly()
    {
        point *p = this;
        for (unsigned int i = 0; i < len; ++i)
        {
            p->print();
            p = p->next;
        }
        cout << endl;
    }
    void delete_poly();
    void delete_poly_tranf(set<point *> &);
    void delete_poly_tranf(set<point *> &, set<point *> &);
    friend ostream &operator<<(ostream &os, const point &p);
    virtual void print()
    {
        string ang = "\\/";
        cout << *this << this->intersection.size() << ' ' << this << " is poly with root " << *(this->root) << ' ' << this->root << endl;
    };
    virtual bool ispoint() { return true; };
    void setcounterclockwise();
    bool operator!=(point q) { return x != q.x || y != q.y; };
    bool operator==(point q) { return x == q.x && y == q.y; };

    vector<point *> intersection;
    vector<point *> all_intersection;
    long long x, y;
    double areas;
    point *next; // 後一個點
    point *prev; // 前一個點
    point *root;
    long long minx, miny, maxx, maxy; //多邊形的邊界
    int len;                          // ROOT 專用
    int pcolor;
    bool angle; //角度
    bool verti; //垂直
    bool dir;   //往正的地方走(上、右)
    bool counterclockwise;
    bool isclip;
    bool iscolored;
    bool has_intersect;
    // bool end; // 簡化walk步驟
};

class root_point : public point
{
private:
public:
};

class intersect_point : public point
{
private:
public:
    intersect_point(long long, long long, int, bool);
    ~intersect_point();
    bool ispoint() { return false; };
    void print();
    intersect_point *cross_point;
    int color;     // 標示交點屬於哪個多邊形
    bool in;       // 標示進入對面的多邊形否
    bool tran;     // 可以轉到對面的交點上
    point *next_p; //下一個交點
};

void point::setcounterclockwise()
{
    point *p = this;
    areas = 0;
    for (unsigned int i = 0; i < this->len; ++i)
    {
        areas += p->area(p->next);
        p = p->next;
    }
    bool cc = (areas > 0);
    for (unsigned int i = 0; i < len; ++i)
    {
        p->counterclockwise = cc;
        p = p->next;
    }
}
ostream &operator<<(ostream &os, const point &p)
{
    os << '(' << setw(4) << p.x << ',' << setw(4) << p.y << ')';
    return os;
};

point::point(long long xt, long long yt) : x(xt), y(yt)
{
    // cout << "construct vertex" << *this << this << endl;
    point_cnt += 1;
    verti = false;
    dir = false;
}
// 複製新的多邊形
point::point(const point &p2)
{
    // cout << "construct vertex" << *this << endl;
    point_cnt += 1;
    x = p2.x;
    y = p2.y;
    verti = p2.verti;
    dir = p2.dir;
}

// 把頂點裡面存的交點清掉
point::~point()
{
    // cout << "destruct vertex" << *this << endl;
    point_cnt -= 1;
}

void point::connect(bool ismerge)
{
    point *n = next;
    point *temp = this;
    if (intersection.size() == 0)
        return;
    sort_intersection(ismerge);
    int i;
    for (i = 0; i < intersection.size(); ++i)
    {
        intersection[i]->prev = temp;
        temp->next = intersection[i];
        temp = intersection[i];
    }
    n->prev = intersection[--i];
    intersection[i]->next = n;
}

// point *point::connect_2(bool ismerge)
// {
//     point *n = next;
//     point *temp = this;
//     if (intersection.size() == 0)
//         return NULL;
//     sort_intersection(ismerge);
//     int i;
//     temp = intersection[0];
//     temp->prev = this;
//     next = temp;

//     for (i = 0; i < intersection.size() - 1; ++i)
//     {
//         static_cast<intersect_point *>(temp)->next_p = intersection[i + 1];
//         temp->next = intersection[i + 1];
//         temp = intersection[i + 1];
//         temp->prev = intersection[i];
//     }
//     n->prev = intersection[i];
//     intersection[i]->next = n;
//     return intersection[i];
// }

void point::sort_asc(bool ismerge)
{
    if (ismerge)
    {
        if (verti)
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->y < intersection[j]->y || (key->y == intersection[j]->y && static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
        else
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->x < intersection[j]->x || (key->x == intersection[j]->x && static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
    }
    else
    {

        if (verti)
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->y < intersection[j]->y || (key->y == intersection[j]->y && !static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
        else
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->x < intersection[j]->x || (key->x == intersection[j]->x && !static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
    }
}

void point::sort_dsc(bool ismerge)
{
    if (ismerge)
    {
        if (verti)
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->y > intersection[j]->y || (key->y == intersection[j]->y && static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
        else
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->x > intersection[j]->x || (key->x == intersection[j]->x && static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
    }
    else
    {
        if (verti)
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->y > intersection[j]->y || (key->y == intersection[j]->y && !static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
        else
        {
            for (int i = 1; i < intersection.size(); ++i)
            {
                point *key = intersection[i];
                int j = i - 1;
                while (j >= 0 && (key->x > intersection[j]->x || (key->x == intersection[j]->x && !static_cast<intersect_point *>(intersection[j])->in)))
                {
                    intersection[j + 1] = intersection[j];
                    --j;
                }
                intersection[j + 1] = key;
            }
        }
    }
}

void point::sort_intersection(bool ismerge)
{
    bool reverse = false;
    if (verti)
    {
        if (y > next->y)
            reverse = true;
    }
    else if (x > next->x)
        reverse = true;
    if (reverse)
    {
        sort_dsc(ismerge);
    }
    else
    {
        sort_asc(ismerge);
    }
}
// 把整個多邊形刪掉，需要刪掉全部的 頂點 和 交點
void point::delete_poly()
{
    point *p = this->next;
    for (unsigned int i = 0; i < len - 1; ++i)
    {
        p = p->next;
        for (unsigned int j = 0; j < p->prev->intersection.size(); ++j)
            delete p->prev->intersection[j];
        p->prev->intersection.clear();
        delete p->prev;
    }
    for (unsigned int j = 0; j < this->intersection.size(); ++j)
        delete this->intersection[j];
}

void point::delete_poly_tranf(set<point *> &out_list)
{
    point *p = this->next;
    for (unsigned int i = 0; i < len - 1; ++i)
    {
        p = p->next;
        for (unsigned int j = 0; j < p->prev->intersection.size(); ++j)
        {
            intersect_point *p_t = static_cast<intersect_point *>(p->prev->intersection[j]);
            p_t->cross_point->tran = false;
            out_list.erase(p_t->cross_point);
            out_list.erase(p->prev->intersection[j]);
            delete p->prev->intersection[j];
        }
        p->prev->intersection.clear();
        delete p->prev;
    }
    for (unsigned int j = 0; j < this->intersection.size(); ++j)
    {
        intersect_point *p_t = static_cast<intersect_point *>(this->intersection[j]);
        p_t->cross_point->tran = false;
        out_list.erase(p_t->cross_point);
        out_list.erase(this->intersection[j]);
        delete this->intersection[j];
    }
}

void point::delete_poly_tranf(set<point *> &out_list, set<point *> &out_list2)
{
    point *p = this->next;
    for (unsigned int i = 0; i < len - 1; ++i)
    {
        p = p->next;
        for (unsigned int j = 0; j < p->prev->intersection.size(); ++j)
        {
            intersect_point *p_t = static_cast<intersect_point *>(p->prev->intersection[j]);
            p_t->cross_point->tran = false;
            out_list.erase(p_t->cross_point);
            out_list2.erase(p_t->cross_point);
            out_list.erase(p->prev->intersection[j]);
            out_list2.erase(p->prev->intersection[j]);
            delete p->prev->intersection[j];
        }
        p->prev->intersection.clear();
        delete p->prev;
    }
    for (unsigned int j = 0; j < this->intersection.size(); ++j)
    {
        intersect_point *p_t = static_cast<intersect_point *>(this->intersection[j]);
        p_t->cross_point->tran = false;
        out_list.erase(p_t->cross_point);
        out_list2.erase(p_t->cross_point);
        out_list.erase(this->intersection[j]);
        out_list2.erase(this->intersection[j]);
        delete this->intersection[j];
    }
}

intersect_point::intersect_point(long long xt, long long yt, int colort, bool is_in) : point(xt, yt), color(colort), in(is_in)
{
    point_cnt -= 1;
    intersect_cnt += 1;
    // cout << "construct intersect" << *this << endl;
}

intersect_point::~intersect_point()
{
    point_cnt += 1;
    intersect_cnt -= 1;
    // cout << "delete intersect" << *this << endl;
}

void intersect_point::print() { cout << "II " << *this << (in ? "  in" : " out") << " tran" << tran << ' ' << this << endl; }
#endif
