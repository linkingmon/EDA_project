#ifndef HopcroftKarp_H
#define HopcroftKarp_H
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;
// Constants used by our algorithm
namespace constants
{
const int NIL = 0;
const int MAX_SIZE = 100000;
const int INF = 1000000;
} // namespace constants

class HopcroftKarp
{
private:
    // Nodes for both sides in the bipartite graph stored in arrays of vectors
    // We don't care about the values/weights of the nodes, just that they exist
    // Thus, the data structure is as follows:
    //      Array[] Index -> Vector of connecting edge indices { a, b, c }
    //      Example: graph[1] = { 5, 6, 7 } <-- with num_nodes_left = 4
    //      means that node 1 on the left has edges to nodes (5-4), (6-4), (7-4)
    //      on the right, or node 1 on the left has edges to nodes 1, 2, 3
    //      on the right side of the graph.
    // This logic with index node numbering also extends to matching and distance
    std::vector<int> graph[constants::MAX_SIZE];

    // Cached number of nodes on each side
    int num_nodes_left, num_nodes_right;

    // A mapping of matched nodes used during the algorithm
    int matching[constants::MAX_SIZE];

    // Distance stored during search
    int distance[constants::MAX_SIZE];

    // Private functions
    bool bfs();
    bool dfs(int);
    void output_matching(int);

public:
    // The only public interface for running the algorithm
    void run(const vector<vector<bool> > &adj_matrix);
};

bool HopcroftKarp::bfs()
{
    // Standard use of queues to perform BFS
    std::queue<int> queue;
    // Nodes are numbered 1 through n, and don't start from 0
    for (int i = 1; i <= num_nodes_left; i++)
    {
        // Check to see if there is a matching for this node already
        if (matching[i] == constants::NIL)
        {
            // Not found, set distance to 0 and add to queue
            distance[i] = 0;
            queue.push(i);
        }
        else
        {
            distance[i] = constants::INF;
        }
    }

    distance[constants::NIL] = constants::INF;

    // As long as the queue isn't empty...
    while (!queue.empty())
    {
        int left = queue.front();
        queue.pop();
        // Sanity check
        if (left != constants::NIL)
        {
            // For each node on the right connected to left by an edge...
            for(unsigned int i = 0 ; i < graph[left].size() ; ++i){
                int right = graph[left][i];
                // If the distance is set to INF/-1, set distance to be
                // that of left node + 1 (for this edge)
                if (distance[matching[right]] == constants::INF)
                {
                    distance[matching[right]] = distance[left] + 1;
                    queue.push(matching[right]);
                }
            }
        }
    }
    return (distance[constants::NIL] != constants::INF);
}

bool HopcroftKarp::dfs(int left)
{
    // Only proceed with DFS if the node isn't NIL
    if (left != constants::NIL)
    {
        // For each node on the right connected to left by an edge...
        for(unsigned int i = 0 ; i < graph[left].size() ; ++i){
            int right = graph[left][i];
            if (distance[matching[right]] == distance[left] + 1)
            {
                // Standard DFS recursion
                if (dfs(matching[right]))
                {
                    matching[left] = right;
                    matching[right] = left;
                    std::cout << "\n\tFound Matching; left: " << left << ", right: " << right;
                    return true;
                }
            }
        }
        distance[left] = constants::INF;
        return false;
    }
    // Node was nil, no further depth search possible; return true
    return true;
}

void HopcroftKarp::output_matching(int num_matching)
{
    std::cout << "\n\tMaximum Cardinality Matching: " << num_matching << "\n";
}

void HopcroftKarp::run(const vector<vector<bool> > &adj_matrix)
{
    // Input data to form bipartite graph
    // left * right
    num_nodes_left = adj_matrix.size();
    num_nodes_right = adj_matrix[0].size();

    for (unsigned int i = 0; i < adj_matrix.size(); ++i)
    {
        for (unsigned int j = 0; j < adj_matrix[0].size(); ++j)
        {
            if (adj_matrix[i][j])
            {
                graph[i + 1].push_back(j + 1 + num_nodes_left);
                graph[j + 1 + num_nodes_left].push_back(i + 1);
            }
        }
    }

    // Run the algorithm to count max matching
    int num_matching;
    while (bfs())
    {
        for (int i = 0; i <= num_nodes_left; i++)
        {
            if (matching[i] == constants::NIL && dfs(i))
            {
                num_matching++;
            }
        }
    }

    // Output function
    output_matching(num_matching);
}
#endif