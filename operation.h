#ifndef OPERATION_H
#define OPERATION_H
#include "glob_func.h"
#include <set>
// #define DEBUG
using namespace std;

class operation
{
private:
protected:
public:
    operation(){};
    operation(string s) : name(s){};
    ~operation(){};
    string name;
    vector<point *> root_list; // 存該 operation 下的所有多邊形
};
#endif