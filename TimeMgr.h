#ifndef TIMEMGR_H
#define TIMEMGR_H
#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iomanip>
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
    map<string, vector<clock_t>> times;
    clock_t t;
};
void TimeMgr::tic()
{
    t = clock();
}
void TimeMgr::toc(string s)
{
    times[s].push_back(clock() - t);
}
void TimeMgr::print()
{
    cout << "===========================================================" << endl;
    clock_t total_sum = 0;
    for (map<string, vector<clock_t>>::iterator iter = times.begin(); iter != times.end(); ++iter)
    {
        clock_t sum = 0;
        for (unsigned int i = 0; i < iter->second.size(); ++i)
            sum += iter->second[i];
        total_sum += sum;
        cout << setw(25) << left << iter->first << " consumes " << setw(8) << right << double(sum) / CLOCKS_PER_SEC << " seconds." << endl;
    }
    cout << "-----------------------------------------------------------" << endl;
    cout << setw(25) << left << "All"
         << " consumes " << setw(8) << right << double(total_sum) / CLOCKS_PER_SEC << " seconds." << endl;
    cout << "===========================================================" << endl;
}
void TimeMgr::output()
{
    fstream timefout("time.csv", fstream::out);
    for (map<string, vector<clock_t>>::iterator iter = times.begin(); iter != times.end(); ++iter)
    {
        timefout << iter->first << ", ";
        for (unsigned int j = 0; j < iter->second.size(); ++j)
        {
            timefout << iter->second[j] << " ,";
        }
        timefout << endl;
    }
}
#endif