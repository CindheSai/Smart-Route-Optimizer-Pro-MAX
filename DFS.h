#pragma once
#include "../Graph/Graph.h"
#include <vector>
#include <string>

// ============================================================
// DFS — Depth-First Search
// Purpose : traversal, connectivity, cycle detection
// Complexity : O(V + E)
// ============================================================

struct DFSResult {
    std::vector<int>         visitOrder;
    std::vector<std::string> visitNames;
    bool                     hasCycle;
    int                      components;    // connected components
    int                      nodesExplored;
};

class DFS {
private:
    static void dfsVisit(const Graph& g, int cur,
                         std::vector<bool>& visited,
                         std::vector<int>& color,    // 0=white,1=gray,2=black
                         DFSResult& res,
                         int parent);
public:
    static DFSResult traverse(const Graph& g, int src);
    static DFSResult fullTraversal(const Graph& g);   // visits all components
    static void printResult(const Graph& g, const DFSResult& res);
};
