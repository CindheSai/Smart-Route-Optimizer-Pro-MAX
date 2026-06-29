#pragma once
#include "../Graph/Graph.h"
#include <vector>

// ============================================================
// Kruskal's Minimum Spanning Tree
// Uses custom DisjointSet
// Complexity : O(E log E)
// ============================================================

struct MSTEdge {
    int    src, dst;
    double weight;
    std::string srcName, dstName;
};

struct KruskalResult {
    std::vector<MSTEdge> mstEdges;
    double               totalCost;
    int                  edgesUsed;
    bool                 connected;
};

class Kruskal {
public:
    static KruskalResult run(const Graph& g);
    static void printResult(const KruskalResult& res);
};
