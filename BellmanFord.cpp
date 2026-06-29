#include "BellmanFord.h"
#include <cstdio>

BFordResult BellmanFord::run(const Graph& g, int src, int dst) {
    int N = g.getNextId();
    BFordResult res;
    res.dist.assign(N, 1e18);
    res.parent.assign(N, -1);
    res.totalDist    = 0;
    res.totalTime    = 0;
    res.reachable    = false;
    res.negativeCycle = false;
    res.iterations   = 0;

    res.dist[src] = 0.0;

    // Collect all edges
    struct Edge { int u, v; double w, dist, time; };
    std::vector<Edge> edges;
    for (int u = 0; u < N; u++) {
        if (!g.isValid(u)) continue;
        for (const auto& e : g.getAdj(u)) {
            if (!g.isValid(e.dst)) continue;
            if (e.road.status != RoadStatus::OPEN) continue;
            edges.push_back({u, e.dst, e.road.distance,
                             e.road.distance, e.road.travelTime});
        }
    }

    // Relax V-1 times
    for (int i = 0; i < N - 1; i++) {
        bool updated = false;
        res.iterations++;
        for (const auto& e : edges) {
            if (res.dist[e.u] < 1e17 && res.dist[e.u] + e.w < res.dist[e.v]) {
                res.dist[e.v]   = res.dist[e.u] + e.w;
                res.parent[e.v] = e.u;
                updated = true;
            }
        }
        if (!updated) break;
    }

    // Check for negative cycles
    for (const auto& e : edges) {
        if (res.dist[e.u] < 1e17 && res.dist[e.u] + e.w < res.dist[e.v]) {
            res.negativeCycle = true;
        }
    }

    if (res.dist[dst] >= 1e17) return res;
    res.reachable = true;

    // Reconstruct
    std::vector<int> rev;
    for (int v = dst; v != -1; v = res.parent[v]) rev.push_back(v);
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
                break;
            }
        }
    }
    return res;
}

void BellmanFord::printResult(const Graph& g, const BFordResult& res, int src, int dst) {
    printf("\n  ╔══════════════════════════════════════════════════════╗\n");
    printf("  ║           BELLMAN-FORD SHORTEST PATH                ║\n");
    printf("  ╠══════════════════════════════════════════════════════╣\n");
    printf("  ║  Source         : %-34s║\n", g.getLocation(src).name.c_str());
    printf("  ║  Destination    : %-34s║\n", g.getLocation(dst).name.c_str());
    printf("  ║  Iterations     : %-34d║\n", res.iterations);
    printf("  ║  Negative Cycle : %-34s║\n", res.negativeCycle ? "DETECTED ⚠" : "None");
    printf("  ╠══════════════════════════════════════════════════════╣\n");

    if (!res.reachable) {
        printf("  ║  ✗  No route found.                                 ║\n");
        printf("  ╚══════════════════════════════════════════════════════╝\n");
        return;
    }

    printf("  ║  Total Distance : %-6.0f km                           ║\n", res.totalDist);
    printf("  ║  Total Time     : %-4.1f hrs                           ║\n", res.totalTime);
    printf("  ╚══════════════════════════════════════════════════════╝\n");

    printf("\n  Route:\n");
    for (int i = 0; i < (int)res.pathNames.size(); i++) {
        if (i == 0) printf("  ◉ %s\n", res.pathNames[i].c_str());
        else {
            int u = res.path[i-1], v = res.path[i];
            for (const auto& e : g.getAdj(u))
                if (e.dst == v) { printf("    │  %.0f km\n", e.road.distance); break; }
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        }
    }
    printf("\n");
}
