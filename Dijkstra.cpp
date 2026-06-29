#include "Dijkstra.h"
#include "../DataStructures/MinHeap.h"
#include <cstdio>
#include <cmath>

// ============================================================
// Edge cost selector
// ============================================================

double Dijkstra::edgeCost(const Road& r, CostMode mode) {
    if (r.status != RoadStatus::OPEN) return 1e18; // treat closed roads as impassable
    switch (mode) {
        case CostMode::DISTANCE: return r.distance;
        case CostMode::TIME:     return r.travelTime;
        case CostMode::TRAFFIC:  return r.effectiveCost();
        default:                 return r.distance;
    }
}

// ============================================================
// Dijkstra::run
// ============================================================

DijkstraResult Dijkstra::run(const Graph& g, int src, int dst, CostMode mode) {
    int N = g.getNextId();
    DijkstraResult res;
    res.dist.assign(N, 1e18);
    res.parent.assign(N, -1);
    res.totalDist     = 0;
    res.totalTime     = 0;
    res.totalCost     = 0;
    res.nodesExplored = 0;
    res.reachable     = false;
    res.mode          = mode;

    MinHeap heap;
    heap.build(N);

    res.dist[src] = 0.0;
    heap.insert(0.0, src);

    while (!heap.isEmpty()) {
        HeapNode hn = heap.extractMin();
        int u = hn.nodeId;
        res.nodesExplored++;

        if (u == dst) { res.reachable = true; break; }

        for (const auto& e : g.getAdj(u)) {
            int v = e.dst;
            if (!g.isValid(v)) continue;
            double w = edgeCost(e.road, mode);
            if (res.dist[u] + w < res.dist[v]) {
                res.dist[v]   = res.dist[u] + w;
                res.parent[v] = u;
                if (heap.contains(v))
                    heap.decreaseKey(v, res.dist[v]);
                else
                    heap.insert(res.dist[v], v);
            }
        }
    }

    if (!res.reachable && res.dist[dst] < 1e17) res.reachable = true;
    if (!res.reachable) return res;

    // Reconstruct path
    std::vector<int> rev;
    for (int v = dst; v != -1; v = res.parent[v]) rev.push_back(v);
    for (int i = (int)rev.size() - 1; i >= 0; i--) {
        res.path.push_back(rev[i]);
        res.pathNames.push_back(g.getLocation(rev[i]).name);
    }

    // Calculate actual distance and time along path
    for (int i = 0; i + 1 < (int)res.path.size(); i++) {
        int u2 = res.path[i], v2 = res.path[i+1];
        for (const auto& e : g.getAdj(u2)) {
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

// ============================================================
// Dijkstra::printResult
// ============================================================

void Dijkstra::printResult(const Graph& g, const DijkstraResult& res,
                           int src, int dst) {
    const char* modeStr = (res.mode == CostMode::DISTANCE) ? "Distance" :
                          (res.mode == CostMode::TIME)     ? "Time"     : "Traffic-Weighted";

    printf("\n  ╔══════════════════════════════════════════════════════╗\n");
    printf("  ║              DIJKSTRA SHORTEST PATH                 ║\n");
    printf("  ╠══════════════════════════════════════════════════════╣\n");
    printf("  ║  Mode           : %-34s║\n", modeStr);
    printf("  ║  Source         : %-34s║\n", g.getLocation(src).name.c_str());
    printf("  ║  Destination    : %-34s║\n", g.getLocation(dst).name.c_str());
    printf("  ║  Nodes Explored : %-34d║\n", res.nodesExplored);
    printf("  ╠══════════════════════════════════════════════════════╣\n");

    if (!res.reachable) {
        printf("  ║  ✗  No route found between these locations.         ║\n");
        printf("  ╚══════════════════════════════════════════════════════╝\n");
        return;
    }

    printf("  ║  Total Distance : %-6.0f km                           ║\n", res.totalDist);
    printf("  ║  Total Time     : %-4.1f hrs                           ║\n", res.totalTime);
    printf("  ║  Traffic Cost   : %-6.0f                              ║\n", res.totalCost);
    printf("  ╚══════════════════════════════════════════════════════╝\n");

    printf("\n  Best Route:\n");
    for (int i = 0; i < (int)res.pathNames.size(); i++) {
        if (i == 0)
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        else {
            // find edge details
            int u = res.path[i-1], v = res.path[i];
            for (const auto& e : g.getAdj(u)) {
                if (e.dst == v) {
                    printf("    │  %.0f km, %.1f hr [Traffic: %s]\n",
                           e.road.distance, e.road.travelTime,
                           trafficStr(e.road.traffic).c_str());
                    break;
                }
            }
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        }
    }
    printf("\n");
}
