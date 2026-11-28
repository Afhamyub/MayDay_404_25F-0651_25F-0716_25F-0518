// ============================================================================
// RouteFinder.h
// Module 8 : Fastest Intercept Route  (Phase 3 - Graphs & Algorithms)
//
// Dijkstra's algorithm written from scratch. The priority selection uses a
// custom min-heap keyed by the shortest distance found so far, the edge weights
// are read from the adjacency matrix, and the complete sector sequence is
// rebuilt from parent pointers.
// ============================================================================
#ifndef ROUTEFINDER_H
#define ROUTEFINDER_H

#include "SpaceGraph.h"

// A computed route: the sector sequence plus its total travel time in ticks.
struct Route
{
    int *sectors;  // sector indices from source to destination
    int length;    // number of sectors in the sequence
    int totalCost; // total travel time in ticks
    bool found;    // false when the destination is unreachable
};

// One entry of the custom min-heap used inside Dijkstra.
struct HeapEntry
{
    int sector;
    int distance;
};

// Custom array based min-heap keyed by distance-so-far.
class DistanceHeap
{
private:
    HeapEntry *data;
    int capacity;
    int count;

    void swapEntries(int i, int j);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    DistanceHeap(int size);
    ~DistanceHeap();
    void insert(int sector, int distance);
    HeapEntry extractMin();
    bool isEmpty() const;
};

class RouteFinder
{
private:
    SpaceGraph *graph;

public:
    RouteFinder(SpaceGraph *airspace);

    Route findFastestRoute(const std::string &sourceSector,
                           const std::string &destinationRunway);
    void printRoute(const Route &route) const;
    static void releaseRoute(Route &route); // free the sector array
};

#endif
