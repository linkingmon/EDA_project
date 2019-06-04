#include "point.h"
#include <set>
using namespace std;

static int glob_color = 0;
class operation
{
private:
protected:
public:
    operation(string s) : name(s){};
    ~operation(){};
    void find_intersect();
    void insert_intersect();
    void find_intersect(point *a, point *b);
    void find_cross(point *, point *);
    void add_intersect(point *a, point *b);
    bool inside_edge(long long x1, long long x2, long long y);
    void two_way_intersect(intersect_point *, intersect_point *);
    void new_intersect(point *a, point *b, long long x, long long y);
    bool inside_region(point *a, point *b, long long x, long long y);
    void in_out_cross(point *, point *);
    bool outside_poly(point *, point *);
    string name;
    vector<point *> root_list; // 存該 operation 下的所有多邊形
    set<point *> out_list;
};

void operation::find_intersect()
{
    for (int i = 0; i < root_list.size(); ++i)
        for (int j = i + 1; j < root_list.size(); ++j)
            find_intersect(root_list[i], root_list[j]);
}
//insert intersect in one POLYGON
void operation::insert_intersect()
{
    for (int i = 0; i < root_list.size(); ++i)
    {
        point *a = root_list[i];
        int len = 0;
        for (int j = 0; j < root_list[i]->len; ++j)
        {
            point *temp = a->next;
            a->connect();
            len += a->intersection.size();
            a = temp;
        }
        root_list[i]->len += len;
    }
}

// find cross point in two POLYGON
// 找兩個 多邊形 的交點
void operation::find_intersect(point *a, point *b)
{
    point *p_k = a;
    point *p_l = b;
    // 每兩個頂點都做 找交點
    ++glob_color;
    for (int k = 0; k < a->len; k++)
    {
        for (int l = 0; l < b->len; l++)
        {
            find_cross(p_k, p_l);
            p_l = p_l->next;
        }
        p_k = p_k->next;
    }
    static int num = 0;
    num += 1;
    cout << "X" << num << endl;
    in_out_cross(a, b);
    in_out_cross(b, a);
}

// cross 來 merge root
// 先判斷 root 是不是在 cross 外面
// root 如果在 cross 的邊上就算是在裡面
void operation::in_out_cross(point *root, point *cross)
{
    bool outward = outside_poly(root, cross);
#ifdef DEBUG
    static bool pr = true;
    if (pr)
    {
        cout << "--------detect in out--------" << endl;
        pr = false;
    }
    cout << "detect that " << *root << " is " << (outward ? "outside" : " inside") << " of POLY" << *cross << endl;
#endif
    point *p = root;
    for (int i = 0; i < root->len; i++)
    {
        if (p->intersection.size() > 0)
        {
            p->sort_intersection();
            for (unsigned int i = 0; i < p->intersection.size(); ++i)
            {
                intersect_point *p_t = static_cast<intersect_point *>(p->intersection[i]);
                if (p_t->color == glob_color)
                {

                    p_t->in = outward;
                    outward = !outward;
                    if (outward)
                    {
                        out_list.insert(p->intersection[i]);
                        // cout << *(p->intersection[i]) << " is outward intersect of poly " << *cross << endl;
                    }
                }
            }
        }
        p = p->next;
    }
}

