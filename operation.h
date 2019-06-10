// #include "point.h"
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
    void find_intersect();
    void find_intersect(operation &);
    void insert_intersect();
    void check_list(vector<point *> &);
    int find_intersect(point *a, point *b, operation &);
    bool find_cross(point *, point *);
    void add_intersect(point *a, point *b);
    bool inside_edge(long long x1, long long x2, long long y);
    void two_way_intersect(intersect_point *, intersect_point *);
    void new_intersect(point *a, point *b, long long x, long long y);
    bool inside_region(point *a, point *b, long long x, long long y);
    bool in_out_cross(point *, point *);
    bool outside_poly(point *, point *);
    operation &operator+=(operation &); // Merge
    operation &operator-=(operation &); // Clip
    string name;
    vector<point *> root_list; // 存該 operation 下的所有多邊形
    set<point *> out_list;
    vector<point *> out_list_buf; //先存好out焦點 如果沒有要被刪掉才推到out list之中
};

void operation::find_intersect()
{
#ifdef DEBUG
    cout << "----------intersect----------" << endl;
#endif

    for (int i = 0; i < root_list.size(); ++i)
        for (int j = i + 1; j < root_list.size(); ++j)
        {
#ifdef DEBUG
            // 0是有焦點 1是要刪掉前面的 2是要刪掉後面的
            cout << "Find intersect: " << *root_list[i] << " " << *root_list[j] << endl;
#endif
            int state = find_intersect(root_list[i], root_list[j], *this);
            switch (state)
            {
            case 1:
                root_list[i] = root_list[root_list.size() - 1];
                root_list.pop_back();
                // root_list.erase(root_list.begin() + i);
                --i;
                continue;
                break;
            case 2:
                root_list[j] = root_list[root_list.size() - 1];
                root_list.pop_back();
                // root_list.erase(root_list.begin() + j);
                --j;
                continue;
                break;
            default:
                break;
            }
        }
}

void operation::find_intersect(operation &total)
{
#ifdef DEBUG
    // cout << "----------intersect total root list----------" << endl;
#endif
    // cout << "CUR size" << root_list.size() << endl;
    // cout << "TOTAL size" << total.root_list.size() << endl;
    // cout << "CUR size" << out_list_buf.size() << endl;
    // cout << "TOTAL size" << total.out_list_buf.size() << endl;
    for (int i = 0; i < root_list.size(); ++i)
    {
        for (int j = 0; j < total.root_list.size(); ++j)
        {
#ifdef DEBUG
            // 0是有焦點 1是要刪掉前面的 2是要刪掉後面的
            // cout << "Find intersect: " << *root_list[i] << " " << *(total.root_list[j]) << endl;
#endif
            int state = find_intersect(root_list[i], total.root_list[j], total);
            // cout << "CUR size" << out_list_buf.size() << endl;
            // cout << "TOTAL size" << total.out_list_buf.size() << endl;
            // cout << "X" << state << endl;
            switch (state)
            {
            case 1:
                root_list.erase(root_list.begin() + i);
                --i;
                continue;
                break;
            case 2:
                total.root_list.erase(total.root_list.begin() + j);
                --j;
                continue;
                break;
            default:
                break;
            }
        }
    }
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
            a->intersection.clear();
            a = temp;
        }
        root_list[i]->len += len;
    }
}

