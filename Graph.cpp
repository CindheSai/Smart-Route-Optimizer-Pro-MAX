#include "Graph.h"
#include <cstdio>
#include <cmath>
#include <stdexcept>

// ============================================================
// Helper string converters
// ============================================================

std::string categoryStr(LocationCategory c) {
    switch (c) {
        case LocationCategory::CITY:     return "City";
        case LocationCategory::TOWN:     return "Town";
        case LocationCategory::VILLAGE:  return "Village";
        case LocationCategory::JUNCTION: return "Junction";
        case LocationCategory::AIRPORT:  return "Airport";
        default:                         return "Other";
    }
}

std::string statusStr(RoadStatus s) {
    switch (s) {
        case RoadStatus::OPEN:               return "Open";
        case RoadStatus::CLOSED:             return "Closed";
        case RoadStatus::UNDER_CONSTRUCTION: return "Under Construction";
        default:                             return "Unknown";
    }
}

std::string trafficStr(TrafficLevel t) {
    switch (t) {
        case TrafficLevel::LOW:    return "LOW";
        case TrafficLevel::MEDIUM: return "MEDIUM";
        case TrafficLevel::HIGH:   return "HIGH";
        default:                   return "?";
    }
}

// ============================================================
// Graph constructor
// ============================================================

Graph::Graph() : nextId(0), nodeCount(0), edgeCount(0) {}

// ============================================================
// addLocation
// ============================================================

int Graph::addLocation(const std::string& name, double lat, double lng,
                       LocationCategory cat) {
    if (nameIndex.search(name) != -1) {
        printf("  [!] Location '%s' already exists (id=%d).\n",
               name.c_str(), nameIndex.search(name));
        return -1;
    }
    int id = nextId++;
    if (id >= (int)nodes.size()) {
        nodes.resize(id + 1);
        adj.resize(id + 1);
    }
    nodes[id] = Location(id, name, lat, lng, cat);
    adj[id].clear();
    nameIndex.insert(name, id);
    nodeCount++;
    return id;
}

// ============================================================
// removeLocation
// ============================================================

bool Graph::removeLocation(int id) {
    if (!isValid(id)) return false;
    // Remove all edges involving this node
    for (int i = 0; i < (int)adj.size(); i++) {
        if (!nodes[i].active) continue;
        auto& list = adj[i];
        for (int j = (int)list.size() - 1; j >= 0; j--) {
            if (list[j].dst == id) {
                list.erase(list.begin() + j);
                edgeCount--;
            }
        }
    }
    adj[id].clear();
    nameIndex.remove(nodes[id].name);
    nodes[id].active = false;
    nodeCount--;
    return true;
}

// ============================================================
// addRoad
// ============================================================

bool Graph::addRoad(int src, int dst, double distance, double travelTime,
                    TrafficLevel tf, bool bidir) {
    if (!isValid(src) || !isValid(dst)) {
        printf("  [!] Invalid node id(s): %d -> %d\n", src, dst);
        return false;
    }
    Road road(src, dst, distance, travelTime, tf, bidir);
    // Check for duplicate
    for (auto& e : adj[src])
        if (e.dst == dst) { printf("  [!] Road %d->%d already exists.\n", src, dst); return false; }

    adj[src].push_back(AdjEntry(dst, road));
    edgeCount++;
    if (bidir) {
        Road rev(dst, src, distance, travelTime, tf, bidir);
        adj[dst].push_back(AdjEntry(src, rev));
        edgeCount++;
    }
    return true;
}

// ============================================================
// removeRoad
// ============================================================

bool Graph::removeRoad(int src, int dst) {
    if (!isValid(src) || !isValid(dst)) return false;
    bool found = false;
    auto& list = adj[src];
    for (int i = (int)list.size() - 1; i >= 0; i--) {
        if (list[i].dst == dst) { list.erase(list.begin() + i); edgeCount--; found = true; }
    }
    // Remove reverse if bidirectional
    auto& rlist = adj[dst];
    for (int i = (int)rlist.size() - 1; i >= 0; i--) {
        if (rlist[i].dst == src) { rlist.erase(rlist.begin() + i); edgeCount--; }
    }
    return found;
}

