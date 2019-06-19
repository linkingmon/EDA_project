#ifndef SPLITMGR_H
#define SPLITMGR_H
#include "interval.h"
#include "HopcroftKarft.h"
#include "glob_func.h"

class SplitMgr
{
public:
    ~SplitMgr() { clear(); }
    void splitV(vector<point *> &total);
    void splitH(vector<point *> &total);
    void splitO(vector<point *> &total);
    void output_rect(string outPath);
    bool outside_poly(point *root, point *cross);
    void build_polygon_table(vector<vector<point *> > &, vector<point *> &);
    void find_concave(vector<point *>& concaveList, vector<point *>& polygon);
    void find_diagonal(vector<vector<Diagonal> >& diagonalList, vector<point *>& polygon);
    void find_cross(vector<vector<Diagonal> >&diagonalList, vector<vector<int> >& adj_matrix);
    void find_max_matching(vector<vector<int> >& adj_matrix);
    void find_min_cover(vector<vector<Diagonal> >&diagonalList, vector<vector<int> >& adj_matrix);
    void split_poly(vector<point *>& polygon, vector<vector<Diagonal> >&diagonalList);
    void clear();

private:
    Interval_Mgr _IM;
    vector<Rectangle *> _rectList;
};

void SplitMgr::splitV(vector<point *> &total)
{
    vector<vector<point *> > polygon_table;
    build_polygon_table(polygon_table, total);
    for (size_t i = 0; i < polygon_table.size(); i++)
    {
        _IM.build_interval(polygon_table[i], 1);
        _IM.split_polygon();
        _IM.move_rectangle(_rectList);
        _IM.clear();
    }
}

void SplitMgr::splitH(vector<point *> &total)
{
    vector<vector<point *> > polygon_table;
    build_polygon_table(polygon_table, total);
    for (size_t i = 0; i < polygon_table.size(); i++)
    {
        _IM.build_interval(polygon_table[i], 0);
        _IM.split_polygon();
        _IM.move_rectangle(_rectList);
        _IM.clear();
    }
}

void SplitMgr::splitO(vector<point *> &total)
{
    vector<vector<point *> > polygon_table;
    build_polygon_table(polygon_table, total);
    for (size_t i = 0; i < polygon_table.size(); i++)
    {
        vector<vector<Diagonal > > diagonalList;
        find_diagonal(diagonalList, polygon_table[i]);
        vector<vector<int> > adj_matrix;
        find_cross(diagonalList, adj_matrix);
        find_max_matching(adj_matrix);
        find_min_cover(diagonalList, adj_matrix);
        split_poly(polygon_table[i], diagonalList);

    }
}

void SplitMgr::output_rect(string outPath)
{
    ofstream outfile(outPath.c_str());
    for (size_t i = 0; i < _rectList.size(); i++)
    {
        outfile << *_rectList[i] << endl;
    }
}

void SplitMgr::clear()
{
    for (size_t i = 0; i < _rectList.size(); i++)
    {
        delete _rectList[i];
        _rectList[i] = 0;
    }
}

bool SplitMgr::outside_poly(point *root, point *cross)
{
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
                    return true;
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
                    return true;
        }
        p = p->next;
    }
    if (cross_cnt % 2 == 1)
        return false;
    else
        return true;
}

void SplitMgr::build_polygon_table(vector<vector<point *> > &polygon_table, vector<point *> &total)
{
    for (size_t i = 0; i < total.size(); i++)
    {
        bool new_poly = 1;
        point *p = total[i];
        for (size_t j = 0; j < polygon_table.size(); j++)
        {
            if (!outside_poly(p, polygon_table[j][0]))
            {
                polygon_table[j].push_back(p);
                new_poly = 0;
                break;
            }
        }

        if (new_poly)
        {
            vector<point *> poly;
            poly.push_back(p);
            for (size_t j = 0; j < polygon_table.size(); j++)
            {
                if (!outside_poly(polygon_table[j][0], p))
                {
                    poly.insert(poly.end(), polygon_table[j].begin(), polygon_table[j].end());
                    polygon_table.erase(polygon_table.begin() + j);
                    j--;
                }
            }
            polygon_table.push_back(poly);
        }
    }
}

void SplitMgr::find_concave(vector<point *>& concaveList, vector<point *>& polygon)
{
    glob_color = -1;
    concaveList.clear();
    for (size_t i=0; i<polygon.size(); i++)
    {
        ++glob_color;
        point* p = polygon[i];
        size_t n_vertex = p->len;
        for (size_t j=0; j<n_vertex; j++)
        {
            p->pcolor = glob_color;
            if (p->verti && (p->dir!=p->prev->dir)) concaveList.push_back(p);
            if (!p->verti && (p->dir==p->prev->dir)) concaveList.push_back(p);
            p = p->next;
        }
    }
    // cout << "Concave vertices: ";
    // for (size_t i=0; i<concaveList.size(); i++){
    //     cout << *concaveList[i] << endl;
    // }
}

void SplitMgr::find_diagonal(vector<vector<Diagonal> >& diagonalList, vector<point *>& polygon)
{
    vector<point *> concaveList;
    find_concave(concaveList, polygon);
    _IM.find_diagonal(diagonalList, concaveList, polygon);
}

