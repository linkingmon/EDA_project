#include <iostream>
#include <vector>
#include <set>
#include <iomanip>
using namespace std;
#define DIV 922337203685477580.0
class point
{
private:
protected:
public:
    point();
    point(long long xt, long long yt);
    ~point();
    float area(point *p) { return x / DIV * (p->y) / DIV - y / DIV * (p->x) / DIV; }
    // virtual void print() { cout << x << " " << y << endl; }
    void swap_dir() { swap(next, prev); };
    void connect();
    void sort_intersection();
    friend ostream &operator<<(ostream &os, const point &p);
    virtual void print() { cout << *this << endl; };

    vector<point *> intersection;
    long long x, y;
    point *next;
    point *prev;
    point *s_next;
    int len;
    // point* straright_next;
    short angle;
    bool verti;
    // point &bool operator<(point &b);
};

ostream &operator<<(ostream &os, const point &p) { os << '(' << setw(4) << p.x << ',' << setw(4) << p.y << ')'; };

point::point(long long xt, long long yt) : x(xt), y(yt)
{
    // cout << "point construct" << endl;
    // cout << "x & y:" << x << " " << y << endl;
    verti = false;
}

point::point()
{
}

point::~point()
{
}

// bool operator < (point* a,point* b)

void point::connect()
{
    point *n = next;
    point *temp = this;
    if (intersection.size() == 0)
        return;
    sort_intersection();
    bool reverse = false;
    int i;
    if (verti)
    {
        if (y > next->y)
            reverse = true;
    }
    else if (x > next->x)
        reverse = true;
    if (reverse)
    {
        for (i = intersection.size() - 1; i >= 0; --i)
        {
            intersection[i]->prev = temp;
            temp->next = intersection[i];
            temp = intersection[i];
        }
        n->prev = intersection[0];
        intersection[0]->next = n;
    }
    else
    {
        for (i = 0; i < intersection.size(); ++i)
        {
            intersection[i]->prev = temp;
            temp->next = intersection[i];
            temp = intersection[i];
        }

        n->prev = intersection[--i];
        intersection[i]->next = n;
    }
    // set<point *>::iterator iter = intersection.begin();
    // for (; iter != intersection.end(); ++iter)
    // {
    //     (*iter)->prev = temp;
    //     temp->next = (*iter);
    //     temp = (*iter);
    // }
    // n->prev = *(--iter);
    // (*iter)->next = n;
    // intersection.clear();
}
// void point::print(){
//     cout << x << " " << y << endl;
//     // cout << "intersection number is " << intersection.size() << endl;
//     // for (int i = 0; i < intersection.size();++i){
//     //     cout << "intersection is : " << endl;
//     //     cout << intersection[i]->x << " " << intersection[i]->y<<endl;
//     // }
// }

void point::sort_intersection()
{
    if (verti)
    {
        for (int i = 1; i < intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (key->y < intersection[j]->y && j > 0)
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
            while (key->x < intersection[j]->x && j > 0)
            {
                intersection[j + 1] = intersection[j];
                --j;
            }
            intersection[j + 1] = key;
        }
    }
}
class intersect_point : public point
{
private:
public:
    intersect_point(/* args */);
    intersect_point(long long, long long);
    ~intersect_point();
    intersect_point *cross_point;
    void print();
    bool in;
};

intersect_point::intersect_point(/* args */)
{
    cout << "point construct" << endl;
}

intersect_point::intersect_point(long long xt, long long yt) : point(xt, yt)
{
    // cout << "point construct" << endl;
    // cout << "x & y:" << x << " " << y << endl;
}

intersect_point::~intersect_point()
{
}

void intersect_point::print()
{
    cout << "II " << *this << (in ? "  in" : " out") << endl;
}