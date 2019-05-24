#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include "point.h"
#define DEBUG
using namespace std;

inline bool read_operation(fstream&, point**);

int main(){
    clock_t t = clock();
    // can choose file sample_in.txt / OpenCase_1.txt / OpenCase_2.txt
    string filename = "sample_in.txt";
    fstream fin(filename.c_str(), fstream::in);
    
    // read operation
    vector<string> operation;
    string s;
    fin >> s;
    while(1){
        fin >> s;
        if(s == ";")
            break;
        operation.push_back(s);
    }

    // print operation
    #ifdef DEBUG
    cout << "All operations: ";
    for(int i = 0 ; i < operation.size() ; ++i)
        cout << operation[i] << " ";
    cout << endl;
    #endif

    // read dummy line
    for(int i = 0 ; i < operation.size() ; ++i){
        #ifdef DEBUG
        cout << "=============================" << endl;
        cout << operation[i] << endl;
        #endif
        if(operation[i] == "SV" || operation[i] == "SH" || operation[i] == "SO"){
            #ifdef DEBUG
            cout << "Start splitting" << endl;
            #endif
            break;
        }
        fin >> s >> s >> s >> s;
        // read points and construct list
        point* root;
        while(read_operation(fin, &root)){
            #ifdef DEBUG
            point* p = root;
            for(int i = 0 ; i < root->len ; ++i){
                p->print();
                p = p->next;
            }
            #endif
        }
    }
    // cout << endl << "Consuming " << int((clock() - t)  / (double) CLOCKS_PER_SEC) << " s in total" << endl;
}

inline bool read_operation(fstream& fin, point** root){
    // read points and construct polygon
    string s, s2;

    // read dummy POLYGON
    fin >> s;
    if(s == "END" && fin >> s) return false;

    // read first 2 dummy int
    fin >> s >> s;

    // read root
    fin >> s >> s2;
    point* p;
    point* prev = new point((long long)(atoi(s.c_str())),(long long)(atoi(s2.c_str())));
    *root = prev;
    int cnt = 1;

    // area for detecting if clockwise
    float area = 0;
    while(fin >> s && s != ";" && fin >> s2){
        p = new point((long long)(atoi(s.c_str())),(long long)(atoi(s2.c_str())));
        p->prev = prev;
        prev->next = p;
        area += prev->area(p);
        prev = p;
        ++cnt;
    }
    p->next = *root;
    (*root)->prev = p;
    (*root)->len = cnt;
    area += p->area(*root);

    // see if it is clock wise or counter clockwise
    // cout << ((area > 0) ? "Counter-clockwise" : "Clockwise") << endl;

    // if it is clockwise, reverse it
    if(area < 0){
        point* p = *root;
        for(int i = 0 ; i < (*root)->len ; ++i){
            p->swap_dir();
            p = p->prev;
        }
    }
    return true;
}