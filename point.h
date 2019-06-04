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
    point(){};
    point(long long xt, long long yt);
    ~point(){};
    float area(point *p) { return x / DIV * (p->y) / DIV - y / DIV * (p->x) / DIV; }
    // virtual void print() { cout << x << " " << y << endl; }
    void swap_dir() { swap(next, prev); };
    void connect();
    void sort_intersection();
    void sort_asc();
    void sort_dsc();
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
    // point &bool operator<(point &b);
};

ostream &operator<<(ostream &os, const point &p) { os << '(' << setw(4) << p.x << ',' << setw(4) << p.y << ')';
    return os;

};

point::point(long long xt, long long yt) : x(xt), y(yt)
{
    verti = false;
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

void point::sort_asc(){
    if (verti){
        for (int i = 1; i < intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (j >= 0 && key->y < intersection[j]->y )
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
            while (j >= 0 && key->x < intersection[j]->x )
            {
                intersection[j + 1] = intersection[j];
                --j;
            }
            intersection[j + 1] = key;
        }
    }
}

void point::sort_dsc(){
    if (verti){
        for (int i = 1; i < intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (j >= 0 && key->y > intersection[j]->y )
            {
                intersection[j + 1] = intersection[j];
                --j;
            }
            intersection[j + 1] = key;
        }
    }
    else
    {
        for (int i = 1; i > intersection.size(); ++i)
        {
            point *key = intersection[i];
            int j = i - 1;
            while (j >= 0 && key->x > intersection[j]->x )
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
    int i;
    if (verti)
    {
        if (y > next->y)
            reverse = true;
    }
    else if (x > next->x)
        reverse = true;
    if(reverse){
        sort_dsc();
    }
    else {
        sort_asc();
    }
}
class intersect_point : public point
{
private:
public:
    intersect_point(/* args */);
    intersect_point(long long, long long, int);
    ~intersect_point();
    bool ispoint() { return false; };
    void print();
    intersect_point *cross_point;
    int color;
    bool in;
};

intersect_point::intersect_point(/* args */)
{
    cout << "point construct" << endl;
}

intersect_point::intersect_point(long long xt, long long yt, int colort) : point(xt, yt), color(colort)
{
}

intersect_point::~intersect_point()
{
}

void intersect_point::print()
{
    cout << "II " << *this << (in ? "  in" : " out") << endl;
}