// ============================================================
// updateRoad
// ============================================================

bool Graph::updateRoad(int src, int dst, double distance, double travelTime,
                       TrafficLevel tf) {
    for (auto& e : adj[src]) {
        if (e.dst == dst) {
            e.road.distance   = distance;
            e.road.travelTime = travelTime;
            e.road.traffic    = tf;
            e.road.updateTrafficWeight();
            // Mirror update for reverse edge
            for (auto& re : adj[dst]) {
                if (re.dst == src) {
                    re.road.distance   = distance;
                    re.road.travelTime = travelTime;
                    re.road.traffic    = tf;
                    re.road.updateTrafficWeight();
                }
            }
            return true;
        }
    }
    return false;
}

// ============================================================
// setTraffic
// ============================================================

bool Graph::setTraffic(int src, int dst, TrafficLevel tf) {
    for (auto& e : adj[src]) {
        if (e.dst == dst) {
            e.road.traffic = tf;
            e.road.updateTrafficWeight();
            for (auto& re : adj[dst])
                if (re.dst == src) { re.road.traffic = tf; re.road.updateTrafficWeight(); }
            return true;
        }
    }
    return false;
}

bool Graph::setRoadStatus(int src, int dst, RoadStatus status) {
    for (auto& e : adj[src]) {
        if (e.dst == dst) {
            e.road.status = status;
            for (auto& re : adj[dst])
                if (re.dst == src) re.road.status = status;
            return true;
        }
    }
    return false;
}

// ============================================================
// Accessors
// ============================================================

const Location& Graph::getLocation(int id) const {
    if (id < 0 || id >= (int)nodes.size()) throw std::out_of_range("Invalid node id");
    return nodes[id];
}

Location& Graph::getLocationMut(int id) {
    if (id < 0 || id >= (int)nodes.size()) throw std::out_of_range("Invalid node id");
    return nodes[id];
}

const std::vector<AdjEntry>& Graph::getAdj(int id) const {
    if (id < 0 || id >= (int)adj.size()) throw std::out_of_range("Invalid node id");
    return adj[id];
}

int Graph::searchLocation(const std::string& name) const {
    return nameIndex.search(name);
}

bool Graph::isValid(int id) const {
    return id >= 0 && id < (int)nodes.size() && nodes[id].active;
}

// ============================================================
// heuristic — straight-line distance (km) via lat/lng
// ============================================================

double Graph::heuristic(int a, int b) const {
    const double R = 6371.0;
    double lat1 = nodes[a].lat * M_PI / 180.0;
    double lat2 = nodes[b].lat * M_PI / 180.0;
    double dlat = (nodes[b].lat - nodes[a].lat) * M_PI / 180.0;
    double dlng = (nodes[b].lng - nodes[a].lng) * M_PI / 180.0;
    double h = sin(dlat/2)*sin(dlat/2) +
               cos(lat1)*cos(lat2)*sin(dlng/2)*sin(dlng/2);
    return R * 2.0 * atan2(sqrt(h), sqrt(1-h));
}

// ============================================================
// displayGraph
// ============================================================

void Graph::displayGraph() const {
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════╗\n");
    printf("  ║           TRANSPORTATION NETWORK MAP                 ║\n");
    printf("  ╚══════════════════════════════════════════════════════╝\n");
    printf("  Locations: %d   Roads: %d\n\n", nodeCount, edgeCount / 2);

    for (int i = 0; i < (int)nodes.size(); i++) {
        if (!nodes[i].active) continue;
        printf("  [%2d] %-20s  (%.2f, %.2f)  %s\n",
               nodes[i].id, nodes[i].name.c_str(),
               nodes[i].lat, nodes[i].lng,
               categoryStr(nodes[i].category).c_str());
        for (const auto& e : adj[i]) {
            if (!nodes[e.dst].active) continue;
            printf("         ──► %-20s  %.0f km  %.1f hr  Traffic:%-7s  %s\n",
                   nodes[e.dst].name.c_str(),
                   e.road.distance,
                   e.road.travelTime,
                   trafficStr(e.road.traffic).c_str(),
                   statusStr(e.road.status).c_str());
        }
    }
    printf("\n");
}

