#pragma once
#include "../Graph/Graph.h"
#include <vector>
#include <string>

// ============================================================
// A* Search Algorithm
// f(n) = g(n) + h(n)
// g(n) = cost from source
// h(n) = haversine heuristic to destination
// Complexity : O((V + E) log V)  — typically far fewer nodes explored
// ============================================================

struct AStarResult {
    std::vector<int>         path;
    std::vector<std::string> pathNames;
    double                   totalDist;
    double                   totalTime;
    double                   totalCost;
    int                      nodesExplored;
    bool                     reachable;
};

class AStar {
public:
    static AStarResult run(const Graph& g, int src, int dst);
    static void printResult(const Graph& g, const AStarResult& res,
                            int src, int dst);
};
