#include "DFS.h"
#include "../DataStructures/Stack.h"
#include <cstdio>

// ============================================================
// Iterative DFS with cycle detection (using coloring)
// ============================================================

void DFS::dfsVisit(const Graph& g, int start,
                   std::vector<bool>& visited,
                   std::vector<int>& color,
                   DFSResult& res,
                   int /*parent*/) {
    Stack<std::pair<int,int>> stk; // (node, parentNode)
    stk.push({start, -1});

    while (!stk.isEmpty()) {
        auto [cur, par] = stk.pop();
        if (visited[cur]) {
            // We reached a gray node → back edge → cycle
            if (color[cur] == 1) res.hasCycle = true;
            continue;
        }
        visited[cur] = true;
        color[cur]   = 1; // gray — in stack
        res.visitOrder.push_back(cur);
        res.visitNames.push_back(g.getLocation(cur).name);
        res.nodesExplored++;

        // Push neighbours (reverse so leftmost is visited first)
        const auto& adj = g.getAdj(cur);
        for (int i = (int)adj.size() - 1; i >= 0; i--) {
            int nb = adj[i].dst;
            if (!g.isValid(nb)) continue;
            if (adj[i].road.status != RoadStatus::OPEN) continue;
            if (!visited[nb]) {
                stk.push({nb, cur});
            } else if (nb != par && color[nb] == 1) {
                res.hasCycle = true;
            }
        }
        color[cur] = 2; // black — fully processed
    }
}

DFSResult DFS::traverse(const Graph& g, int src) {
    DFSResult res;
    res.hasCycle      = false;
    res.components    = 0;
    res.nodesExplored = 0;

    int N = g.getNextId();
    std::vector<bool> visited(N, false);
    std::vector<int>  color(N, 0);

    if (g.isValid(src)) {
        res.components++;
        dfsVisit(g, src, visited, color, res, -1);
    }
    return res;
}

DFSResult DFS::fullTraversal(const Graph& g) {
    DFSResult res;
    res.hasCycle      = false;
    res.components    = 0;
    res.nodesExplored = 0;

    int N = g.getNextId();
    std::vector<bool> visited(N, false);
    std::vector<int>  color(N, 0);

    for (int i = 0; i < N; i++) {
        if (g.isValid(i) && !visited[i]) {
            res.components++;
            dfsVisit(g, i, visited, color, res, -1);
        }
    }
    return res;
}

void DFS::printResult(const Graph& g, const DFSResult& res) {
    printf("\n  ┌── DFS RESULT ──────────────────────────────────────────┐\n");
    printf("  │ Nodes Explored  : %-38d│\n", res.nodesExplored);
    printf("  │ Components      : %-38d│\n", res.components);
    printf("  │ Cycle Detected  : %-38s│\n", res.hasCycle ? "YES" : "NO");
    printf("  └────────────────────────────────────────────────────────┘\n");

    printf("\n  DFS Visit Order:\n  ");
    for (int i = 0; i < (int)res.visitNames.size(); i++) {
        printf("%s", res.visitNames[i].c_str());
        if (i + 1 < (int)res.visitNames.size()) printf(" → ");
        if ((i + 1) % 4 == 0) printf("\n  ");
    }
    printf("\n");
}
