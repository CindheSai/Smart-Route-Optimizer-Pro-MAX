#pragma once
#include <vector>
#include <stdexcept>
#include <climits>

// ============================================================
// Custom Min Heap — used by Dijkstra and A* algorithms
// Stores (cost, nodeId) pairs; lowest cost always at root
// ============================================================

struct HeapNode {
    double cost;
    int    nodeId;
    HeapNode(double c, int n) : cost(c), nodeId(n) {}
};

class MinHeap {
private:
    std::vector<HeapNode> heap;
    std::vector<int>      pos;   // pos[nodeId] = index in heap (-1 = not present)
    int                   size_;

    int parent(int i) { return (i - 1) / 2; }
    int left  (int i) { return 2 * i + 1; }
    int right (int i) { return 2 * i + 2; }

    void swap(int i, int j) {
        pos[heap[i].nodeId] = j;
        pos[heap[j].nodeId] = i;
        std::swap(heap[i], heap[j]);
    }

    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)].cost > heap[i].cost) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

public:
    void heapifyDown(int i) {
        int smallest = i;
        int l = left(i), r = right(i);
        if (l < size_ && heap[l].cost < heap[smallest].cost) smallest = l;
        if (r < size_ && heap[r].cost < heap[smallest].cost) smallest = r;
        if (smallest != i) {
            swap(i, smallest);
            heapifyDown(smallest);
        }
    }

    // Build heap for `n` nodes; all positions initialised to -1
    void build(int n) {
        heap.clear();
        pos.assign(n, -1);
        size_ = 0;
    }

    void insert(double cost, int nodeId) {
        if (nodeId >= (int)pos.size()) pos.resize(nodeId + 1, -1);
        heap.push_back({cost, nodeId});
        pos[nodeId] = size_;
        size_++;
        heapifyUp(size_ - 1);
    }

    HeapNode extractMin() {
        if (isEmpty()) throw std::underflow_error("Heap is empty");
        HeapNode root = heap[0];
        pos[root.nodeId] = -1;
        size_--;
        if (size_ > 0) {
            heap[0] = heap[size_];
            heap.pop_back();
            pos[heap[0].nodeId] = 0;
            heapifyDown(0);
        } else {
            heap.pop_back();
        }
        return root;
    }

    void decreaseKey(int nodeId, double newCost) {
        if (nodeId >= (int)pos.size() || pos[nodeId] == -1)
            throw std::invalid_argument("Node not in heap");
        int i = pos[nodeId];
        heap[i].cost = newCost;
        heapifyUp(i);
    }

    bool contains(int nodeId) const {
        return nodeId < (int)pos.size() && pos[nodeId] != -1;
    }

    bool isEmpty() const { return size_ == 0; }
    int  getSize() const { return size_; }

    void displayHeap() const {
        printf("\n  [Min Heap Contents — size=%d]\n", size_);
        for (int i = 0; i < size_; i++)
            printf("    idx=%d  cost=%.2f  node=%d\n", i, heap[i].cost, heap[i].nodeId);
    }
};
