#pragma once
#include <vector>

// ============================================================
// Disjoint Set Union (Union-Find) with path compression
// and union by rank — used by Kruskal MST
// ============================================================

class DisjointSet {
private:
    std::vector<int> parent, rank_;

public:
    explicit DisjointSet(int n) : parent(n), rank_(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);   // path compression
        return parent[x];
    }

    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;         // already same set
        if (rank_[px] < rank_[py]) std::swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        return true;
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};
