#include <iostream>
using namespace std;

class A{
    public:
        A(int xt) : x(xt){};
        int x;

};
bool operator < (A* q, A* p) const { cout << "CMP" << endl;return q->x < p->x; }

int main(){
    A* p1 = new A(3);
    A* p2 = new A(3);
    A b(2);
    cout << (p1 < p2) << endl;
}