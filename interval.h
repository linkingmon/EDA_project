#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "glob_func.h"
using namespace std;

/**********************************/
/*          Declarations          */
/**********************************/
class Interval;
class Interval_Mgr;
class Rectangle;
bool compare_Interval (Interval* a, Interval* b);


class Interval
{
public:
    Interval(long long left, long long right, long long position, bool reverse): _left(left), _right(right), _position(position), _reverse(reverse) {}
    ~Interval() {}
    long long get_left() const { return _left;}
    long long get_right() const { return _right;}
    long long get_position() const { return _position;}
    bool is_reverse() const { return _reverse; }
private:
    long long _left;
    long long _right;
    long long _position;
    bool _reverse;
};
ostream &operator<<(ostream &os, const Interval &p)
{
    os << "Interval= [" << setw(4) << p.get_left() << ',' << setw(4) << p.get_right() << ']';
    os << ", position = " << p.get_position() << ", reverse = " << p.is_reverse();
    return os;
};

class Interval_Mgr
{
public:
    ~Interval_Mgr() { clear(); }
    void build_interval( point* &p, bool direction);
    void split_polygon();
    bool find_intersections(Interval* &p);
    void combine_interval(Interval* &p);
    void add_interval(Interval* &p);
    void add_rectangle(Interval* &p);
    void clear();
private:
    vector<Interval*> _intervalList;
    vector<Interval*> _currentList;
    vector<size_t> _intersections;
    vector<Rectangle> _rectList;
    bool _direction; // 0 horizontal; 1 vertical;
};

class Rectangle
{
public:
    Rectangle(long long LLX, long long LLY, long long URX, long long URY): _LLX(LLX), _LLY(LLY), _URX(URX), _URY(URY) { }
    friend ostream& operator << (ostream&, const Rectangle&);
private:
    long long _LLX;
    long long _LLY;
    long long _URX;
    long long _URY;

};

void Interval_Mgr::clear()
{
    for (size_t i=0; i<_intervalList.size(); i++){
        delete _intervalList[i]; 
        _intervalList[i] = 0;
    }
}


void Interval_Mgr::build_interval( point* &p, bool direction)
{
    _direction = direction;
    size_t n_vertex = p->len;
    // for (size_t i=0; i<n_vertex; i++){
    //     cout << *p << endl;
    //     p = p->next;
    // }
    // p = p->prev;
    if (_direction==0){ //horizontal
        if (p->verti){
            for (size_t i=0; i<n_vertex; i+=2){
                cout << *p->prev << " " << *p << endl;
                if (p->prev->x < p->x){
                    _intervalList.push_back(new Interval(p->prev->x, p->x, p->y, 0));
                } else {
                    _intervalList.push_back(new Interval(p->x, p->prev->x, p->y, 1));
                }
                p = p->next->next;
            }
        } else {
            for (size_t i=0; i<n_vertex; i+=2){
                cout << *p << " " << *p->next << endl;
                if (p->x < p->next->x){
                    _intervalList.push_back(new Interval(p->x, p->next->x, p->y, 0));
                } else {
                    _intervalList.push_back(new Interval(p->next->x, p->x, p->y, 1));
                }
                p = p->next->next;
            }
        }
    } else { // vertical
        if (p->verti){
            for (size_t i=0; i<n_vertex; i+=2){
                cout << *p << " " << *p->next << endl;
                if (p->y > p->next->y){
                    _intervalList.push_back(new Interval(p->next->y, p->y, p->x, 0));
                } else {
                    _intervalList.push_back(new Interval(p->y, p->next->y, p->x, 1));
                }
                p = p->next->next;
            }
        } else {
            for (size_t i=0; i<n_vertex; i+=2){
                cout << *p->prev << " " << *p << endl;
                if (p->prev->y > p->y){
                    _intervalList.push_back(new Interval(p->y, p->prev->y, p->x, 0));
                } else {
                    _intervalList.push_back(new Interval(p->prev->y, p->y, p->x, 1));
                }
                p = p->next->next;
            }
        }
    }
    sort(_intervalList.begin(), _intervalList.end(), ::compare_Interval);
    for (size_t i=0; i<_intervalList.size(); i++){
        cout << *_intervalList[i] << endl;
    }

}

