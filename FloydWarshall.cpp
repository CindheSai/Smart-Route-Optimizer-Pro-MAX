#include "FloydWarshall.h"
#include <cstdio>
#include <cstring>

FWResult FloydWarshall::run(const Graph& g) {
    int N = g.getNextId();
    FWResult res;
    res.N = N;
    res.dist.assign(N, std::vector<double>(N, 1e18));
    res.next.assign(N, std::vector<int>(N, -1));

    // Init diagonal
    for (int i = 0; i < N; i++)
        if (g.isValid(i)) res.dist[i][i] = 0.0;

    // Init from adjacency list
    for (int u = 0; u < N; u++) {
        if (!g.isValid(u)) continue;
        for (const auto& e : g.getAdj(u)) {
            if (!g.isValid(e.dst)) continue;
            if (e.road.status != RoadStatus::OPEN) continue;
            if (e.road.distance < res.dist[u][e.dst]) {
                res.dist[u][e.dst] = e.road.distance;
                res.next[u][e.dst] = e.dst;
            }
        }
    }

    // Relax
    for (int k = 0; k < N; k++) {
        if (!g.isValid(k)) continue;
        for (int i = 0; i < N; i++) {
            if (!g.isValid(i) || res.dist[i][k] >= 1e17) continue;
            for (int j = 0; j < N; j++) {
                if (!g.isValid(j) || res.dist[k][j] >= 1e17) continue;
                if (res.dist[i][k] + res.dist[k][j] < res.dist[i][j]) {
                    res.dist[i][j] = res.dist[i][k] + res.dist[k][j];
                    res.next[i][j] = res.next[i][k];
                }
            }
        }
    }
    return res;
}

void FloydWarshall::printDistanceTable(const Graph& g, const FWResult& res) {
    printf("\n  ╔══ FLOYD-WARSHALL ALL-PAIRS DISTANCE TABLE (km) ══════╗\n");

    // Collect active node ids
    std::vector<int> ids;
    for (int i = 0; i < res.N; i++)
        if (g.isValid(i)) ids.push_back(i);

    // Header row
    printf("  %-16s", "");
    for (int j : ids)
        printf(" %-8s", g.getLocation(j).name.substr(0, 8).c_str());
    printf("\n  ");
    for (int i = 0; i <= (int)ids.size() * 9 + 16; i++) printf("─");
    printf("\n");

    for (int i : ids) {
        printf("  %-16s", g.getLocation(i).name.substr(0, 16).c_str());
        for (int j : ids) {
            if (res.dist[i][j] >= 1e17) printf(" %-8s", "∞");
            else                         printf(" %-8.0f", res.dist[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void FloydWarshall::printPath(const Graph& g, const FWResult& res, int src, int dst) {
    if (res.dist[src][dst] >= 1e17) {
        printf("  [✗] No path from %s to %s\n",
               g.getLocation(src).name.c_str(), g.getLocation(dst).name.c_str());
        return;
    }
    printf("\n  FW Path: %s → %s  (%.0f km)\n",
           g.getLocation(src).name.c_str(),
           g.getLocation(dst).name.c_str(),
           res.dist[src][dst]);

    int cur = src;
    printf("  ◉ %s\n", g.getLocation(cur).name.c_str());
    while (cur != dst) {
        int nxt = res.next[cur][dst];
        if (nxt == -1) break;
        printf("    ↓\n  ◉ %s\n", g.getLocation(nxt).name.c_str());
        cur = nxt;
    }
    printf("\n");
}
