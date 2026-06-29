#include "AStar.h"
#include "../DataStructures/MinHeap.h"
#include <cstdio>
#include <cmath>

// ============================================================
// A* Search
// ============================================================

AStarResult AStar::run(const Graph& g, int src, int dst) {
    int N = g.getNextId();
    AStarResult res;
    res.totalDist     = 0;
    res.totalTime     = 0;
    res.totalCost     = 0;
    res.nodesExplored = 0;
    res.reachable     = false;

    std::vector<double> gCost(N, 1e18);   // g(n) — cost from source
    std::vector<double> fCost(N, 1e18);   // f(n) = g(n) + h(n)
    std::vector<int>    parent(N, -1);
    std::vector<bool>   closed(N, false);

    MinHeap openSet;
    openSet.build(N);

    gCost[src] = 0.0;
    fCost[src] = g.heuristic(src, dst);
    openSet.insert(fCost[src], src);

    while (!openSet.isEmpty()) {
        HeapNode hn = openSet.extractMin();
        int cur = hn.nodeId;
        res.nodesExplored++;

        if (cur == dst) { res.reachable = true; break; }
        closed[cur] = true;

        for (const auto& e : g.getAdj(cur)) {
            int nb = e.dst;
            if (!g.isValid(nb) || closed[nb]) continue;
            if (e.road.status != RoadStatus::OPEN)  continue;

            double tentG = gCost[cur] + e.road.distance;
            if (tentG < gCost[nb]) {
                parent[nb] = cur;
                gCost[nb]  = tentG;
                fCost[nb]  = tentG + g.heuristic(nb, dst);
                if (openSet.contains(nb))
                    openSet.decreaseKey(nb, fCost[nb]);
                else
                    openSet.insert(fCost[nb], nb);
            }
        }
    }

    if (!res.reachable) return res;

    // Reconstruct
    std::vector<int> rev;
    for (int v = dst; v != -1; v = parent[v]) rev.push_back(v);
    for (int i = (int)rev.size() - 1; i >= 0; i--) {
        res.path.push_back(rev[i]);
        res.pathNames.push_back(g.getLocation(rev[i]).name);
    }

    for (int i = 0; i + 1 < (int)res.path.size(); i++) {
        int u = res.path[i], v2 = res.path[i+1];
        for (const auto& e : g.getAdj(u)) {
            if (e.dst == v2) {
                res.totalDist += e.road.distance;
                res.totalTime += e.road.travelTime;
                res.totalCost += e.road.effectiveCost();
                break;
            }
        }
    }
    return res;
}

void AStar::printResult(const Graph& g, const AStarResult& res, int src, int dst) {
    printf("\n  ╔══════════════════════════════════════════════════════╗\n");
    printf("  ║               A* SMART ROUTE RESULT                 ║\n");
    printf("  ╠══════════════════════════════════════════════════════╣\n");
    printf("  ║  Source         : %-34s║\n", g.getLocation(src).name.c_str());
    printf("  ║  Destination    : %-34s║\n", g.getLocation(dst).name.c_str());
    printf("  ║  Nodes Explored : %-34d║\n", res.nodesExplored);
    printf("  ╠══════════════════════════════════════════════════════╣\n");

    if (!res.reachable) {
        printf("  ║  ✗  No route found.                                 ║\n");
        printf("  ╚══════════════════════════════════════════════════════╝\n");
        return;
    }

    printf("  ║  Total Distance : %-6.0f km                           ║\n", res.totalDist);
    printf("  ║  Total Time     : %-4.1f hrs                           ║\n", res.totalTime);
    printf("  ║  Heuristic      : Haversine (straight-line)           ║\n");
    printf("  ╚══════════════════════════════════════════════════════╝\n");

    printf("\n  A* Route:\n");
    for (int i = 0; i < (int)res.pathNames.size(); i++) {
        if (i == 0)
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        else {
            int u = res.path[i-1], v = res.path[i];
            for (const auto& e : g.getAdj(u)) {
                if (e.dst == v) {
                    printf("    │  %.0f km, %.1f hr\n", e.road.distance, e.road.travelTime);
                    break;
                }
            }
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        }
    }
    printf("\n");
}
