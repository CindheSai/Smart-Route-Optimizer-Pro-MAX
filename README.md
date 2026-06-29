<div align="center">

```
 ╔══════════════════════════════════════════════════════════════╗
 ║        SMART ROUTE OPTIMIZER  PRO MAX  v1.0                  ║
 ║     Advanced Graph-Based Navigation & Route Intelligence     ║
 ╚══════════════════════════════════════════════════════════════╝
```

# Smart Route Optimizer Pro MAX

**An enterprise-grade C++ navigation engine implementing 7 graph algorithms over a custom-built data structure stack — no external dependencies, no ML, no frameworks. Pure DSA.**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)
![Algorithms](https://img.shields.io/badge/Algorithms-7-orange?style=flat-square)
![Data Structures](https://img.shields.io/badge/Custom%20DS-6-purple?style=flat-square)

</div>

---

## Overview

Smart Route Optimizer Pro MAX models a real-world transportation network as a **dynamic weighted graph** and provides intelligent route optimization using advanced graph algorithms — entirely from scratch in C++.

Built as a high-impact portfolio project for:
- Software Engineer · Backend Developer · DSA Engineer · Competitive Programming

---

## Features

| Feature | Description |
|---|---|
| 🏙️ Dynamic City Builder | Add/remove/update locations and roads interactively |
| 🗺️ Smart Route Finder | Find shortest path using 4 different algorithms |
| 🔀 Alternative Routes | Generate 3 alternative paths between any two cities |
| 🚦 Traffic Simulation | LOW / MEDIUM / HIGH traffic with cost recalculation |
| 🚨 Emergency Routing | Ambulance, Police, Fire Rescue priority modes |
| 📊 Algorithm Benchmark | Side-by-side time & node-count comparison |
| 🖥️ ASCII Visualizer | Terminal map of the full transportation network |
| 📋 All-Pairs Table | Floyd-Warshall complete city distance matrix |
| 🌲 MST Generator | Kruskal minimum road network with DSU |

---

## Architecture

```
User Interface (CLI Dashboard)
         │
Route Management Engine
         │
   ┌─────┴──────┐
   │  Graph     │  ← Adjacency list, Hash table index
   └─────┬──────┘
         │
Algorithm Engine
   ├── BFS          (min-hop traversal)
   ├── DFS          (deep exploration, cycle detection)
   ├── Dijkstra     (custom MinHeap, shortest distance)
   ├── A*           (Haversine heuristic, fewest nodes explored)
   ├── Bellman-Ford (negative edge support)
   ├── Floyd-Warshall (all-pairs O(V³))
   └── Kruskal MST  (DisjointSet union-find)
         │
Custom Data Structures
   ├── MinHeap      (decrease-key, used by Dijkstra & A*)
   ├── Queue        (circular array, used by BFS)
   ├── Stack        (dynamic array, used by DFS)
   ├── HashTable    (chaining, used for name→id lookup)
   └── DisjointSet  (path compression + rank, used by Kruskal)
```

---

## Project Structure

```
Smart-Route-Optimizer-Pro-MAX/
├── src/
│   ├── main.cpp
│   ├── Graph/
│   │   ├── Graph.h
│   │   └── Graph.cpp
│   ├── DataStructures/
│   │   ├── MinHeap.h
│   │   ├── Queue.h
│   │   ├── Stack.h
│   │   ├── HashTable.h
│   │   └── DisjointSet.h
│   ├── Algorithms/
│   │   ├── BFS.h / BFS.cpp
│   │   ├── DFS.h / DFS.cpp
│   │   ├── Dijkstra.h / Dijkstra.cpp
│   │   ├── AStar.h / AStar.cpp
│   │   ├── BellmanFord.h / BellmanFord.cpp
│   │   ├── FloydWarshall.h / FloydWarshall.cpp
│   │   └── Kruskal.h / Kruskal.cpp
│   └── Utils/
│       └── Helper.h
├── Makefile
└── README.md
```

---

## Custom Data Structures

### 1. MinHeap
Binary min-heap with `decrease-key` support. Used by Dijkstra and A*.
- `insert(cost, nodeId)` · `extractMin()` · `decreaseKey(nodeId, cost)` · `heapifyDown()`
- Position map (`pos[]`) enables O(log V) decrease-key.

### 2. Queue (Circular Array)
Dynamic circular array queue for BFS. Auto-doubles capacity.
- `enqueue()` · `dequeue()` · `peek()` · `isEmpty()`

### 3. Stack (Dynamic Array)
Resizable stack for iterative DFS.
- `push()` · `pop()` · `peek()` · `isEmpty()`

### 4. HashTable (Chaining)
Open-chaining hash map for O(1) average name→id lookup.
- `insert()` · `search()` · `remove()` · auto-rehash at load > 0.7

### 5. DisjointSet (Union-Find)
Path compression + union by rank. Used by Kruskal's MST.
- `find(x)` · `unite(x, y)` · `connected(x, y)`

### 6. Graph (Adjacency List)
Core data model with dynamic node/edge management.
- `addLocation()` · `removeLocation()` · `addRoad()` · `removeRoad()` · `updateRoad()` · `setTraffic()`

---

## Algorithms

### BFS — Breadth-First Search
- **Use:** Graph traversal, minimum-hop path finding
- **Complexity:** O(V + E)
- **DS used:** Custom Queue

### DFS — Depth-First Search
- **Use:** Deep exploration, cycle detection, component counting
- **Complexity:** O(V + E)
- **DS used:** Custom Stack, three-color marking

### Dijkstra's Algorithm
- **Use:** Shortest path by distance / time / traffic-weight
- **Complexity:** O((V + E) log V)
- **DS used:** Custom MinHeap with decrease-key

### A* Search
- **Use:** Heuristic-guided shortest path (fewer nodes explored than Dijkstra)
- **Formula:** f(n) = g(n) + h(n) where h = Haversine distance
- **Complexity:** O((V + E) log V) — usually faster in practice
- **DS used:** Custom MinHeap

### Bellman-Ford
- **Use:** Shortest path with negative edge support + negative cycle detection
- **Complexity:** O(V × E)
- **Feature:** Detects negative cycles

### Floyd-Warshall
- **Use:** All-pairs shortest path matrix
- **Complexity:** O(V³)
- **Output:** Complete city-to-city distance table

### Kruskal's MST
- **Use:** Minimum spanning tree — minimum road infrastructure
- **Complexity:** O(E log E)
- **DS used:** DisjointSet (Union-Find)

---

## Complexity Table

| Algorithm | Time | Space | Use Case |
|---|---|---|---|
| BFS | O(V+E) | O(V) | Min hops, traversal |
| DFS | O(V+E) | O(V) | Connectivity, cycles |
| Dijkstra | O((V+E)logV) | O(V) | Shortest distance/time |
| A* | O((V+E)logV) | O(V) | Heuristic shortest path |
| Bellman-Ford | O(V·E) | O(V) | Negative weights |
| Floyd-Warshall | O(V³) | O(V²) | All-pairs distances |
| Kruskal MST | O(E·log E) | O(V) | Minimum road network |

---

## Demo Network — South India

12 cities pre-loaded with real approximate coordinates and road distances:

```
Hyderabad ──(275km)── Vijayawada ──(35km)── Guntur
    │                      │                   │
  (148km)              (350km)              (100km)
    │                      │                   │
 Warangal          Visakhapatnam            Ongole
    │                                          │
  (215km)                                   (105km)
    │                                          │
 Kurnool ──(130km)── Kadapa ──(140km)── Tirupati ──(145km)── Chennai
    │                    │                   │
  (330km)             (170km)            (115km)
    │                    │                   │
 Bangalore ──(346km)── Chennai ──(175km)── Nellore
```

---

## Installation & Compilation

### Requirements
- g++ 17 or higher
- Linux / macOS / WSL

### Build with Make
```bash
git clone https://github.com/yourusername/Smart-Route-Optimizer-Pro-MAX.git
cd Smart-Route-Optimizer-Pro-MAX
make
./SmartRouteOptimizer
```

### Manual Compilation
```bash
g++ -std=c++17 -O2 \
  src/main.cpp \
  src/Graph/Graph.cpp \
  src/Algorithms/BFS.cpp \
  src/Algorithms/DFS.cpp \
  src/Algorithms/Dijkstra.cpp \
  src/Algorithms/AStar.cpp \
  src/Algorithms/BellmanFord.cpp \
  src/Algorithms/FloydWarshall.cpp \
  src/Algorithms/Kruskal.cpp \
  -lm -o SmartRouteOptimizer
./SmartRouteOptimizer
```

### Debug Build
```bash
make debug
```

---

## Usage

```
1. Choose option 1 to load the South India demo network
2. Choose option 7 to display the full network
3. Choose option 11 (Dijkstra) to find the shortest route
   → Enter: 0 (Hyderabad) → 2 (Chennai) → mode 1 (Distance)
4. Choose option 19 (Benchmark) to compare all algorithms
5. Choose option 14 (Floyd-Warshall) → option 1 for distance table
```

### Example Session

```
Choice: 1
[✓] Demo city loaded: South India Transportation Network
[✓] 12 locations, 18 roads ready.

Choice: 11   (Dijkstra)
Source: 0    (Hyderabad)
Destination: 2  (Chennai)
Mode: 1  (Distance)

  ╔══════════════════════════════════════════════════════╗
  ║              DIJKSTRA SHORTEST PATH                 ║
  ║  Total Distance : 630 km                            ║
  ║  Total Time     : 10.2 hrs                          ║
  ╚══════════════════════════════════════════════════════╝

  ◉ Hyderabad  → 215 km
  ◉ Kurnool    → 130 km
  ◉ Kadapa     → 140 km
  ◉ Tirupati   → 145 km
  ◉ Chennai
```

---

## Future Improvements

- [ ] Bidirectional Dijkstra for even faster pathfinding
- [ ] Contraction Hierarchies (highway node pre-processing)
- [ ] Johnson's Algorithm (all-pairs with negative edges)
- [ ] Real-time traffic feed integration
- [ ] File I/O to save/load city networks
- [ ] Multi-modal routing (road + rail + air)
- [ ] Turn restrictions and one-way road support

---

## License

MIT License — free to use, modify, and distribute.

---

<div align="center">

**Built with pure C++17 · Zero external dependencies · Full OOP · Production-quality DSA**

</div>
