#include <iostream>
#include "karp.h"
#include <cstdlib>
#include <ctime>
using namespace std;

int main()
{
    srand(time(0));
    vector<vector<bool> > adj;
    for (unsigned int i = 0; i < 7; ++i)
    {
        vector<bool> temp;
        for (unsigned int j = 0; j < 5; ++j)
        {
            bool ins = rand() % 2;
            temp.push_back(ins);
            cout << ins << ' ';
        }
        cout << '\n';
        adj.push_back(temp);
    }
    HopcroftKarp karp;
    karp.run(adj);
    return 0;
}