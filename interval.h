#ifndef INTERVAL_H
#define INTERVAL_H
#include <vector>
#include <string>
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
class Diagonal;
bool compare_Interval(Interval *a, Interval *b);
bool compare_x(point *a, point *b);
bool compare_y(point *a, point *b);

class Interval
{
public:
    Interval(long long left, long long right, long long position, long long left_pos, long long right_pos, bool reverse): 
            _left(left), _right(right), _position(position), _left_pos(left_pos), _right_pos(right_pos), _reverse(reverse) {}
    ~Interval() {}
    long long get_left() const { return _left;}
    long long get_right() const { return _right;}
    long long get_left_pos() const { return _left_pos;}
    long long get_right_pos() const { return _right_pos;}
    long long get_position() const { return _position;}
    bool is_reverse() const { return _reverse; }

private:
    long long _left;
    long long _right;
    long long _position;
    long long _left_pos;
    long long _right_pos;
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
    void build_interval(vector<point *> &rootList, bool direction);
    void split_polygon();
    bool find_intersections(Interval *&p);
    void combine_interval(Interval *&p);
    void add_interval(Interval *&p);
    void add_rectangle(Interval *&p);
    void move_rectangle(vector<Rectangle *> &totalList);
    void find_diagonal(vector<vector<Diagonal>> &diagonalList, vector<point *> &concaveList, vector<point *> &polygon);
    void clear();

private:
    vector<Interval *> _intervalList;
    vector<Interval *> _currentList;
    vector<size_t> _intersections;
    vector<Rectangle *> _rectList;
    bool _direction; // 0 horizontal; 1 vertical;
};

class Rectangle
{
public:
    Rectangle(long long LLX, long long LLY, long long URX, long long URY) : _LLX(LLX), _LLY(LLY), _URX(URX), _URY(URY) {}
    friend ostream &operator<<(ostream &, const Rectangle &);

private:
    long long _LLX;
    long long _LLY;
    long long _URX;
    long long _URY;
};

class Diagonal
{
public:
    Diagonal(point *p1, point *p2, bool vertical) : _p1(p1), _p2(p2), _vertical(vertical), _is_matched(false), _in_min_cover(false) {}
    friend ostream &operator<<(ostream &, const Diagonal &);

    point *_p1;
    point *_p2;
    bool _vertical;
    bool _is_matched;
    bool _in_min_cover;
};

void Interval_Mgr::clear()
{
    for (size_t i = 0; i < _intervalList.size(); i++)
    {
        delete _intervalList[i];
        _intervalList[i] = 0;
    }
    for (size_t i = 0; i < _rectList.size(); i++)
    {
        if (_rectList[i] != 0)
        {
            delete _rectList[i];
            _rectList[i] = 0;
        }
    }
    _intervalList.clear();
    _rectList.clear();
}

