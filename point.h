#include <iostream>
#include <vector>
#include <set>
#include <iomanip>
using namespace std;
#define DIV 922337203685477580.0
static int point_cnt = 0;
static int intersect_cnt = 0;

class point
{
private:
protected:
public:
    point(long long xt, long long yt);
    point(const point &); // copy constructor
    virtual ~point();
    float area(point *p) { return x / DIV * (p->y) / DIV - y / DIV * (p->x) / DIV; }
    // virtual void print() { cout << x << " " << y << endl; }
    void swap_dir() { swap(next, prev); };
    void connect();
    void sort_intersection();
    void sort_asc();
    void sort_dsc();
    void delete_poly();
    friend ostream &operator<<(ostream &os, const point &p);
    virtual void print() { cout << *this << endl; };
    virtual bool ispoint() { return true; };

    vector<point *> intersection;
    long long x, y;
    point *next;
    point *prev;
    point *s_next;
    int len;
    // point* straright_next;
    short angle;
    bool verti;
    bool dir;
    // point &bool operator<(point &b);
};

ostream &operator<<(ostream &os, const point &p)
{
    os << '(' << setw(4) << p.x << ',' << setw(4) << p.y << ')';
    return os;
};

point::point(long long xt, long long yt) : x(xt), y(yt)
{
    // cout << "construct vertex" << *this << endl;
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
}

// 把頂點裡面存的交點清掉
point::~point()
{
    // cout << "destruct vertex" << *this << endl;
    point_cnt -= 1;
    // 宥璁沒有把他從 intesection 清掉
    // 所以不用清了

    // cout << "delete vertex" << *this << endl;
    // for (unsigned int j = 0; j < intersection.size(); ++j)
    // {
    //     delete intersection[j];
    // }
    // intersection.clear();
}
// bool operator < (point* a,point* b)

void point::connect()
{
    point *n = next;
    point *temp = this;
    if (intersection.size() == 0)
        return;
    sort_intersection();
    int i;
    for (i = 0; i < intersection.size(); ++i)
    {
        intersection[i]->prev = temp;
        temp->next = intersection[i];
        temp = intersection[i];
    }
    n->prev = intersection[--i];
    intersection[i]->next = n;
    // bool reverse = false;
    // int i;
    // if (verti)
    // {
    //     if (y > next->y)
    //         reverse = true;
    // }
    // else if (x > next->x)
    //     reverse = true;
    // if (reverse)
    // {
    //     for (i = intersection.size() - 1; i >= 0; --i)
    //     {
    //         intersection[i]->prev = temp;
    //         temp->next = intersection[i];
    //         temp = intersection[i];
    //     }
    //     n->prev = intersection[0];
    //     intersection[0]->next = n;
    // }
    // else
    // {
    //     for (i = 0; i < intersection.size(); ++i)
    //     {
    //         intersection[i]->prev = temp;
    //         temp->next = intersection[i];
    //         temp = intersection[i];
    //     }

    //     n->prev = intersection[--i];
    //     intersection[i]->next = n;
    // }
}

void point::sort_asc()
{
    if (verti)
    {
        for (int i = 1; i < intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (j >= 0 && key->y < intersection[j]->y)
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
            while (j >= 0 && key->x < intersection[j]->x)
            {
                intersection[j + 1] = intersection[j];
                --j;
            }
            intersection[j + 1] = key;
        }
    }
}

void point::sort_dsc()
{
    if (verti)
    {
        for (int i = 1; i < intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (j >= 0 && key->y > intersection[j]->y)
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
            while (j >= 0 && key->x > intersection[j]->x)
            {
                intersection[j + 1] = intersection[j];
                --j;
            }
            intersection[j + 1] = key;
        }
    }
}

void point::sort_intersection()
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
        sort_dsc();
    }
    else
    {
        sort_asc();
    }

    // cout << "***************************************" << endl;
    // cout << *this << *(this->next) << endl;
    // for (signed int i = 0; i < intersection.size(); ++i)
    //     cout << *(intersection[i]) << " ";
    // cout << endl;
    // cout << "***************************************" << endl;
}
// 把整個多邊形刪掉，需要刪掉全部的 頂點 和 交點
void point::delete_poly()
{
    point *p = this->next;
    // cout << "POLY" << *p << "LEN" << len << endl;
    for (unsigned int i = 0; i < len - 1; ++i)
    {
        p = p->next;
        // cout << "DELoo " << *(p->prev) << endl;
        delete p->prev;
    }
}

class intersect_point : public point
{
private:
public:
    intersect_point(long long, long long, int);
    ~intersect_point();
    bool ispoint() { return false; };
    void print();
    intersect_point *cross_point;
    int color;
    bool in;
    bool tran;
};

intersect_point::intersect_point(long long xt, long long yt, int colort) : point(xt, yt), color(colort), in(true)
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

void intersect_point::print()
{
    cout << "II " << *this << (in ? "  in" : " out") << endl;
}