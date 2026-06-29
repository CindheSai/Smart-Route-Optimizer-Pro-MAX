#pragma once
#include "../Graph/Graph.h"
#include <vector>
#include <string>

// ============================================================
// BFS — Breadth-First Search
// Purpose : graph traversal, minimum-hop path
// Complexity : O(V + E)
// ============================================================

struct BFSResult {
    std::vector<int>    visitOrder;    // nodes in BFS visit order
    std::vector<int>    path;          // shortest-hop path src->dst
    std::vector<std::string> pathNames;
    int                 hops;
    bool                reachable;
    int                 nodesExplored;
};

class BFS {
public:
    // Full graph traversal from source
    static BFSResult traverse(const Graph& g, int src);

    // Minimum hop path from src to dst
    static BFSResult shortestHop(const Graph& g, int src, int dst);

    static void printResult(const Graph& g, const BFSResult& res, int src, int dst = -1);
};