// find cross point in two POLYGON
// 找兩個 多邊形 的交點
// o_b point*b 屬於的operation
int operation::find_intersect(point *a, point *b, operation &o_b)
{
    if (b->minx > a->maxx || b->miny > a->maxy || b->maxx < a->minx || b->maxy < a->miny)
    {
        // cout << a->minx << " " << a->miny << " " << a->maxx << " " << a->maxy << endl;
        // cout << b->minx << " " << b->miny << " " << b->maxx << " " << b->maxy << endl;
        // cout << "No Intersect" << endl;
        return 0;
    }
    out_list_buf.clear();
    o_b.out_list_buf.clear();
    point *p_k = a;
    point *p_l = b;
    bool cross = false; // 紀錄有沒有交點，如果有交點才做 in out cross
    // 每兩個頂點都做 找交點
    ++glob_color;
    for (int k = 0; k < a->len; k++)
    {
        for (int l = 0; l < b->len; l++)
        {
            // find_cross(p_k, p_l);
            // cout << "FIND CROSS: " << *p_k << " " << *p_l << endl;
            cross = (cross | find_cross(p_k, p_l));
            p_l = p_l->next;
        }
        p_k = p_k->next;
    }
    int return_num = 0;
    // if (cross) // 有交點
    // {
    bool has_out;
    has_out = in_out_cross(a, b);
    if (!has_out)
    {
        return_num += 1;
        out_list_buf.clear();
    }
    // cout << "HAS OUT" << has_out << endl;
    has_out = o_b.in_out_cross(b, a);
    if (!has_out)
    {
        return_num += 2;
        o_b.out_list_buf.clear();
    }
    // cout << "HAS OUT" << has_out << endl;
    // }
    // else
    // {
    //     if (!outside_poly(a, b)) // a再b裡面，a要刪掉
    //         return_num = 1;
    //     else
    //     {
    //         if (!outside_poly(b, a)) // b再a裡面，b要刪掉
    //             return_num = 2;
    //         else // 兩個是分開的 都不用刪掉
    //             return_num = 0;
    //     }
    // }
    // cout << "CROSS" << cross << endl;
    // cout << "RET: " << return_num << endl;
    point *p;
    intersect_point *p_t;
    switch (return_num)
    {
    case 1:
#ifdef DEBUG
        cout << "Delete1 Polygon: " << *a << endl;
#endif
        if (this != &o_b)
            a->delete_poly_tranf(out_list, o_b.out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        else
            a->delete_poly_tranf(out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        delete a;
        p = b;
        for (unsigned int i = 0; i < b->len; ++i)
        {
            for (unsigned int j = 0; j < p->intersection.size(); ++j)
            {
                p_t = static_cast<intersect_point *>(p->intersection[j]);
                if (p_t->color == glob_color)
                {
                    p->intersection.erase(p->intersection.begin() + j);
                    delete p_t;
                    --j;
                }
            }
            p = p->next;
        }
        return 1;
        break;
    case 2:
    case 3:
#ifdef DEBUG
        cout << "Delete2 Polygon: " << *b << endl;
#endif
        if (this != &o_b)
            b->delete_poly_tranf(out_list, o_b.out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        else
            b->delete_poly_tranf(out_list); // 要把對面的tran清掉 // 還有要把out的點從out_list移除
        delete b;
        p = a;
        for (unsigned int i = 0; i < a->len; ++i)
        {
            for (unsigned int j = 0; j < p->intersection.size(); ++j)
            {
                p_t = static_cast<intersect_point *>(p->intersection[j]);
                if (p_t->color == glob_color)
                {
                    p->intersection.erase(p->intersection.begin() + j);
                    delete p_t;
                    --j;
                }
            }
            p = p->next;
        }
        return 2;
        break;
    default:

        // cout << "CUR OUT LIST " << out_list.size() << endl;
        // cout << "CURR OUT LIST BUF" << out_list_buf.size() << endl;
        for (unsigned int kk = 0; kk < out_list_buf.size(); ++kk)
        {
            // cout << "KKK " << *(out_list_buf[kk]) << endl;
            out_list.insert(out_list_buf[kk]);
        }
        if (this != &o_b)
        {
            // cout << "NOT SAME" << endl;
            // cout << "OB OUT LIST " << o_b.out_list.size() << endl;
            // cout << "OB OUT LIST BUF" << o_b.out_list_buf.size() << endl;
            for (unsigned int kk = 0; kk < o_b.out_list_buf.size(); ++kk)
            {
                // cout << "KKK " << *(o_b.out_list_buf[kk]) << endl;
                o_b.out_list.insert(o_b.out_list_buf[kk]);
            }
        }
        return 0;
        break;
    }
    // return 0;
}

// cross 來 merge root
// 先判斷 root 是不是在 cross 外面
// root 如果在 cross 的邊上就算是在裡面
// 2019/6/6
// root 如果在外面才開始"順時針"遶(by 宥璁)
bool operation::in_out_cross(point *root, point *cross)
{
    // 要一直判斷點是不是在外面，有點多餘
    point *p = root;
    bool has_out = false;
    for (int i = 0; i < root->len; i++)
    {
        bool outside = outside_poly(p, cross);
        // cout << "POINT" << *p << " " << outside << endl;
        // 如果是 outside 順著走的交點是 outward
        if (outside)
        {
            has_out = true;
            // cout << "SIZE" << p->prev->intersection.size() << endl;
            if (p->prev->intersection.size() > 0)
            {
                p->prev->sort_intersection();
                // cout << "SIZE after sort" << p->prev->intersection.size() << endl;
                // cout << "SORT Done" << endl;
                for (int j = p->prev->intersection.size() - 1; j >= 0; --j)
                {
                    // cout << "J: " << j << endl;
                    intersect_point *p_t = static_cast<intersect_point *>(p->prev->intersection[j]);
                    if (p_t->color == glob_color)
                    {
                        p_t->in = false;
                        // cout << "PUSH " << *p_t << endl;
                        out_list_buf.push_back(p_t);
                        break;
                    }
                }
            }
        }
        p = p->next;
    }
    return has_out;
    // cout << "Should delete Poly: " << *p << endl;
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
    if (cross_cnt % 2 == 1)
        return false;
    else
        return true;
}

// given two point a, b ; find cross point between a, a->next ,b ,b->bext
bool operation::find_cross(point *a, point *b)
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
            return true;
        }
    }
    return false;
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
    if (b->verti)
    {
        temp_a = b;
        temp_b = a;
        swap(insert_a, insert_b);
    }
    if (temp_a->dir)
    {
        if (!temp_b->dir)
        {
            order = false;
        }
    }
    else if (temp_b->dir)
        order = false;

    if (order)
        two_way_intersect(insert_a, insert_b);
    else
        two_way_intersect(insert_b, insert_a);
}

