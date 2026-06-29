#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <cstdio>

// ============================================================
// Timer — high-resolution wall-clock timer for benchmarking
// ============================================================

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_;
    double elapsed_ms_;

public:
    Timer() : elapsed_ms_(0) {}

    void start() {
        start_ = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        auto end = std::chrono::high_resolution_clock::now();
        elapsed_ms_ = std::chrono::duration<double, std::milli>(end - start_).count();
    }

    double getMs()  const { return elapsed_ms_; }
    double getSec() const { return elapsed_ms_ / 1000.0; }
};

// ============================================================
// Helper — shared display utilities
// ============================================================

namespace Helper {

    inline void printBanner() {
        printf("\n");
        printf("  ╔══════════════════════════════════════════════════════════════╗\n");
        printf("  ║                                                              ║\n");
        printf("  ║        SMART ROUTE OPTIMIZER  PRO MAX  v1.0                 ║\n");
        printf("  ║     Advanced Graph-Based Navigation & Route Intelligence     ║\n");
        printf("  ║                                                              ║\n");
        printf("  ╚══════════════════════════════════════════════════════════════╝\n");
        printf("\n");
    }

    inline void printMenu() {
        printf("\n");
        printf("  ┌─────────────────────────────────────────────────────────────┐\n");
        printf("  │                    MAIN MENU                                │\n");
        printf("  ├─────────────────────────────────────────────────────────────┤\n");
        printf("  │  CITY BUILDER                                               │\n");
        printf("  │   1. Load Demo City (South India Network)                   │\n");
        printf("  │   2. Add Location                                           │\n");
        printf("  │   3. Add Road                                               │\n");
        printf("  │   4. Remove Location                                        │\n");
        printf("  │   5. Remove Road                                            │\n");
        printf("  │   6. Update Road                                            │\n");
        printf("  │   7. Display Network                                        │\n");
        printf("  │   8. Search Location                                        │\n");
        printf("  ├─────────────────────────────────────────────────────────────┤\n");
        printf("  │  TRAVERSAL                                                  │\n");
        printf("  │   9. BFS Traversal                                          │\n");
        printf("  │  10. DFS Traversal                                          │\n");
        printf("  ├─────────────────────────────────────────────────────────────┤\n");
        printf("  │  ROUTE FINDING                                              │\n");
        printf("  │  11. Dijkstra Shortest Route                                │\n");
        printf("  │  12. A* Smart Route                                         │\n");
        printf("  │  13. Bellman-Ford (Handles Negative Weights)                │\n");
        printf("  │  14. Floyd-Warshall (All-Pairs Distance Table)              │\n");
        printf("  ├─────────────────────────────────────────────────────────────┤\n");
        printf("  │  NETWORK ANALYSIS                                           │\n");
        printf("  │  15. Kruskal MST (Minimum Road Network)                    │\n");
        printf("  │  16. Traffic Update                                         │\n");
        printf("  │  17. Emergency Priority Routing                             │\n");
        printf("  │  18. Alternative Routes                                     │\n");
        printf("  │  19. Algorithm Benchmark                                    │\n");
        printf("  │  20. ASCII Graph Visualizer                                 │\n");
        printf("  ├─────────────────────────────────────────────────────────────┤\n");
        printf("  │   0. Exit                                                   │\n");
        printf("  └─────────────────────────────────────────────────────────────┘\n");
        printf("  Choice: ");
    }

    inline void printSeparator() {
        printf("  ────────────────────────────────────────────────────────────\n");
    }

    inline void printRoute(const std::vector<std::string>& path) {
        if (path.empty()) { printf("  (no path)\n"); return; }
        for (int i = 0; i < (int)path.size(); i++) {
            if (i == 0)
                printf("  ◉ %s\n", path[i].c_str());
            else
                printf("    ↓\n  ◉ %s\n", path[i].c_str());
        }
    }

    inline int readInt(const char* prompt) {
        int v;
        printf("%s", prompt);
        while (scanf("%d", &v) != 1) {
            // flush bad input
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("  [!] Enter a valid integer: ");
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {} // flush newline
        return v;
    }

    inline double readDouble(const char* prompt) {
        double v;
        printf("%s", prompt);
        while (scanf("%lf", &v) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("  [!] Enter a valid number: ");
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return v;
    }

    inline std::string readString(const char* prompt) {
        char buf[256];
        printf("%s", prompt);
        fgets(buf, sizeof(buf), stdin);
        std::string s(buf);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
        return s;
    }

} // namespace Helper
