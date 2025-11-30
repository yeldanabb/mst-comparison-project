#include "graph_generator.hpp"
#include <random>
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <utility>

GraphGenerator::GraphGenerator(unsigned seed) : rng(seed) {}

Graph GraphGenerator::generateSparseGraph(int V, double averageDegree) {
    Graph graph(V, false);
    std::uniform_real_distribution<double> weightDist(1.0, 100.0);
    for (int i = 1; i < V; ++i) {
        std::uniform_int_distribution<int> parentDist(0, i-1);
        int u = parentDist(rng);
        graph.addEdge(u, i, weightDist(rng));
    }
    
    int targetEdges = static_cast<int>(V * averageDegree / 2);
    int currentEdges = V - 1;
    std::unordered_set<long long> existingEdges;
    for (const auto& edge : graph.getEdgeList()) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        existingEdges.insert((long long)std::min(u, v) * V + std::max(u, v));
    }
    
    std::uniform_int_distribution<int> vertexDist(0, V-1);
    int attempts = 0;
    int maxAttempts = V * V * 2; 
    
    while (currentEdges < targetEdges && attempts < maxAttempts) {
        int u = vertexDist(rng);
        int v = vertexDist(rng);
        if (u == v) continue;
        
        long long edgeKey = (long long)std::min(u, v) * V + std::max(u, v);
        if (existingEdges.find(edgeKey) == existingEdges.end()) {
            graph.addEdge(u, v, weightDist(rng));
            existingEdges.insert(edgeKey);
            currentEdges++;
        }
        attempts++;
    }
    
    return graph;
}

Graph GraphGenerator::generateDenseGraph(int V, double density) {
    Graph graph(V, false);
    std::uniform_real_distribution<double> weightDist(1.0, 100.0);
    for (int i = 1; i < V; ++i) {
        graph.addEdge(i-1, i, weightDist(rng));
    }
    
    int maxEdges = V * (V - 1) / 2;
    int targetEdges = static_cast<int>(density * maxEdges);

    std::vector<std::pair<int, int>> possibleEdges;
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            possibleEdges.push_back({i, j});
        }
    }
    
    std::shuffle(possibleEdges.begin(), possibleEdges.end(), rng);
    
    std::unordered_set<long long> existingEdges;
    for (const auto& edge : graph.getEdgeList()) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        existingEdges.insert((long long)std::min(u, v) * V + std::max(u, v));
    }
    
    int edgesAdded = V - 1;
    for (const auto& edge : possibleEdges) {
        if (edgesAdded >= targetEdges) break;
        int u = edge.first;
        int v = edge.second;
        long long edgeKey = (long long)u * V + v;
        
        if (existingEdges.find(edgeKey) == existingEdges.end()) {
            graph.addEdge(u, v, weightDist(rng));
            existingEdges.insert(edgeKey);
            edgesAdded++;
        }
    }
    return graph;
}

Graph GraphGenerator::generateCompleteGraph(int V) {
    Graph graph(V, false);
    std::uniform_real_distribution<double> weightDist(1.0, 100.0);
    
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            graph.addEdge(i, j, weightDist(rng));
        }
    }
    
    return graph;
}

Graph GraphGenerator::generateGridGraph(int rows, int cols) {
    int V = rows * cols;
    Graph graph(V, false);
    std::uniform_real_distribution<double> weightDist(1.0, 10.0);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int current = i * cols + j;
            if (j < cols - 1) {
                int right = i * cols + (j + 1);
                graph.addEdge(current, right, weightDist(rng));
            }
            if (i < rows - 1) {
                int down = (i + 1) * cols + j;
                graph.addEdge(current, down, weightDist(rng));
            }
        }
    }
    
    return graph;
}

Graph GraphGenerator::generateGraphWithParameters(int V, int E) {
    double density = static_cast<double>(2 * E) / (V * (V - 1));
    if (density < 0.3) {
        double averageDegree = static_cast<double>(2 * E) / V;
        return generateSparseGraph(V, averageDegree);
    } else {
        return generateDenseGraph(V, density);
    }
}