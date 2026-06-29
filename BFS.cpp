#include "BFS.h"
#include "../DataStructures/Queue.h"
#include <vector>
#include <cstdio>

// ============================================================
// BFS::traverse — visit all reachable nodes from src
// ============================================================

BFSResult BFS::traverse(const Graph& g, int src) {
    BFSResult res;
    res.reachable = false;
    res.hops      = 0;
    res.nodesExplored = 0;

    int N = g.getNextId();
    std::vector<bool> visited(N, false);
    Queue<int> q;

    visited[src] = true;
    q.enqueue(src);
    res.visitOrder.push_back(src);
    res.nodesExplored++;

    while (!q.isEmpty()) {
        int cur = q.dequeue();
        for (const auto& e : g.getAdj(cur)) {
            if (!g.isValid(e.dst))            continue;
            if (e.road.status != RoadStatus::OPEN) continue;
            if (visited[e.dst])               continue;
            visited[e.dst] = true;
            q.enqueue(e.dst);
            res.visitOrder.push_back(e.dst);
            res.nodesExplored++;
        }
    }
    return res;
}

// ============================================================
// BFS::shortestHop — minimum hop path src -> dst
// ============================================================

BFSResult BFS::shortestHop(const Graph& g, int src, int dst) {
    BFSResult res;
    res.reachable     = false;
    res.hops          = 0;
    res.nodesExplored = 0;

    int N = g.getNextId();
    std::vector<bool> visited(N, false);
    std::vector<int>  parent(N, -1);
    Queue<int> q;

    visited[src] = true;
    q.enqueue(src);
    res.nodesExplored++;

    bool found = false;
    while (!q.isEmpty() && !found) {
        int cur = q.dequeue();
        res.visitOrder.push_back(cur);
        for (const auto& e : g.getAdj(cur)) {
            if (!g.isValid(e.dst))               continue;
            if (e.road.status != RoadStatus::OPEN) continue;
            if (visited[e.dst])                  continue;
            visited[e.dst] = true;
            parent[e.dst]  = cur;
            q.enqueue(e.dst);
            res.nodesExplored++;
            if (e.dst == dst) { found = true; break; }
        }
    }

    if (!found) return res;

    // Reconstruct path
    res.reachable = true;
    std::vector<int> rev;
    for (int v = dst; v != -1; v = parent[v]) rev.push_back(v);
    for (int i = (int)rev.size() - 1; i >= 0; i--) {
        res.path.push_back(rev[i]);
        res.pathNames.push_back(g.getLocation(rev[i]).name);
    }
    res.hops = (int)res.path.size() - 1;
    return res;
}

// ============================================================
// BFS::printResult
// ============================================================

void BFS::printResult(const Graph& g, const BFSResult& res, int src, int dst) {
    printf("\n  ┌── BFS RESULT ──────────────────────────────────────────┐\n");
    printf("  │ Source: %-48s│\n", g.getLocation(src).name.c_str());
    if (dst != -1)
        printf("  │ Destination: %-43s│\n", g.getLocation(dst).name.c_str());
    printf("  │ Nodes Explored: %-40d│\n", res.nodesExplored);
    printf("  └────────────────────────────────────────────────────────┘\n");

    printf("\n  Visit Order:\n  ");
    for (int i = 0; i < (int)res.visitOrder.size(); i++) {
        printf("%s", g.getLocation(res.visitOrder[i]).name.c_str());
        if (i + 1 < (int)res.visitOrder.size()) printf(" → ");
        if ((i + 1) % 4 == 0) printf("\n  ");
    }
    printf("\n");

    if (dst != -1) {
        if (!res.reachable) {
            printf("\n  [✗] No path found from %s to %s\n",
                   g.getLocation(src).name.c_str(), g.getLocation(dst).name.c_str());
        } else {
            printf("\n  Minimum-Hop Path (%d hops):\n", res.hops);
            for (int i = 0; i < (int)res.pathNames.size(); i++) {
                if (i == 0) printf("  ◉ %s\n", res.pathNames[i].c_str());
                else        printf("    ↓\n  ◉ %s\n", res.pathNames[i].c_str());
            }
        }
    }
}
