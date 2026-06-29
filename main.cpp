// ============================================================
//  Smart Route Optimizer Pro MAX  —  main.cpp
//  Entry point & full interactive CLI
// ============================================================

#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

#include "Graph/Graph.h"
#include "Algorithms/BFS.h"
#include "Algorithms/DFS.h"
#include "Algorithms/Dijkstra.h"
#include "Algorithms/AStar.h"
#include "Algorithms/BellmanFord.h"
#include "Algorithms/FloydWarshall.h"
#include "Algorithms/Kruskal.h"
#include "Utils/Helper.h"
#include "DataStructures/MinHeap.h"

// ============================================================
// Forward declarations for feature modules
// ============================================================

static void featureAddLocation(Graph& g);
static void featureAddRoad(Graph& g);
static void featureRemoveLocation(Graph& g);
static void featureRemoveRoad(Graph& g);
static void featureUpdateRoad(Graph& g);
static void featureSearchLocation(const Graph& g);
static void featureBFS(const Graph& g);
static void featureDFS(const Graph& g);
static void featureDijkstra(const Graph& g);
static void featureAStar(const Graph& g);
static void featureBellmanFord(const Graph& g);
static void featureFloydWarshall(const Graph& g);
static void featureKruskal(const Graph& g);
static void featureTrafficUpdate(Graph& g);
static void featureEmergencyRouting(const Graph& g);
static void featureAlternativeRoutes(const Graph& g);
static void featureBenchmark(const Graph& g);
static void featureASCIIVisualizer(const Graph& g);

// ============================================================
// Helpers shared across features
// ============================================================

static int pickNode(const Graph& g, const char* label) {
    printf("  Enter %s (id or name): ", label);
    char buf[256];
    fgets(buf, sizeof(buf), stdin);
    std::string s(buf);
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();

    // Try integer
    bool allDigit = !s.empty();
    for (char c : s) if (c < '0' || c > '9') { allDigit = false; break; }
    if (allDigit && !s.empty()) {
        int id = std::stoi(s);
        if (g.isValid(id)) return id;
        printf("  [!] No node with id %d\n", id);
        return -1;
    }
    // Try name search
    int id = g.searchLocation(s);
    if (id == -1) printf("  [!] Location '%s' not found\n", s.c_str());
    return id;
}

// ============================================================
// Feature: Add Location
// ============================================================

static void featureAddLocation(Graph& g) {
    printf("\n  ── Add New Location ──\n");
    std::string name = Helper::readString("  Location name: ");
    double lat  = Helper::readDouble("  Latitude  (e.g. 17.38): ");
    double lng  = Helper::readDouble("  Longitude (e.g. 78.47): ");

    printf("  Category  0=City 1=Town 2=Village 3=Junction 4=Airport 5=Other\n");
    int cat = Helper::readInt("  Choice: ");
    LocationCategory lc = (LocationCategory)std::min(cat, 5);

    int id = g.addLocation(name, lat, lng, lc);
    if (id != -1)
        printf("  [✓] '%s' added with ID %d\n", name.c_str(), id);
}

// ============================================================
// Feature: Add Road
// ============================================================

static void featureAddRoad(Graph& g) {
    printf("\n  ── Add New Road ──\n");
    g.displayGraph();
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    double dist = Helper::readDouble("  Distance (km): ");
    double time = Helper::readDouble("  Travel time (hours): ");
    printf("  Traffic  0=LOW  1=MEDIUM  2=HIGH\n");
    int tf = Helper::readInt("  Traffic level: ");
    TrafficLevel tl = (tf == 1) ? TrafficLevel::MEDIUM :
                      (tf == 2) ? TrafficLevel::HIGH : TrafficLevel::LOW;

    if (g.addRoad(src, dst, dist, time, tl))
        printf("  [✓] Road %s ↔ %s added.\n",
               g.getLocation(src).name.c_str(),
               g.getLocation(dst).name.c_str());
}

// ============================================================
// Feature: Remove Location
// ============================================================

static void featureRemoveLocation(Graph& g) {
    printf("\n  ── Remove Location ──\n");
    int id = pickNode(g, "Location to remove");
    if (id == -1) return;
    std::string name = g.getLocation(id).name;
    if (g.removeLocation(id))
        printf("  [✓] '%s' removed.\n", name.c_str());
}