void Interval_Mgr::build_interval(vector<point *> &rootList, bool direction)
{
    _direction = direction;
    for (size_t pi = 0; pi < rootList.size(); pi++)
    {
        point *p = rootList[pi];
        size_t n_vertex = p->len;
        // for (size_t i=0; i<n_vertex; i++){
        //     cout << *p << endl;
        //     p = p->next;
        // }
        // p = p->prev;
        if (_direction == 0)
        { //horizontal
            if (p->verti)
            {
                for (size_t i = 0; i < n_vertex; i += 2)
                {
                    // cout << *p->prev << " " << *p << endl;
                    if (p->prev->x < p->x){
                        _intervalList.push_back(new Interval(p->prev->x, p->x, p->y, p->prev->prev->y, p->next->y, 0));
                    } else {
                        _intervalList.push_back(new Interval(p->x, p->prev->x, p->y, p->next->y, p->prev->prev->y, 1));
                    }
                    p = p->next->next;
                }
            }
            else
            {
                for (size_t i = 0; i < n_vertex; i += 2)
                {
                    // cout << *p << " " << *p->next << endl;
                    if (p->x < p->next->x){
                        _intervalList.push_back(new Interval(p->x, p->next->x, p->y, p->prev->y, p->next->next->y, 0));
                    } else {
                        _intervalList.push_back(new Interval(p->next->x, p->x, p->y, p->next->next->y, p->prev->y, 1));
                    }
                    p = p->next->next;
                }
            }
        }
        else
        { // vertical
            if (p->verti)
            {
                for (size_t i = 0; i < n_vertex; i += 2)
                {
                    // cout << *p << " " << *p->next << endl;
                    if (p->y > p->next->y){
                        _intervalList.push_back(new Interval(p->next->y, p->y, p->x, p->next->next->x, p->prev->x, 0));
                    } else {
                        _intervalList.push_back(new Interval(p->y, p->next->y, p->x, p->prev->x, p->next->next->x, 1));
                    }
                    p = p->next->next;
                }
            }
            else
            {
                for (size_t i = 0; i < n_vertex; i += 2)
                {
                    // cout << *p->prev << " " << *p << endl;
                    if (p->prev->y > p->y){
                        _intervalList.push_back(new Interval(p->y, p->prev->y, p->x, p->next->x, p->prev->prev->x, 0));
                    } else {
                        _intervalList.push_back(new Interval(p->prev->y, p->y, p->x, p->prev->prev->x, p->next->x, 1));
                    }
                    p = p->next->next;
                }
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
    for (size_t i = 0; i < _intervalList.size(); i++)
    {
        Interval *I = _intervalList[i];
        if (find_intersections(I))
        {
            add_rectangle(I);
            combine_interval(I);
        }
        else
        {
            add_interval(I);
        }
        for (size_t i = 0; i < _currentList.size(); i++)
        {
            // cout << *_currentList[i] << endl;
        }
    }
}

bool Interval_Mgr::find_intersections(Interval *&p)
{
    _intersections.clear();
    if (p->is_reverse())
    {
        for (size_t i=0; i<_currentList.size(); i++)
        {
            if (p->get_right()<=_currentList[i]->get_right() && p->get_left()>=_currentList[i]->get_left())
            {
                _intersections.push_back(i);
            }
        }
    } else {
        for (size_t i=0; i<_currentList.size(); i++)
        {
            if (p->get_left() == _currentList[i]->get_right() && _currentList[i]->get_right_pos() == p->get_position())
            {
                _intersections.push_back(i);
            } else if (p->get_right() == _currentList[i]->get_left() && _currentList[i]->get_left_pos() == p->get_position()){
                _intersections.push_back(i);
            }
        }
    }
    
    
    // cout << "Intersections: ";
    // for (size_t i=0; i<_intersections.size(); i++){
    //     cout << _intersections[i] << " ";
    // }
    // cout << endl;
    if (_intersections.empty())
        return 0;
    return 1;
}

void Interval_Mgr::combine_interval(Interval *&p)
{
    if (p->is_reverse()){
        assert(_intersections.size()==1);
        Interval* I=_currentList[_intersections[0]];
        _currentList.erase(_currentList.begin()+_intersections[0]);
        if (I->get_left()!=p->get_left()){
            _currentList.push_back(new Interval(I->get_left(), p->get_left(), p->get_position(), I->get_left_pos(), p->get_left_pos(), 0));
        }
        if (I->get_right()!=p->get_right()){
            _currentList.push_back(new Interval(p->get_right(), I->get_right(), p->get_position(), p->get_right_pos(), I->get_right_pos(), 0));
        }
        delete I;
    } else {
        assert(_intersections.size()<=2);
        if (_intersections.size()==1){
            Interval* I=_currentList[_intersections[0]];
            _currentList.erase(_currentList.begin()+_intersections[0]);
            if (I->get_left()==p->get_right()){
                _currentList.push_back(new Interval(p->get_left(), I->get_right(), p->get_position(), p->get_left_pos(), I->get_right_pos(), 0));
            } else if (p->get_left()==I->get_right()){
                _currentList.push_back(new Interval(I->get_left(), p->get_right(), p->get_position(), I->get_left_pos(), p->get_right_pos(), 0));
            } else {
                cerr << "The two intervals can not combine." << endl;
                cerr << *I << endl;
                cerr << *p << endl;
            }
            delete I;
        } else if (_intersections.size()==2){
            Interval* I1=_currentList[_intersections[0]];
            Interval* I2=_currentList[_intersections[1]];
            _currentList.erase(_currentList.begin()+_intersections[1]);
            _currentList.erase(_currentList.begin()+_intersections[0]);
            if(I1->get_left()<I2->get_left()){
                _currentList.push_back(new Interval(I1->get_left(), I2->get_right(), p->get_position(), I1->get_left_pos(), I2->get_right_pos(), 0));
            } else {
                _currentList.push_back(new Interval(I2->get_left(), I1->get_right(), p->get_position(), I2->get_left_pos(), I1->get_right_pos(), 0));
            }
            delete I1;
            delete I2;
        }
    }
}

void Interval_Mgr::add_interval(Interval *&p)
{
    _currentList.push_back(new Interval(*p));
}

void Interval_Mgr::add_rectangle(Interval *&p)
{
    if (_direction == 0)
    {
        for (size_t i = 0; i < _intersections.size(); i++)
        {
            Interval *I = _currentList[_intersections[i]];
            if (p->get_position() > I->get_position())
            {
                _rectList.push_back(new Rectangle(I->get_left(), I->get_position(), I->get_right(), p->get_position()));
                // cout << *_rectList.back() << endl;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < _intersections.size(); i++)
        {
            Interval *I = _currentList[_intersections[i]];
            if (p->get_position() > I->get_position())
            {
                _rectList.push_back(new Rectangle(I->get_position(), I->get_left(), p->get_position(), I->get_right()));
                // cout << *_rectList.back() << endl;
            }
        }
    }
}

void Interval_Mgr::move_rectangle(vector<Rectangle *> &totalList)
{
    for (size_t i = 0; i < _rectList.size(); i++)
    {
        totalList.push_back(_rectList[i]);
        _rectList[i] = 0;
    }
}

void Interval_Mgr::find_diagonal(vector<vector<Diagonal>> &diagonalList, vector<point *> &concaveList, vector<point *> &polygon)
{
    build_interval(polygon, 1);
    sort(concaveList.begin(), concaveList.end(), ::compare_x);
    size_t k = 0;
    Interval *I = _intervalList[k];
    vector<Diagonal> d;
    for (size_t i = 0; i < concaveList.size() - 1; i++)
    {
        for (size_t j = i + 1; j < concaveList.size(); j++)
        {
            if (concaveList[i]->x != concaveList[j]->x)
                break;
            // cout << *concaveList[i] << *concaveList[j] << endl;
            while (I->get_position() < concaveList[i]->x || (I->get_position() == concaveList[i]->x && I->is_reverse()))
            {
                if (find_intersections(I))
                {
                    combine_interval(I);
                }
                else
                {
                    add_interval(I);
                }
                // for (size_t idx=0; idx<_currentList.size(); idx++){
                //     cout << *_currentList[idx] << endl;
                // }
                I = _intervalList[++k];
            }
            for (size_t idx = 0; idx < _currentList.size(); idx++)
            {
                if (_currentList[idx]->get_left() == concaveList[i]->y && _currentList[idx]->get_right() == concaveList[j]->y)
                {
                    d.push_back(Diagonal(concaveList[i], concaveList[j], 1));
                    // cout << d.back() << endl;
                    break;
                }
            }
        }
    }
    while (k < _intervalList.size())
    {
        I = _intervalList[k];
        if (find_intersections(I))
        {
            combine_interval(I);
        }
        else
        {
            add_interval(I);
        }
        ++k;
        // for (size_t idx=0; idx<_currentList.size(); idx++){
        //     cout << *_currentList[idx] << endl;
        // }
    }
    diagonalList.push_back(d);
    d.clear();
    clear();

    build_interval(polygon, 0);
    sort(concaveList.begin(), concaveList.end(), ::compare_y);
    k = 0;
    I = _intervalList[k];
    for (size_t i = 0; i < concaveList.size() - 1; i++)
    {
        for (size_t j = i + 1; j < concaveList.size(); j++)
        {
            if (concaveList[i]->y != concaveList[j]->y)
                break;
            // cout << *concaveList[i] << *concaveList[j] << endl;
            while (I->get_position() < concaveList[i]->y || (I->get_position() == concaveList[i]->y && I->is_reverse()))
            {
                if (find_intersections(I))
                {
                    combine_interval(I);
                }
                else
                {
                    add_interval(I);
                }
                // for (size_t idx=0; idx<_currentList.size(); idx++){
                //     cout << *_currentList[idx] << endl;
                // }
                I = _intervalList[++k];
            }
            for (size_t idx = 0; idx < _currentList.size(); idx++)
            {
                if (_currentList[idx]->get_left() == concaveList[i]->x && _currentList[idx]->get_right() == concaveList[j]->x)
                {
                    d.push_back(Diagonal(concaveList[i], concaveList[j], 0));
                    cout << d.back() << endl;
                    break;
                }
            }
        }
    }
    while (k < _intervalList.size())
    {
        I = _intervalList[k];
        if (find_intersections(I))
        {
            combine_interval(I);
        }
        else
        {
            add_interval(I);
        }
        ++k;
        // for (size_t idx=0; idx<_currentList.size(); idx++){
        //     cout << *_currentList[idx] << endl;
        // }
    }
    diagonalList.push_back(d);
    d.clear();
    clear();
}

bool compare_Interval(Interval *a, Interval *b)
{
    if (a->get_position() != b->get_position())
        return a->get_position() < b->get_position();
    if (a->is_reverse() && !b->is_reverse())
        return true;
    return false;
}

bool compare_x(point *a, point *b)
{
    if (a->x != b->x)
        return a->x < b->x;
    return a->y < b->y;
}

bool compare_y(point *a, point *b)
{
    if (a->y != b->y)
        return a->y < b->y;
    return a->x < b->x;
}
ostream &operator<<(ostream &os, const Rectangle &rect)
{
    os << "RECT " << rect._LLX << " " << rect._LLY << " " << rect._URX << " " << rect._URY << " ;";
}

ostream &operator<<(ostream &os, const Diagonal &diag)
{
    os << "Diagonal " << *diag._p1 << " " << *diag._p2;
}

#endif