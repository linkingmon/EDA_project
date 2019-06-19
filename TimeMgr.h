#ifndef TIMEMGR_H
#define TIMEMGR_N
#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
using namespace std;

class TimeMgr
{
public:
    TimeMgr(){};
    ~TimeMgr(){};
    void tic();
    void toc(string s);
    void print();
    void output();

private:
    map<string, vector<double> > times;
    clock_t t;
};
void TimeMgr::tic()
{
    t = clock();
}
void TimeMgr::toc(string s)
{
    times[s].push_back(((float)(clock() - t)) / CLOCKS_PER_SEC);
}
void TimeMgr::print()
{
    cout << "============================================" << endl;
    for (map<string, vector<double> >::iterator iter = times.begin(); iter != times.end(); ++iter)
    {
        double sum = 0;
        for (unsigned int i = 0; i < iter->second.size(); ++i)
            sum += iter->second[i];
        cout << iter->first << " consumes " << int(sum) << " seconds." << endl;
    }
    cout << "============================================" << endl;
}
void TimeMgr::output()
{
    fstream timefout("time.txt", fstream::out);
    for (map<string, vector<double> >::iterator iter = times.begin(); iter != times.end(); ++iter){
        cerr << iter->first << endl;
        timefout << iter->first << ", ";
        cerr << iter->second.size() << endl;
        for(unsigned int j = 0 ; j < iter->second.size() ; ++j){
             timefout << iter->second[j] << " ,";
        }
         timefout << endl;
    }
}
#endif