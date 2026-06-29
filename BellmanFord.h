#pragma once
#include "../Graph/Graph.h"
#include <vector>
#include <string>

// ============================================================
// Bellman-Ford Algorithm
// Handles negative edge weights, detects negative cycles
// Complexity : O(V * E)
// ============================================================

struct BFordResult {
    std::vector<double>      dist;
    std::vector<int>         parent;
    std::vector<int>         path;
    std::vector<std::string> pathNames;
    double                   totalDist;
    double                   totalTime;
    bool                     reachable;
    bool                     negativeCycle;
    int                      iterations;
};

class BellmanFord {
public:
    static BFordResult run(const Graph& g, int src, int dst);
    static void printResult(const Graph& g, const BFordResult& res, int src, int dst);
};
