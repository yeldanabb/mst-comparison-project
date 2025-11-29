#include "kkt.hpp"
#include "../algorithms/kruskal.hpp"  
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>  

MSTResult KKT::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    
    Timer timer;
    timer.start();
    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    
    int V = graph.getVertices();
    UnionFind uf(V);
    
    std::random_device rd;
    std::mt19937 rng(rd());
    
    result.edges = kktRecursive(graph, uf, rng);
    result.totalWeight = 0.0;
    for (const auto& edge : result.edges) {
        result.totalWeight += std::get<2>(edge);
    }
    
    timer.stop();
    result.executionTime = timer.elapsedMilliseconds();
    result.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() - initialMemory;
    return result;
}

std::vector<std::tuple<int, int, double>> KKT::kktRecursive(
    const Graph& graph, 
    UnionFind& uf,
    std::mt19937& rng) {
    
    int V = graph.getVertices();
    int E = graph.getEdges();
    if (V <= 10 || E <= V*2) {
        Kruskal kruskal;
        MSTResult result = kruskal.solve(graph);
        return result.edges;
    }
    
    std::vector<std::tuple<int, int, double>> mstEdges;

    boruvkaStep(graph, uf, mstEdges);
    
    if (uf.getComponents() == 1) {
        return mstEdges;
    }
    
    Graph sampledGraph = sampleGraph(graph, 0.5, rng);
    
    UnionFind sampledUF(V);
    auto sampledMST = kktRecursive(sampledGraph, sampledUF, rng);
    
    std::vector<std::tuple<int, int, double>> remainingEdges;
    filterEdges(graph, sampledMST, remainingEdges);
    Graph remainingGraph(V, false);
    for (const auto& edge : remainingEdges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        remainingGraph.addEdge(u, v, weight);
    }
    
    UnionFind remainingUF(V);
    auto remainingMST = kktRecursive(remainingGraph, remainingUF, rng);
    mstEdges.insert(mstEdges.end(), remainingMST.begin(), remainingMST.end());
    return mstEdges;
}

void KKT::boruvkaStep(
    const Graph& graph, 
    UnionFind& uf,
    std::vector<std::tuple<int, int, double>>& mstEdges) {
    
    int V = graph.getVertices();
    const auto& edges = graph.getEdgeList();
    bool changed = true;
    int iterations = 0;
    const int maxIterations = 10; 
    
    while (changed && iterations < maxIterations && uf.getComponents() > 1) {
        changed = false;
        iterations++;
        std::vector<std::tuple<int, int, double>> cheapest(V, {-1, -1, std::numeric_limits<double>::max()});
        for (const auto& edge : edges) {
            int u = std::get<0>(edge);
            int v = std::get<1>(edge);
            double weight = std::get<2>(edge);
            int compU = uf.find(u);
            int compV = uf.find(v);
            if (compU != compV) {
                if (weight < std::get<2>(cheapest[compU])) {
                    cheapest[compU] = {u, v, weight};
                }
                if (weight < std::get<2>(cheapest[compV])) {
                    cheapest[compV] = {u, v, weight};
                }
            }
        }
        
        for (int comp = 0; comp < V; comp++) {
            if (std::get<0>(cheapest[comp]) != -1) {
                int u = std::get<0>(cheapest[comp]);
                int v = std::get<1>(cheapest[comp]);
                double weight = std::get<2>(cheapest[comp]);
                if (!uf.connected(u, v)) {
                    uf.unite(u, v);
                    mstEdges.push_back({u, v, weight});
                    changed = true;
                }
            }
        }
    }
}

Graph KKT::sampleGraph(const Graph& graph, double probability, std::mt19937& rng) {
    int V = graph.getVertices();
    Graph sampledGraph(V, false);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    const auto& edges = graph.getEdgeList();
    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        if (dist(rng) < probability) {
            sampledGraph.addEdge(u, v, weight);
        }
    }
    return sampledGraph;
}

void KKT::filterEdges(
    const Graph& graph,
    const std::vector<std::tuple<int, int, double>>& sampledMST,
    std::vector<std::tuple<int, int, double>>& remainingEdges) {
    int V = graph.getVertices();
    UnionFind sampledUF(V);
    for (const auto& edge : sampledMST) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        sampledUF.unite(u, v);
    }
    
    const auto& edges = graph.getEdgeList();
    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        if (!sampledUF.connected(u, v)) {
            remainingEdges.push_back(edge);
        }
    }
}

Graph KKT::contractGraph(const Graph& graph, const UnionFind& uf) {
    int V = graph.getVertices();
    UnionFind ufCopy = uf;
    std::unordered_map<int, int> componentMap;
    int newV = 0;
    for (int i = 0; i < V; i++) {
        int comp = ufCopy.find(i);
        if (componentMap.find(comp) == componentMap.end()) {
            componentMap[comp] = newV++;
        }
    }
    
    Graph contractedGraph(newV, false);
    const auto& edges = graph.getEdgeList();
    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        int compU = ufCopy.find(u);
        int compV = ufCopy.find(v);
        if (compU != compV) {
            int newU = componentMap[compU];
            int newV = componentMap[compV];
            contractedGraph.addEdge(newU, newV, weight);
        }
    }
    
    return contractedGraph;
}