// ============================================================
// Feature: Remove Road
// ============================================================

static void featureRemoveRoad(Graph& g) {
    printf("\n  ── Remove Road ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    if (g.removeRoad(src, dst))
        printf("  [✓] Road removed.\n");
    else
        printf("  [!] Road not found.\n");
}

// ============================================================
// Feature: Update Road
// ============================================================

static void featureUpdateRoad(Graph& g) {
    printf("\n  ── Update Road ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    double dist = Helper::readDouble("  New Distance (km): ");
    double time = Helper::readDouble("  New Travel time (hours): ");
    printf("  Traffic  0=LOW  1=MEDIUM  2=HIGH\n");
    int tf = Helper::readInt("  Traffic level: ");
    TrafficLevel tl = (tf == 1) ? TrafficLevel::MEDIUM :
                      (tf == 2) ? TrafficLevel::HIGH : TrafficLevel::LOW;
    if (g.updateRoad(src, dst, dist, time, tl))
        printf("  [✓] Road updated.\n");
    else
        printf("  [!] Road not found.\n");
}

// ============================================================
// Feature: Search Location
// ============================================================

static void featureSearchLocation(const Graph& g) {
    printf("\n  ── Search Location ──\n");
    std::string name = Helper::readString("  Enter name: ");
    int id = g.searchLocation(name);
    if (id == -1) { printf("  [✗] Not found.\n"); return; }
    printf("  [✓] Found:\n");
    g.displayNode(id);
    printf("\n  Connected roads:\n");
    for (const auto& e : g.getAdj(id)) {
        if (!g.isValid(e.dst)) continue;
        printf("    ──► %-20s  %.0f km  %.1f hr  [%s]\n",
               g.getLocation(e.dst).name.c_str(),
               e.road.distance, e.road.travelTime,
               trafficStr(e.road.traffic).c_str());
    }
}

// ============================================================
// Feature: BFS
// ============================================================

static void featureBFS(const Graph& g) {
    printf("\n  ── BFS Traversal ──\n");
    printf("  1. Full traversal from source\n");
    printf("  2. Minimum-hop path\n");
    int ch = Helper::readInt("  Choice: ");

    int src = pickNode(g, "Source"); if (src == -1) return;

    if (ch == 2) {
        int dst = pickNode(g, "Destination"); if (dst == -1) return;
        auto res = BFS::shortestHop(g, src, dst);
        BFS::printResult(g, res, src, dst);
    } else {
        auto res = BFS::traverse(g, src);
        BFS::printResult(g, res, src);
    }
}

// ============================================================
// Feature: DFS
// ============================================================

static void featureDFS(const Graph& g) {
    printf("\n  ── DFS Traversal ──\n");
    printf("  1. From specific source\n");
    printf("  2. Full graph (all components)\n");
    int ch = Helper::readInt("  Choice: ");

    DFSResult res;
    if (ch == 2) {
        res = DFS::fullTraversal(g);
    } else {
        int src = pickNode(g, "Source"); if (src == -1) return;
        res = DFS::traverse(g, src);
    }
    DFS::printResult(g, res);
}

// ============================================================
// Feature: Dijkstra
// ============================================================

static void featureDijkstra(const Graph& g) {
    printf("\n  ── Dijkstra Shortest Route ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    printf("  Optimise by:  1=Distance  2=Time  3=Traffic-Weight\n");
    int m = Helper::readInt("  Mode: ");
    CostMode mode = (m == 2) ? CostMode::TIME :
                    (m == 3) ? CostMode::TRAFFIC : CostMode::DISTANCE;
    auto res = Dijkstra::run(g, src, dst, mode);
    Dijkstra::printResult(g, res, src, dst);
}

// ============================================================
// Feature: A*
// ============================================================

static void featureAStar(const Graph& g) {
    printf("\n  ── A* Smart Route ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    auto res = AStar::run(g, src, dst);
    AStar::printResult(g, res, src, dst);
}

// ============================================================
// Feature: Bellman-Ford
// ============================================================

static void featureBellmanFord(const Graph& g) {
    printf("\n  ── Bellman-Ford ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;
    auto res = BellmanFord::run(g, src, dst);
    BellmanFord::printResult(g, res, src, dst);
}

// ============================================================
// Feature: Floyd-Warshall
// ============================================================

static void featureFloydWarshall(const Graph& g) {
    printf("\n  ── Floyd-Warshall All-Pairs Shortest Path ──\n");
    printf("  1. Display full distance table\n");
    printf("  2. Show path between two nodes\n");
    int ch = Helper::readInt("  Choice: ");

    printf("  [Computing Floyd-Warshall...]\n");
    auto res = FloydWarshall::run(g);

    if (ch == 1) {
        FloydWarshall::printDistanceTable(g, res);
    } else {
        int src = pickNode(g, "Source");      if (src == -1) return;
        int dst = pickNode(g, "Destination"); if (dst == -1) return;
        FloydWarshall::printPath(g, res, src, dst);
    }
}

// ============================================================
// Feature: Kruskal MST
// ============================================================

static void featureKruskal(const Graph& g) {
    printf("\n  ── Kruskal Minimum Spanning Tree ──\n");
    auto res = Kruskal::run(g);
    Kruskal::printResult(res);
}

// ============================================================
// Feature: Traffic Update
// ============================================================

static void featureTrafficUpdate(Graph& g) {
    printf("\n  ── Traffic Update ──\n");
    int src = pickNode(g, "Road Source");      if (src == -1) return;
    int dst = pickNode(g, "Road Destination"); if (dst == -1) return;
    printf("  New traffic:  0=LOW  1=MEDIUM  2=HIGH\n");
    int tf = Helper::readInt("  Level: ");
    TrafficLevel tl = (tf == 1) ? TrafficLevel::MEDIUM :
                      (tf == 2) ? TrafficLevel::HIGH : TrafficLevel::LOW;
    if (g.setTraffic(src, dst, tl)) {
        printf("  [✓] Traffic on %s ↔ %s updated to %s\n",
               g.getLocation(src).name.c_str(),
               g.getLocation(dst).name.c_str(),
               trafficStr(tl).c_str());
        printf("  Tip: Re-run Dijkstra with 'Traffic-Weight' mode to see impact.\n");
    } else {
        printf("  [!] Road not found.\n");
    }
}

// ============================================================
// Feature: Emergency Priority Routing
// ============================================================

static void featureEmergencyRouting(const Graph& g) {
    printf("\n  ── Emergency Priority Routing ──\n");
    printf("  Mode:  1=Normal  2=Ambulance  3=Police  4=Fire Rescue\n");
    int mode = Helper::readInt("  Mode: ");

    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;

    const char* modeNames[] = {"", "NORMAL", "AMBULANCE 🚑", "POLICE 🚔", "FIRE RESCUE 🚒"};
    printf("\n  ┌── EMERGENCY ROUTING — %s ─────────────────────────┐\n",
           modeNames[std::min(mode,4)]);

    // Emergency uses time-optimised route
    CostMode cm = (mode == 1) ? CostMode::DISTANCE : CostMode::TIME;
    auto res = Dijkstra::run(g, src, dst, cm);

    if (!res.reachable) {
        printf("  │  ✗ No route available.                                │\n");
        printf("  └───────────────────────────────────────────────────────┘\n");
        return;
    }

    printf("  │  Optimise By    : %-34s│\n",
           (mode == 1) ? "Distance (km)" : "Time (fastest)");
    printf("  │  Total Distance : %-6.0f km                           │\n", res.totalDist);
    printf("  │  Total Time     : %-4.1f hrs                           │\n", res.totalTime);
    printf("  └───────────────────────────────────────────────────────┘\n");

    printf("\n  Emergency Route:\n");
    for (int i = 0; i < (int)res.pathNames.size(); i++) {
        if (i == 0) printf("  ◉ %s\n", res.pathNames[i].c_str());
        else {
            int u = res.path[i-1], v = res.path[i];
            for (const auto& e : g.getAdj(u))
                if (e.dst == v) { printf("    │  %.0f km / %.1f hr\n", e.road.distance, e.road.travelTime); break; }
            printf("  ◉ %s\n", res.pathNames[i].c_str());
        }
    }
    printf("\n");
}

// ============================================================
// Feature: Alternative Routes (k-path via path blocking)
// ============================================================

static void featureAlternativeRoutes(const Graph& g) {
    printf("\n  ── Alternative Routes (Top 3) ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;

    // Simple Yen's-style: find route 1, block edges, find route 2, etc.
    // We implement a lightweight version: find route, temporarily mark edge used,
    // run again with slightly increased weights on used edges.

    struct RouteResult { std::vector<std::string> names; double dist; double time; };
    std::vector<RouteResult> routes;

    // We'll collect paths by running Dijkstra normally, then re-running after
    // temporarily removing each edge in the previous path (simple variation).
    // For production, full Yen's k-shortest would be used.

    auto copyGraph = [&]() -> Graph {
        // Build a copy by re-adding same nodes/edges
        // (For simplicity we work with the existing graph but track "blocked" edges)
        return Graph(); // placeholder — see below for actual approach
    };
    (void)copyGraph;

    // Approach: run Dijkstra 3 times, each time with one more edge of prior path removed
    std::vector<std::pair<int,int>> blocked;

    auto runWithBlocked = [&](const std::vector<std::pair<int,int>>& block) -> DijkstraResult {
        // We temporarily update road costs for blocked edges to infinity by removing them
        // Since we can't easily "const Graph" modify, we reconstruct DijkstraResult manually.
        int N = g.getNextId();
        DijkstraResult res2;
        res2.dist.assign(N, 1e18);
        res2.parent.assign(N, -1);
        res2.totalDist = res2.totalTime = res2.totalCost = 0;
        res2.nodesExplored = 0;
        res2.reachable = false;
        res2.mode = CostMode::DISTANCE;

        // Check if an edge is blocked
        auto isBlocked = [&](int u, int v) {
            for (const auto& p : block)
                if (p.first == u && p.second == v) return true;
            return false;
        };

        // MinHeap Dijkstra inline (can't reuse static since we need blocking)
        MinHeap heap; heap.build(N);
        res2.dist[src] = 0;
        heap.insert(0, src);
        while (!heap.isEmpty()) {
            auto hn = heap.extractMin();
            int u = hn.nodeId; res2.nodesExplored++;
            if (u == dst) { res2.reachable = true; break; }
            for (const auto& e : g.getAdj(u)) {
                int v = e.dst;
                if (!g.isValid(v)) continue;
                if (e.road.status != RoadStatus::OPEN) continue;
                if (isBlocked(u, v) || isBlocked(v, u)) continue;
                double w = e.road.distance;
                if (res2.dist[u] + w < res2.dist[v]) {
                    res2.dist[v] = res2.dist[u] + w;
                    res2.parent[v] = u;
                    if (heap.contains(v)) heap.decreaseKey(v, res2.dist[v]);
                    else                  heap.insert(res2.dist[v], v);
                }
            }
        }
        if (!res2.reachable) return res2;
        std::vector<int> rev;
        for (int v = dst; v != -1; v = res2.parent[v]) rev.push_back(v);
        for (int i = (int)rev.size()-1; i >= 0; i--) {
            res2.path.push_back(rev[i]);
            res2.pathNames.push_back(g.getLocation(rev[i]).name);
        }
        for (int i = 0; i+1 < (int)res2.path.size(); i++) {
            int u2 = res2.path[i], v2 = res2.path[i+1];
            for (const auto& e : g.getAdj(u2)) if (e.dst == v2) {
                res2.totalDist += e.road.distance;
                res2.totalTime += e.road.travelTime;
                break;
            }
        }
        return res2;
    };

    printf("\n");
    for (int k = 0; k < 3; k++) {
        auto res = runWithBlocked(blocked);
        if (!res.reachable) break;

        printf("  ── Route %d ──────────────────────────────────────────────\n", k+1);
        for (int i = 0; i < (int)res.pathNames.size(); i++) {
            if (i == 0) printf("  ◉ %s\n", res.pathNames[i].c_str());
            else        printf("    ↓\n  ◉ %s\n", res.pathNames[i].c_str());
        }
        printf("  Distance: %.0f km | Time: %.1f hr\n\n", res.totalDist, res.totalTime);

        // Block a middle edge of this path for next iteration
        if (res.path.size() >= 2) {
            int mid = (int)res.path.size() / 2;
            blocked.push_back({res.path[mid-1], res.path[mid]});
        }
    }
}

// ============================================================
// Feature: Algorithm Benchmark
// ============================================================

static void featureBenchmark(const Graph& g) {
    printf("\n  ── Algorithm Benchmark ──\n");
    int src = pickNode(g, "Source");      if (src == -1) return;
    int dst = pickNode(g, "Destination"); if (dst == -1) return;

    Timer t;

    printf("\n  Running algorithms...\n\n");
    printf("  %-18s  %-12s  %-12s  %-10s  %s\n",
           "Algorithm", "Time (ms)", "Dist (km)", "Nodes", "Found");
    printf("  ");
    for (int i = 0; i < 66; i++) printf("─");
    printf("\n");

    // BFS
    {
        t.start();
        auto r = BFS::shortestHop(g, src, dst);
        t.stop();
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "BFS (min hops)", t.getMs(), "N/A",
               r.nodesExplored, r.reachable ? "Yes" : "No");
    }
    // DFS
    {
        t.start();
        auto r = DFS::traverse(g, src);
        t.stop();
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "DFS", t.getMs(), "N/A",
               r.nodesExplored, "Traversal");
    }
    // Dijkstra
    {
        t.start();
        auto r = Dijkstra::run(g, src, dst, CostMode::DISTANCE);
        t.stop();
        char distBuf[32];
        if (r.reachable) snprintf(distBuf, 32, "%.0f", r.totalDist);
        else             snprintf(distBuf, 32, "N/A");
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "Dijkstra", t.getMs(), distBuf,
               r.nodesExplored, r.reachable ? "Yes" : "No");
    }
    // A*
    {
        t.start();
        auto r = AStar::run(g, src, dst);
        t.stop();
        char distBuf[32];
        if (r.reachable) snprintf(distBuf, 32, "%.0f", r.totalDist);
        else             snprintf(distBuf, 32, "N/A");
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "A*", t.getMs(), distBuf,
               r.nodesExplored, r.reachable ? "Yes" : "No");
    }
    // Bellman-Ford
    {
        t.start();
        auto r = BellmanFord::run(g, src, dst);
        t.stop();
        char distBuf[32];
        if (r.reachable) snprintf(distBuf, 32, "%.0f", r.totalDist);
        else             snprintf(distBuf, 32, "N/A");
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "Bellman-Ford", t.getMs(), distBuf,
               r.iterations, r.reachable ? "Yes" : "No");
    }
    // Floyd-Warshall
    {
        t.start();
        auto r = FloydWarshall::run(g);
        t.stop();
        int N = g.getNextId();
        char distBuf[32];
        if (src < N && dst < N && r.dist[src][dst] < 1e17)
            snprintf(distBuf, 32, "%.0f", r.dist[src][dst]);
        else snprintf(distBuf, 32, "N/A");
        printf("  %-18s  %-12.4f  %-12s  %-10d  %s\n",
               "Floyd-Warshall", t.getMs(), distBuf, N*N, "All-pairs");
    }
    printf("\n  Note: Times may vary. Run multiple times for stable averages.\n\n");
}

// ============================================================
// Feature: ASCII Graph Visualizer
// ============================================================

static void featureASCIIVisualizer(const Graph& g) {
    printf("\n  ── ASCII Network Visualizer ──\n");

    // Collect active nodes
    std::vector<int> nodes;
    for (int i = 0; i < g.getNextId(); i++)
        if (g.isValid(i)) nodes.push_back(i);

    if (nodes.empty()) { printf("  No nodes to display.\n"); return; }

    // Simple grid placement based on lat/lng normalised to 60x20 canvas
    const int W = 72, H = 22;
    char canvas[H][W+1];
    for (int r = 0; r < H; r++) { for (int c = 0; c < W; c++) canvas[r][c] = ' '; canvas[r][W] = '\0'; }

    // Find lat/lng bounds
    double minLat = 1e9, maxLat = -1e9, minLng = 1e9, maxLng = -1e9;
    for (int id : nodes) {
        const auto& loc = g.getLocation(id);
        minLat = std::min(minLat, loc.lat); maxLat = std::max(maxLat, loc.lat);
        minLng = std::min(minLng, loc.lng); maxLng = std::max(maxLng, loc.lng);
    }
    double latRange = (maxLat == minLat) ? 1 : maxLat - minLat;
    double lngRange = (maxLng == minLng) ? 1 : maxLng - minLng;

    struct Pos { int r, c; };
    std::vector<Pos> pos(g.getNextId(), {0,0});

    // Map node positions
    for (int id : nodes) {
        const auto& loc = g.getLocation(id);
        int col = (int)((loc.lng - minLng) / lngRange * (W - 12)) + 2;
        int row = (int)((maxLat - loc.lat) / latRange * (H - 4)) + 1;
        col = std::max(2, std::min(W-12, col));
        row = std::max(1, std::min(H-3, row));
        pos[id] = {row, col};
    }

    // Draw edges (simple ASCII lines — just mark midpoints)
    for (int id : nodes) {
        for (const auto& e : g.getAdj(id)) {
            if (e.dst <= id) continue; // draw once
            if (!g.isValid(e.dst)) continue;
            int r1 = pos[id].r, c1 = pos[id].c;
            int r2 = pos[e.dst].r, c2 = pos[e.dst].c;
            // Draw Bresenham line
            int dr = abs(r2-r1), dc = abs(c2-c1);
            int sr = (r1<r2)?1:-1, sc = (c1<c2)?1:-1;
            int err = dr-dc;
            int cr = r1, cc = c1;
            while (cr != r2 || cc != c2) {
                if (cr>=0 && cr<H && cc>=0 && cc<W && canvas[cr][cc]==' ')
                    canvas[cr][cc] = (dr > dc*2) ? '|' : (dc > dr*2) ? '-' : '.';
                int e2 = 2*err;
                if (e2>-dc){ err-=dc; cr+=sr; }
                if (e2< dr){ err+=dr; cc+=sc; }
            }
        }
    }

    // Draw node labels
    for (int id : nodes) {
        int r = pos[id].r, c = pos[id].c;
        std::string label = std::to_string(id) + ":" + g.getLocation(id).name.substr(0,8);
        // Place marker
        if (r>=0 && r<H && c>=0 && c<W) canvas[r][c] = '*';
        // Place label below
        int lr = r+1;
        if (lr < H) {
            for (int i = 0; i < (int)label.size() && c+i < W; i++)
                canvas[lr][c+i] = label[i];
        }
    }

    // Print canvas
    printf("\n");
    printf("  ┌");
    for (int i = 0; i < W; i++) printf("─");
    printf("┐\n");
    for (int r = 0; r < H; r++) {
        printf("  │%s│\n", canvas[r]);
    }
    printf("  └");
    for (int i = 0; i < W; i++) printf("─");
    printf("┘\n");
    printf("  (* = city,  . = road connection)\n\n");
}

// ============================================================
// MAIN
// ============================================================

int main() {
    Helper::printBanner();

    Graph g;
    bool running = true;

    printf("  Tip: Start with option 1 to load the demo South India city network.\n");

    while (running) {
        Helper::printMenu();

        int choice;
        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        { int c; while ((c = getchar()) != '\n' && c != EOF) {} }

        switch (choice) {
            case 1:
                g = Graph();
                g.loadDemoCity();
                printf("\n  [✓] Demo city loaded: South India Transportation Network\n");
                printf("  [✓] %d locations, %d roads ready.\n",
                       g.getNodeCount(), g.getEdgeCount()/2);
                break;
            case 2:  featureAddLocation(g);       break;
            case 3:  featureAddRoad(g);            break;
            case 4:  featureRemoveLocation(g);     break;
            case 5:  featureRemoveRoad(g);         break;
            case 6:  featureUpdateRoad(g);         break;
            case 7:  g.displayGraph();             break;
            case 8:  featureSearchLocation(g);     break;
            case 9:  featureBFS(g);                break;
            case 10: featureDFS(g);                break;
            case 11: featureDijkstra(g);           break;
            case 12: featureAStar(g);              break;
            case 13: featureBellmanFord(g);        break;
            case 14: featureFloydWarshall(g);      break;
            case 15: featureKruskal(g);            break;
            case 16: featureTrafficUpdate(g);      break;
            case 17: featureEmergencyRouting(g);   break;
            case 18: featureAlternativeRoutes(g);  break;
            case 19: featureBenchmark(g);          break;
            case 20: featureASCIIVisualizer(g);    break;
            case 0:
                printf("\n  Goodbye. Safe travels!\n\n");
                running = false;
                break;
            default:
                printf("  [!] Invalid option. Enter 0-20.\n");
        }
    }
    return 0;
}