void SplitMgr::find_cross(vector<vector<Diagonal> >&diagonalList, vector<vector<int> >& adj_matrix)
{
    vector<int> neighbor;
    for (size_t i=0; i<diagonalList[0].size(); i++)
    {
        Diagonal d1 = diagonalList[0][i];
        for (size_t j=0; j<diagonalList[1].size(); j++)
        {
            Diagonal d2 = diagonalList[1][j];
            
            if (d1._p1->x >= d2._p1->x && d2._p2->x >= d1._p1->x  && d2._p1->y >= d1._p1->y && d1._p2->y >= d2._p1->y)
            neighbor.push_back(1);
            else neighbor.push_back(0);
        }
        adj_matrix.push_back(neighbor);
        neighbor.clear();
    }
    // for (size_t i=0; i<diagonalList[0].size(); i++)
    // {
    //     for (size_t j=0; j<diagonalList[1].size(); j++)
    //     {
    //         if (adj_matrix[i][j]){
    //             cout << "Cross " << diagonalList[0][i] << diagonalList[1][j] << endl;
    //         }
    //     }
    // }
}

void SplitMgr::find_max_matching(vector<vector<int> >& adj_matrix)
{
    size_t m=adj_matrix.size(), n=adj_matrix[0].size();
    BipGraph g(m,n);
    for (size_t i=0; i<m; i++)
    {
        for (size_t j=0; j<n; j++)
        {
            if (adj_matrix[i][j]!=0)
                g.addEdge(i+1, j+1);
        }
    }
    g.hopcroftKarp();
    g.get_matching(adj_matrix);
    // for (size_t i=0; i<m; i++)
    // {
    //     for (size_t j=0; j<n; j++)
    //     {
    //         if (adj_matrix[i][j]==1)
    //             cout << "Unmatched edge " << i+1 << ", " << j+1 << endl;
    //         if(adj_matrix[i][j]==2)
    //             cout << "Matched edge " << i+1 << ", " << j+1 << endl;                
    //     }
    // }

}

void SplitMgr::find_min_cover(vector<vector<Diagonal> >&diagonalList, vector<vector<int> >& adj_matrix)
{
    int m = diagonalList[0].size();
    int n = diagonalList[1].size();
    int num_vertex = m+n;

    // building adjacency list
    vector< list<int> > AdjList;
    AdjList.resize(num_vertex);
    for (int i=0; i<m; i++)
    {
        for (int j=0; j<n; j++)
        {
            if (adj_matrix[i][j]==0) 
                continue;
            else if(adj_matrix[i][j]==1)
                AdjList[i].push_back(m+j);
            else 
            {
                AdjList[m+j].push_back(i);
                diagonalList[0][i]._is_matched = true;
                diagonalList[1][j]._is_matched = true;
            }               
        }
    }

    // BFS
    // Mark all the vertices as not visited
    bool *visited = new bool[num_vertex];
    for (int i=0; i<num_vertex; i++)
        visited[i] = false;

    list<int> queue;
    for (int i=0; i<m; i++){
        if (!diagonalList[0][i]._is_matched){
            visited[i] = true;
            queue.push_back(i);
        }
    }
    list<int>::iterator itr;

    while (!queue.empty())
    {
        int s = queue.front();
        queue.pop_front();

        for (itr = AdjList[s].begin(); itr != AdjList[s].end(); ++itr) 
        { 
            if (!visited[*itr]) 
            { 
                visited[*itr] = true; 
                queue.push_back(*itr); 
            } 
        } 
    }  

    // Max matching to minimun vertex cover
    for (int i=0; i<m; i++){
        if (!visited[i])
            diagonalList[0][i]._in_min_cover = true;
    }
    for (int i=m; i<num_vertex; i++){
        if (visited[i])
            diagonalList[1][i-m]._in_min_cover = true;
    }


    // cout << "Visited";
    // for (int i=0; i<num_vertex; i++){
    //     if (visited[i]) 
    //         cout << " " << i+1;
    // }
    // cout << endl;

    // cout << "Minimun vertex cover";
    // for (int i=0; i<m; i++){
    //     if (diagonalList[0][i]._in_min_cover)
    //         cout << " " << i+1;
    // }
    // for (int i=0; i<n; i++){
    //     if (diagonalList[1][i]._in_min_cover)
    //         cout << " " << m+i+1;
    // }
    // cout << endl;
    
    // for (size_t i=0; i<AdjList.size(); i++)
    // {
    //     cout << i+1 << "'s neighbors: ";
    //     for (itr=AdjList[i].begin(); itr!=AdjList[i].end(); itr++)
    //     cout << *itr+1 << " ";
    //     cout << endl;
    // }
    delete[] visited;
}

void SplitMgr::split_poly(vector<point *>& polygon, vector<vector<Diagonal> >&diagonalList)
{
    for (int i=0; i<diagonalList[0].size(); i++)
    {
        Diagonal & d=diagonalList[0][i];
        if(!d._in_min_cover)
        {
            cout << d << endl;
        }
    }
    for (int i=0; i<diagonalList[1].size(); i++)
    {
        Diagonal & d=diagonalList[1][i];
        if(!d._in_min_cover)
        {
            cout << d << endl;
        }
    }
}

#endif
