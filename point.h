#include <iostream>
using namespace std;
#define DIV 922337203685477580.0
class point{
    private:
    protected:
    public:
        point();
        point(long long, long long);
        ~point();
        float area(point* p){return x/DIV*(p->y)/DIV - y/DIV*(p->x)/DIV;}
        void print();
        void swap_dir(){swap(next,prev);};
        int len;
        long long x,y;
        point* next;
        point* prev;
};

point::point(long long xt, long long yt) : x(xt),y(yt){
    // cout << "point construct" << endl;
    // cout << "x & y:" << x << " " << y << endl;
}

point::point(){

}

point::~point(){

}

void point::print(){
    cout << x << " " << y << endl;
}

class intersect_point : public point{
    private:
        point* cross_point;
    public:
        intersect_point(/* args */);
        ~intersect_point();
};

intersect_point::intersect_point(/* args */){
    cout << "point construct" << endl;
}

intersect_point::~intersect_point(){

}