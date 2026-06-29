#pragma once
#include "../Graph/Graph.h"
#include <vector>
#include <string>
#include <climits>

// ============================================================
// Dijkstra's Algorithm — Single-Source Shortest Path
// Uses custom MinHeap
// Complexity : O((V + E) log V)
// ============================================================

enum class CostMode { DISTANCE, TIME, TRAFFIC };   // what to optimise

struct DijkstraResult {
    std::vector<double>      dist;        // dist[v] = shortest cost from src
    std::vector<int>         parent;
    std::vector<int>         path;        // src → dst path (node ids)
    std::vector<std::string> pathNames;
    double                   totalDist;
    double                   totalTime;
    double                   totalCost;
    int                      nodesExplored;
    bool                     reachable;
    CostMode                 mode;
};

class Dijkstra {
public:
    static DijkstraResult run(const Graph& g, int src, int dst,
                              CostMode mode = CostMode::DISTANCE);

    static void printResult(const Graph& g, const DijkstraResult& res,
                            int src, int dst);
private:
    static double edgeCost(const Road& r, CostMode mode);
};
