#pragma once
#include <string>
#include <vector>
#include "../DataStructures/HashTable.h"

// ============================================================
// Graph.h — Core data model
// Location (Node)  +  Road (Edge)  +  Graph (adjacency list)
// ============================================================

// ---- Enums ------------------------------------------------

enum class LocationCategory { CITY, TOWN, VILLAGE, JUNCTION, AIRPORT, OTHER };
enum class RoadStatus       { OPEN, CLOSED, UNDER_CONSTRUCTION };
enum class TrafficLevel     { LOW, MEDIUM, HIGH };

std::string categoryStr(LocationCategory c);
std::string statusStr(RoadStatus s);
std::string trafficStr(TrafficLevel t);

// ---- Location (Node) --------------------------------------

struct Location {
    int              id;
    std::string      name;
    double           lat, lng;      // approximate coordinates
    LocationCategory category;
    bool             active;

    Location() : id(-1), lat(0), lng(0),
                 category(LocationCategory::CITY), active(false) {}
    Location(int id, const std::string& name,
             double lat, double lng,
             LocationCategory cat = LocationCategory::CITY)
        : id(id), name(name), lat(lat), lng(lng), category(cat), active(true) {}
};

// ---- Road (Edge) ------------------------------------------

struct Road {
    int         src, dst;
    double      distance;       // km
    double      travelTime;     // hours
    double      trafficWeight;  // multiplier applied to cost
    TrafficLevel traffic;
    RoadStatus   status;
    bool         bidirectional;

    Road() : src(-1), dst(-1), distance(0), travelTime(0),
             trafficWeight(1.0), traffic(TrafficLevel::LOW),
             status(RoadStatus::OPEN), bidirectional(true) {}

    Road(int s, int d, double dist, double time,
         TrafficLevel tf = TrafficLevel::LOW,
         bool bidir = true)
        : src(s), dst(d), distance(dist), travelTime(time),
          traffic(tf), status(RoadStatus::OPEN), bidirectional(bidir)
    {
        updateTrafficWeight();
    }

    void updateTrafficWeight() {
        switch (traffic) {
            case TrafficLevel::LOW:    trafficWeight = 1.0; break;
            case TrafficLevel::MEDIUM: trafficWeight = 1.8; break;
            case TrafficLevel::HIGH:   trafficWeight = 3.0; break;
        }
    }

    double effectiveCost()     const { return distance * trafficWeight; }
    double effectiveTimeCost() const { return travelTime * trafficWeight; }
};

// ---- Adjacency list entry ---------------------------------

struct AdjEntry {
    int    dst;
    Road   road;
    AdjEntry(int d, const Road& r) : dst(d), road(r) {}
};

// ---- Graph ------------------------------------------------

class Graph {
private:
    std::vector<Location>           nodes;     // indexed by nodeId
    std::vector<std::vector<AdjEntry>> adj;    // adjacency list
    HashTable                       nameIndex; // name → id
    int                             nextId;
    int                             nodeCount;
    int                             edgeCount;

public:
    Graph();

    // --- Builders ---
    int  addLocation(const std::string& name, double lat, double lng,
                     LocationCategory cat = LocationCategory::CITY);
    bool removeLocation(int id);
    bool addRoad(int src, int dst, double distance, double travelTime,
                 TrafficLevel tf = TrafficLevel::LOW, bool bidir = true);
    bool removeRoad(int src, int dst);
    bool updateRoad(int src, int dst, double distance, double travelTime,
                    TrafficLevel tf);
    bool setTraffic(int src, int dst, TrafficLevel tf);
    bool setRoadStatus(int src, int dst, RoadStatus status);

    // --- Accessors ---
    const Location&                        getLocation(int id)  const;
    Location&                              getLocationMut(int id);
    const std::vector<AdjEntry>&           getAdj(int id)        const;
    int                                    searchLocation(const std::string& name) const;
    int                                    getNodeCount()         const { return nodeCount; }
    int                                    getEdgeCount()         const { return edgeCount; }
    int                                    getNextId()            const { return nextId; }
    bool                                   isValid(int id)        const;
    const std::vector<Location>&           getAllNodes()          const { return nodes; }

    // --- Display ---
    void displayGraph()    const;
    void displayNode(int id) const;

    // --- Prebuilt demo city ---
    void loadDemoCity();

    // Euclidean heuristic distance between two nodes (for A*)
    double heuristic(int a, int b) const;
};
