#include "graph.hpp"
#include <random>
#include <algorithm>
#include <queue>
#include <iostream>

Graph::Graph(int vertices, bool isDirected) : V(vertices), directed(isDirected) {
    adjList.resize(V);
}

void Graph::addEdge(int u, int v, double weight) {
    if (u < 0 || u >= V || v < 0 || v >= V) {
        throw std::out_of_range("Vertex index out of bounds");
    }
    adjList[u].push_back({v, weight});
    edgeList.push_back({u, v, weight});
    if (!directed) {
        adjList[v].push_back({u, weight});
    }
}

Graph Graph::generateRandomGraph(int V, double density, double minWeight, double maxWeight) {
    Graph graph(V, false); 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> weightDist(minWeight, maxWeight);
    std::uniform_int_distribution<int> vertexDist(0, V-1);
    int maxEdges = V * (V - 1) / 2;
    int targetEdges = static_cast<int>(density * maxEdges);
    for (int i = 1; i < V; ++i) {
        int u = vertexDist(gen) % i;
        double weight = weightDist(gen);
        graph.addEdge(u, i, weight);
    }
    
    int currentEdges = V - 1;
    while (currentEdges < targetEdges) {
        int u = vertexDist(gen);
        int v = vertexDist(gen);
        if (u != v) {
            bool exists = false;
            for (const auto& neighbor : graph.adjList[u]) {
                if (neighbor.first == v) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                double weight = weightDist(gen);
                graph.addEdge(u, v, weight);
                currentEdges++;
            }
        }
    }
    return graph;
}

bool Graph::isConnected() const {
    if (V == 0) return true;
    
    std::vector<bool> visited(V, false);
    std::queue<int> q;
    int visitedCount = 0;
    q.push(0);
    visited[0] = true;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        visitedCount++;
        for (const auto& neighbor : adjList[u]) {
            int v = neighbor.first;
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    
    return visitedCount == V;
}