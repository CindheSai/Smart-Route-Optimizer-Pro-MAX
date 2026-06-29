#include "Kruskal.h"
#include "../DataStructures/DisjointSet.h"
#include <algorithm>
#include <cstdio>

KruskalResult Kruskal::run(const Graph& g) {
    int N = g.getNextId();
    KruskalResult res;
    res.totalCost  = 0;
    res.edgesUsed  = 0;
    res.connected  = false;

    // Collect all unique undirected edges
    struct RawEdge { int u, v; double w; };
    std::vector<RawEdge> edges;
    std::vector<std::vector<bool>> seen(N, std::vector<bool>(N, false));

    for (int u = 0; u < N; u++) {
        if (!g.isValid(u)) continue;
        for (const auto& e : g.getAdj(u)) {
            if (!g.isValid(e.dst)) continue;
            if (e.road.status != RoadStatus::OPEN) continue;
            int v = e.dst;
            if (!seen[u][v] && !seen[v][u]) {
                edges.push_back({u, v, e.road.distance});
                seen[u][v] = seen[v][u] = true;
            }
        }
    }

    // Sort edges by weight
    std::sort(edges.begin(), edges.end(),
              [](const RawEdge& a, const RawEdge& b){ return a.w < b.w; });

    DisjointSet dsu(N);
    int nodeCount = g.getNodeCount();

    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            MSTEdge me;
            me.src     = e.u;
            me.dst     = e.v;
            me.weight  = e.w;
            me.srcName = g.getLocation(e.u).name;
            me.dstName = g.getLocation(e.v).name;
            res.mstEdges.push_back(me);
            res.totalCost += e.w;
            res.edgesUsed++;
            if (res.edgesUsed == nodeCount - 1) { res.connected = true; break; }
        }
    }
    return res;
}

void Kruskal::printResult(const KruskalResult& res) {
    printf("\n  ╔══════════════════════════════════════════════════════╗\n");
    printf("  ║         KRUSKAL MINIMUM SPANNING TREE               ║\n");
    printf("  ╠══════════════════════════════════════════════════════╣\n");
    printf("  ║  Edges in MST   : %-34d║\n", res.edgesUsed);
    printf("  ║  Total MST Cost : %-6.0f km                          ║\n", res.totalCost);
    printf("  ║  Graph Connected: %-34s║\n", res.connected ? "YES ✓" : "NO (Forest)");
    printf("  ╚══════════════════════════════════════════════════════╝\n");

    printf("\n  Minimum Road Network:\n");
    printf("  %-20s   %-20s   %s\n", "From", "To", "Distance");
    printf("  ");
    for (int i = 0; i < 52; i++) printf("─");
    printf("\n");
    for (const auto& e : res.mstEdges)
        printf("  %-20s ──► %-20s  %.0f km\n",
               e.srcName.c_str(), e.dstName.c_str(), e.weight);
    printf("\n  Minimum infrastructure cost: %.0f km total road length\n\n", res.totalCost);
}
