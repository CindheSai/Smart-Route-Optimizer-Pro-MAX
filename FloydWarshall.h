#pragma once
#include "../Graph/Graph.h"
#include <vector>

// ============================================================
// Floyd-Warshall — All-Pairs Shortest Path
// Complexity : O(V³)
// ============================================================

struct FWResult {
    std::vector<std::vector<double>> dist;  // dist[i][j]
    std::vector<std::vector<int>>    next;  // for path reconstruction
    int N;
};

class FloydWarshall {
public:
    static FWResult run(const Graph& g);
    static void printDistanceTable(const Graph& g, const FWResult& res);
    static void printPath(const Graph& g, const FWResult& res, int src, int dst);
};