void operation::check_list(vector<point *> &new_list)
{ // 判斷有些多邊形跟其他是分開的，根本不用何在一起，要直接加進去；感覺有點慢?
    // cout << "YYY" << new_list.size() << " " << root_list.size() << endl;
    for (unsigned int k = 0; k < root_list.size(); ++k)
    {
        point *p = root_list[k];
        bool isout = true;
        // cout << p->len << " " << *p << endl;
        for (unsigned int z = 0; z < root_list[k]->len; ++z)
        {
            if (p->pcolor == glob_color)
            {
                isout = false;
                break;
            }
            // cout << p->pcolor << *p << endl;
            p = p->next;
        }
        // cout << "ISOUT" << isout << endl;
        if (isout)
        {
            new_list.push_back(root_list[k]);
        }
        else
        {
            root_list[k]->delete_poly();
            delete root_list[k];
        }
    }
    // root_list = new_list;
    // return new_list;
}

operation &operation::operator+=(operation &oper)
{
#ifdef DEBUG
    cout << "Merging " << oper.name << endl;
#endif

    // cout << "size is " << root_list.size() << endl;
    // cout << "size is " << oper.root_list.size() << endl;
    // cout << "size is " << out_list.size() << endl;
    // cout << "size is " << oper.out_list.size() << endl;
    if ((root_list.size()) != 0)
    {
        oper.find_intersect(*this);
        // cout << "sssize is " << root_list.size() << endl;
        // cout << "sssize is " << oper.root_list.size() << endl;
        // cout << root_list[0]->len << "LL" << endl;
        // cout << oper.root_list[0]->len << "LL" << endl;/
        insert_intersect();
        oper.insert_intersect();
        // cout << root_list[0]->len << "LL" << endl;
        // cout << oper.root_list[0]->len << "LL" << endl;
#ifdef DEBUG
        // cout << "POLY 1: " << endl;
        // root_list[0]->print_poly();
        // cout << "POLY 2: " << endl;
        // oper.root_list[0]->print_poly();
        set<point *>::iterator iter;
        // cout << "OUT LIST size " << out_list.size() << endl;
        for (iter = out_list.begin(); iter != out_list.end(); ++iter)
        {
            cout << **iter;
        }
        // cout << "OPPER OUT LIST size " << oper.out_list.size() << endl;
        for (iter = oper.out_list.begin(); iter != oper.out_list.end(); ++iter)
        {
            cout << **iter;
        }
#endif
        oper.out_list.insert(out_list.begin(), out_list.end()); // 兩個的out道在一起
        // cout << "OPER LIST SIZE " << oper.out_list.size() << endl;
        // cout << "Total pointsdd: " << point_cnt << " & intersects: " << intersect_cnt << endl;
        vector<point *> new_list = ::little_merge(oper.out_list);
        // cout << "Total pointscc: " << point_cnt << " & intersects: " << intersect_cnt << endl;
        out_list.clear();
        out_list_buf.clear();
        oper.out_list.clear();
        oper.out_list_buf.clear();
        oper.check_list(new_list);
        // cout << root_list[0]->len << "LL" << endl;
        check_list(new_list);
        root_list = new_list;
    }
    else
        root_list = oper.root_list;
    // cout << "size is " << total.root_list.size() << endl;

    // vector<point *> new_list;
    // int cnt = 0;
    // ++glob_color;
}

operation &operation::operator-=(operation &clip)
{
    operation &root = *this;
    cout << "Clippingg" << endl;
}
