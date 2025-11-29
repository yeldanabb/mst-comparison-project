#include "prim.hpp"
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include <queue>
#include <vector>
#include <functional>
#include <limits>
#include <iostream>

MSTResult Prim::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    Timer timer;
    timer.start();
    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    int V = graph.getVertices();
    const auto& adjList = graph.getAdjList();
    std::vector<bool> inMST(V, false);
    std::vector<double> key(V, std::numeric_limits<double>::max());
    std::vector<int> parent(V, -1);
    std::priority_queue<std::pair<double, int>, 
                       std::vector<std::pair<double, int>>,
                       std::greater<std::pair<double, int>>> pq;
    
    key[0] = 0.0;
    pq.push({0.0, 0});
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;
        if (parent[u] != -1) {
            result.edges.push_back({parent[u], u, key[u]});
            result.totalWeight += key[u];
        }
        
        for (const auto& neighbor : adjList[u]) {
            int v = neighbor.first;
            double weight = neighbor.second;
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }
    
    timer.stop();
    result.executionTime = timer.elapsedMilliseconds();
    result.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() - initialMemory;
    return result;
}