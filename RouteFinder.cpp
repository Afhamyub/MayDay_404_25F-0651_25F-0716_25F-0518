// ============================================================================
// RouteFinder.cpp
// Module 8 : Fastest Intercept Route
// ============================================================================
#include "RouteFinder.h"
#include <iostream>

using namespace std;

// A distance larger than any possible route cost stands for infinity.
static const int INFINITE_TICKS = 1000000;

// ===========================================================================
//                              DistanceHeap
// ===========================================================================
DistanceHeap::DistanceHeap(int size)
{
    capacity = (size > 0) ? size * size + size : 8;
    data = new HeapEntry[capacity];
    count = 0;
}

DistanceHeap::~DistanceHeap()
{
    delete[] data;
}

void DistanceHeap::swapEntries(int i, int j)
{
    HeapEntry temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}

void DistanceHeap::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (data[index].distance < data[parent].distance)
        {
            swapEntries(index, parent);
            index = parent;
        }
        else
        {
            return;
        }
    }
}

void DistanceHeap::heapifyDown(int index)
{
    while (true)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int best = index;
        if (left < count && data[left].distance < data[best].distance)
        {
            best = left;
        }
        if (right < count && data[right].distance < data[best].distance)
        {
            best = right;
        }
        if (best == index)
        {
            return;
        }
        swapEntries(index, best);
        index = best;
    }
}

void DistanceHeap::insert(int sector, int distance)
{
    if (count == capacity)
    {
        return;
    }
    data[count].sector = sector;
    data[count].distance = distance;
    count++;
    heapifyUp(count - 1);
}

HeapEntry DistanceHeap::extractMin()
{
    HeapEntry best;
    best.sector = -1;
    best.distance = INFINITE_TICKS;
    if (count == 0)
    {
        return best;
    }
    best = data[0];
    data[0] = data[count - 1];
    count--;
    if (count > 0)
    {
        heapifyDown(0);
    }
    return best;
}

bool DistanceHeap::isEmpty() const
{
    return count == 0;
}

// ===========================================================================
//                              RouteFinder
// ===========================================================================
RouteFinder::RouteFinder(SpaceGraph* airspace)
{
    graph = airspace;
}

// Dijkstra with a custom min-heap, adjacency matrix weight lookup and full
// parent-pointer tracing.
Route RouteFinder::findFastestRoute(const string& sourceSector,
                                    const string& destinationRunway)
{
    Route route;
    route.sectors = 0;
    route.length = 0;
    route.totalCost = 0;
    route.found = false;

    int source = graph->findSector(sourceSector);
    int target = graph->findSector(destinationRunway);
    if (source < 0)
    {
        cout << "Error: Unknown source sector : " << sourceSector << endl;
        return route;
    }
    if (target < 0)
    {
        cout << "Error: Unknown destination : " << destinationRunway
             << endl;
        return route;
    }

    int total = graph->getSectorCount();
    int* distance = new int[total];
    int* parent = new int[total];
    bool* settled = new bool[total];
    for (int i = 0; i < total; i++)
    {
        distance[i] = INFINITE_TICKS;
        parent[i] = -1;
        settled[i] = false;
    }
    distance[source] = 0;

    DistanceHeap heap(total);
    heap.insert(source, 0);

    while (!heap.isEmpty())
    {
        HeapEntry entry = heap.extractMin();
        int sector = entry.sector;
        if (sector < 0 || settled[sector])
        {
            continue;                 // stale heap entry, skip it
        }
        settled[sector] = true;
        if (sector == target)
        {
            break;
        }

        AdjNode* walk = graph->getNeighbours(sector);
        while (walk != 0)
        {
            int weight = graph->getWeight(sector, walk->to);   // matrix lookup
            if (weight > 0 && !settled[walk->to] &&
                distance[sector] + weight < distance[walk->to])
            {
                distance[walk->to] = distance[sector] + weight;
                parent[walk->to] = sector;
                heap.insert(walk->to, distance[walk->to]);
            }
            walk = walk->next;
        }
    }

    if (distance[target] < INFINITE_TICKS)
    {
        // Trace the parent pointers back from the destination.
        int* reverse = new int[total];
        int length = 0;
        int walk = target;
        while (walk != -1)
        {
            reverse[length] = walk;
            length++;
            walk = parent[walk];
        }
        route.sectors = new int[length];
        for (int i = 0; i < length; i++)
        {
            route.sectors[i] = reverse[length - 1 - i];
        }
        route.length = length;
        route.totalCost = distance[target];
        route.found = true;
        delete[] reverse;
    }

    delete[] distance;
    delete[] parent;
    delete[] settled;
    return route;
}

// Print the full sector sequence and the total travel time.
void RouteFinder::printRoute(const Route& route) const
{
    if (!route.found)
    {
        cout << "   Error: No corridor sequence exists for that request."
             << endl;
        return;
    }
    cout << "   Route : ";
    for (int i = 0; i < route.length; i++)
    {
        cout << graph->getSectorName(route.sectors[i]);
        if (i < route.length - 1)
        {
            cout << " -> ";
        }
    }
    cout << endl;
    cout << "   Hops : " << (route.length - 1) << " | Total travel time : "
         << route.totalCost << " tick(s)" << endl;
}

void RouteFinder::releaseRoute(Route& route)
{
    delete[] route.sectors;
    route.sectors = 0;
    route.length = 0;
    route.totalCost = 0;
    route.found = false;
}