void Graph::displayNode(int id) const {
    if (!isValid(id)) { printf("  [!] Node %d not found.\n", id); return; }
    const Location& loc = nodes[id];
    printf("  ID: %d | Name: %s | Coords: (%.2f, %.2f) | Cat: %s\n",
           loc.id, loc.name.c_str(), loc.lat, loc.lng,
           categoryStr(loc.category).c_str());
}

// ============================================================
// loadDemoCity — South India transportation network
// ============================================================

void Graph::loadDemoCity() {
    // Add locations (id assigned automatically 0..N-1)
    addLocation("Hyderabad",   17.38, 78.47, LocationCategory::CITY);    // 0
    addLocation("Bangalore",   12.97, 77.59, LocationCategory::CITY);    // 1
    addLocation("Chennai",     13.08, 80.27, LocationCategory::CITY);    // 2
    addLocation("Vijayawada",  16.51, 80.62, LocationCategory::CITY);    // 3
    addLocation("Visakhapatnam", 17.69, 83.22, LocationCategory::CITY);  // 4
    addLocation("Tirupati",    13.63, 79.42, LocationCategory::CITY);    // 5
    addLocation("Warangal",    18.00, 79.58, LocationCategory::CITY);    // 6
    addLocation("Nellore",     14.44, 79.99, LocationCategory::TOWN);    // 7
    addLocation("Kurnool",     15.83, 78.04, LocationCategory::TOWN);    // 8
    addLocation("Guntur",      16.30, 80.45, LocationCategory::CITY);    // 9
    addLocation("Ongole",      15.50, 80.05, LocationCategory::TOWN);    // 10
    addLocation("Kadapa",      14.47, 78.82, LocationCategory::TOWN);    // 11

    // Add roads (src, dst, distance km, time hr, traffic)
    addRoad(0, 3, 275, 4.0, TrafficLevel::MEDIUM);   // Hyd - Vijayawada
    addRoad(0, 8, 215, 3.5, TrafficLevel::LOW);       // Hyd - Kurnool
    addRoad(0, 6, 148, 2.5, TrafficLevel::LOW);       // Hyd - Warangal
    addRoad(0, 1, 570, 8.0, TrafficLevel::HIGH);      // Hyd - Bangalore
    addRoad(1, 5, 155, 2.5, TrafficLevel::LOW);       // Bangalore - Tirupati
    addRoad(1, 2, 346, 5.0, TrafficLevel::MEDIUM);    // Bangalore - Chennai
    addRoad(2, 5, 145, 2.5, TrafficLevel::LOW);       // Chennai - Tirupati
    addRoad(2, 7, 175, 2.8, TrafficLevel::LOW);       // Chennai - Nellore
    addRoad(3, 9, 35,  0.5, TrafficLevel::LOW);       // Vijayawada - Guntur
    addRoad(3, 4, 350, 5.0, TrafficLevel::LOW);       // Vijayawada - Visakhapatnam
    addRoad(4, 6, 340, 5.0, TrafficLevel::LOW);       // Visakhapatnam - Warangal
    addRoad(5, 7, 115, 1.8, TrafficLevel::LOW);       // Tirupati - Nellore
    addRoad(5, 11, 140, 2.2, TrafficLevel::LOW);      // Tirupati - Kadapa
    addRoad(7, 10, 105, 1.7, TrafficLevel::LOW);      // Nellore - Ongole
    addRoad(8, 11, 130, 2.0, TrafficLevel::LOW);      // Kurnool - Kadapa
    addRoad(8, 1, 330, 5.0, TrafficLevel::MEDIUM);    // Kurnool - Bangalore
    addRoad(9, 10, 100, 1.5, TrafficLevel::LOW);      // Guntur - Ongole
    addRoad(10, 7, 105, 1.7, TrafficLevel::LOW);      // Ongole - Nellore (dup handled)
    addRoad(11, 9, 170, 2.8, TrafficLevel::LOW);      // Kadapa - Guntur
}