void Interval_Mgr::split_polygon()
{
    for(size_t i=0; i<_intervalList.size(); i++){
        Interval *I = _intervalList[i];
        if(find_intersections(I)){
            add_rectangle(I);
            combine_interval(I);
        } else {
            add_interval(I);
        }
        for (size_t i=0; i<_currentList.size(); i++){
            cout << *_currentList[i] << endl;
        }
    }
    

}

bool Interval_Mgr::find_intersections(Interval* &p)
{
    _intersections.clear();
    for (size_t i=0; i<_currentList.size(); i++){
        if (p->get_right()>=_currentList[i]->get_left() && _currentList[i]->get_right()>=p->get_left()){
            _intersections.push_back(i);
        }
    }
    cout << "Intersections: ";
    for (size_t i=0; i<_intersections.size(); i++){
        cout << _intersections[i] << " ";
    }
    cout << endl;
    if (_intersections.empty()) return 0;
    return 1;

}

void Interval_Mgr::combine_interval(Interval* &p)
{
    if (p->is_reverse()){
        assert(_intersections.size()==1);
        Interval* I=_currentList[_intersections[0]];
        _currentList.erase(_currentList.begin()+_intersections[0]);
        if (I->get_left()!=p->get_left()){
            _currentList.push_back(new Interval(I->get_left(), p->get_left(), p->get_position(), 0));
        }
        if (I->get_right()!=p->get_right()){
            _currentList.push_back(new Interval(p->get_right(), I->get_right(), p->get_position(), 0));
        }
        delete I;
    } else {
        assert(_intersections.size()<=2);
        if (_intersections.size()==1){
            Interval* I=_currentList[_intersections[0]];
            _currentList.erase(_currentList.begin()+_intersections[0]);
            if (I->get_left()==p->get_right()){
                _currentList.push_back(new Interval(p->get_left(), I->get_right(), p->get_position(), 0));
            } else if (p->get_left()==I->get_right()){
                _currentList.push_back(new Interval(I->get_left(), p->get_right(), p->get_position(), 0));
            } else {
                cerr << "The two intervals can not combine." << endl;
            }
            delete I;
        } else if (_intersections.size()==2){
            Interval* I1=_currentList[_intersections[0]];
            Interval* I2=_currentList[_intersections[1]];
            _currentList.erase(_currentList.begin()+_intersections[1]);
            _currentList.erase(_currentList.begin()+_intersections[0]);
            if(I1->get_left()<I2->get_left()){
                _currentList.push_back(new Interval(I1->get_left(), I2->get_right(), p->get_position(), 0));
            } else {
                _currentList.push_back(new Interval(I2->get_left(), I1->get_right(), p->get_position(), 0));
            }
            delete I1;
            delete I2;
        }
    }
}

void Interval_Mgr::add_interval(Interval* &p)
{
    _currentList.push_back(new Interval(*p));

}

void Interval_Mgr::add_rectangle(Interval* &p)
{
    if (_direction==0){
        for(size_t i=0; i<_intersections.size(); i++){
            Interval* I=_currentList[_intersections[i]];
            if(p->get_position()>I->get_position()){
                _rectList.push_back(Rectangle(I->get_left(), I->get_position(), I->get_right(), p->get_position()));
                cout << _rectList.back();
            }
        }
    } else {
        for(size_t i=0; i<_intersections.size(); i++){
            Interval* I=_currentList[_intersections[i]];
            if(p->get_position()>I->get_position()){
                _rectList.push_back(Rectangle(I->get_position(), I->get_left(), p->get_position(), I->get_right()));
                cout << _rectList.back();
            }
        }
    }

}

bool compare_Interval (Interval* a, Interval* b) { return a->get_position()<b->get_position(); }

ostream& operator << (ostream& os, const Rectangle& rect)
{
    os << "RECT " << rect._LLX << " " << rect._LLY << " " << rect._URX << " " << rect._URY  << endl;
}