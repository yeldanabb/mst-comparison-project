#include "kruskal.hpp"
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include <algorithm>
#include <iostream>

bool Kruskal::compareEdges(const std::tuple<int, int, double>& a, 
                          const std::tuple<int, int, double>& b) {
    return std::get<2>(a) < std::get<2>(b);
}

MSTResult Kruskal::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    
    Timer timer;
    timer.start();

    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    int V = graph.getVertices();
    const auto& edges = graph.getEdgeList();
    
    std::vector<std::tuple<int, int, double>> sortedEdges = edges;
    std::sort(sortedEdges.begin(), sortedEdges.end(), compareEdges);
    UnionFind uf(V);
    result.totalWeight = 0.0;
    for (const auto& edge : sortedEdges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        if (!uf.connected(u, v)) {
            uf.unite(u, v);
            result.edges.push_back(edge);
            result.totalWeight += weight;
            if (result.edges.size() == static_cast<size_t>(V - 1)) {
                break;
            }
        }
    }
    
    timer.stop();
    result.executionTime = timer.elapsedMilliseconds();
    result.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() - initialMemory;
    return result;
}