bool operation::outside_poly(point *root, point *cross)
{
    // minx-1 一定在外面, 設定一個在外面的點P(minx-1,XX) 這個點在該點左上角
    // 找 root 和 P 的連線和 cross多邊形 有幾個交點
    // 分別對每一條邊找
    // 怎麼找?
    // (1) 如果是水平線 水平線y小於該點就不用考慮
    // (2) 如果是水平線 水平線y等於該點 判斷該點如果在上面就是
    // (3) 如果是水平線 水平線y大於該點 先往上走到那個y 然後往左走同樣的x 如果有碰到線就是有交點
    //     如果碰到線的"尾"端點 判斷他是哪一種角
    //     如果是左上右下角或不是角 就是+1 ； 如果是右上左下角就+0
    // (4) 如果是垂直線 垂直線x大於該點就不用考慮
    // (5) 如果是垂直線 垂直線x等於該點 判斷該點如果在上面就是
    // (6) 如果是垂直線 垂直線x小於該點 先往左走到那個x 然後往上走同樣的y 如果有碰到線就是有交點
    //     如果碰到線的"尾"端點 判斷他是哪一種角
    //     如果是左上右下角或不是角 就是+1 ； 如果是右上左下角就+0
    point *p = cross;
    int cross_cnt = 0;
    for (unsigned int i = 0; i < cross->len; ++i)
    {
        if (p->verti)
        {
            if (root->x > p->x)
            { // (6)
                long long inter_y = root->y + (root->x - p->x);
                if ((p->y < inter_y) && (inter_y < p->next->y) || (p->y > inter_y) && (inter_y > p->next->y))
                    cross_cnt += 1;
                else if (inter_y == p->y && p->angle != 1)
                    cross_cnt += 1;
            }
            else if (root->x == p->x)
                if ((root->y >= p->y) && (root->y <= p->s_next->y) || (root->y <= p->y) && (root->y >= p->s_next->y))
                    return false;
        }
        else
        {
            if (root->y < p->y)
            { // (3)
                long long inter_x = root->x - (p->y - root->y);
                if ((p->x < inter_x) && (inter_x < p->next->x) || (p->x > inter_x) && (inter_x > p->next->x))
                    cross_cnt += 1;
                else if (inter_x == p->x && p->angle != 1)
                    cross_cnt += 1;
            }
            else if (root->y == p->y)
                if ((root->x >= p->x) && (root->x <= p->s_next->x) || (root->x <= p->x) && (root->x >= p->s_next->x))
                    return false;
        }
        p = p->next;
    }
    if (cross_cnt % 2)
        return false;
    else
        return true;
}

// given two point a, b ; find cross point between a, a->next ,b ,b->bext
void operation::find_cross(point *a, point *b)
{
    long long x, y;
    bool verti_a = a->verti;
    bool verti_b = b->verti;
    point *a_next = a->next;
    point *b_next = b->next;

    // if(a_next->x == a->x)
    //     verti_a = true;

    // if(b_next->x == b->x)
    //     verti_b = true;

    if (verti_a != verti_b)
    {
        x = verti_a ? a->x : b->x;
        y = verti_b ? a->y : b->y;
        if (inside_region(a, b, x, y))
        {
            new_intersect(a, b, x, y);
        }
    }
}

// add intersect b after point a
void operation::add_intersect(point *a, point *b)
{
    a->intersection.push_back(b);
}

// check whether x is inside x1 and x2 or not
bool operation::inside_edge(long long x1, long long x2, long long x)
{
    if (x >= x1 && x <= x2)
    {
        return true;
    }
    else if (x <= x1 && x >= x2)
    {
        return true;
    }
    else
        return false;
}

// to check (x,y) is inside the region a , a->next , b , b->next
// if (x,y) is on the (a->next and b->next) return false
bool operation::inside_region(point *a, point *b, long long x, long long y)
{
    if (a->verti)
    {
        if (y == a->next->y && x == b->next->x)
            return false;
        bool b_b = inside_edge(b->x, b->next->x, x);
        bool b_c = inside_edge(a->y, a->next->y, y);
        return (b_b && b_c);
    }
    else
    {
        if (x == a->next->x && y == b->next->y)
            return false;
        bool b_a = inside_edge(a->x, a->next->x, x);
        bool b_d = inside_edge(b->y, b->next->y, y);
        return (b_a && b_d);
    }
}

// two the same coordinate intersect point connect other
void operation::two_way_intersect(intersect_point *a, intersect_point *b)
{
    a->cross_point = b;
    a->tran = true;
    b->cross_point = a;
    b->tran = false;
}
// the new intersect (x,y) is right after point a and point b
void operation::new_intersect(point *a, point *b, long long x, long long y)
{
    intersect_point *insert_a = new intersect_point(x, y, glob_color);
    intersect_point *insert_b = new intersect_point(x, y, glob_color);
    add_intersect(a, insert_a);
    add_intersect(b, insert_b);
    bool order = true;
    point *temp_a = a;
    point *temp_b = b;
    if(b->verti){
        temp_a = b;
        temp_b = a;
        swap(insert_a, insert_b);
    }
    if (temp_a->dir){
        if (!temp_b->dir){
            order = false;
        }
        else if(temp_b->dir)
            order = false;
    }
    if(order)
        two_way_intersect(insert_a, insert_b);
    else
        two_way_intersect(insert_b, insert_a);
}