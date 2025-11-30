#include "verifier.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <functional>

std::unordered_set<int> MSTVerifier::findHeavyEdges(
    const std::vector<std::tuple<int, int, double, int>>& graph,
    const std::vector<std::tuple<int, int, double, int>>& forest,
    int numVertices) {
    
    std::unordered_set<int> heavy_edges;
    
    if (forest.empty()) {
        return heavy_edges;
    }
    
    std::vector<std::vector<std::pair<int, double>>> adj(numVertices);
    buildTreeAdjacency(forest, numVertices, adj);
    
    for (const auto& edge : graph) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        int id = std::get<3>(edge);
        if (u == v || u >= numVertices || v >= numVertices || u < 0 || v < 0) {
            continue;
        }
        if (!areConnectedInForest(u, v, adj)) {
            continue;
        }
        std::vector<double> max_weight(numVertices, 0.0);
        std::vector<bool> visited(numVertices, false);
        std::queue<int> q;
        
        q.push(u);
        visited[u] = true;
        max_weight[u] = 0.0;
        
        bool found = false;
        while (!q.empty() && !found) {
            int current = q.front();
            q.pop();
            
            for (const auto& neighbor : adj[current]) {
                int next = neighbor.first;
                double edge_weight = neighbor.second;
                
                if (!visited[next]) {
                    visited[next] = true;
                    max_weight[next] = std::max(max_weight[current], edge_weight);
                    
                    if (next == v) {
                        found = true;
                        break;
                    }
                    q.push(next);
                }
            }
        }
        
        if (found && weight > max_weight[v] + EPSILON) {
            heavy_edges.insert(id);
        }
    }
    
    return heavy_edges;
}

void MSTVerifier::buildTreeAdjacency(
    const std::vector<std::tuple<int, int, double, int>>& forest,
    int numVertices,
    std::vector<std::vector<std::pair<int, double>>>& adj) {
    
    adj.clear();
    adj.resize(numVertices);
    
    for (const auto& edge : forest) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices && u != v) {
            adj[u].emplace_back(v, weight);
            adj[v].emplace_back(u, weight);
        }
    }
}

bool MSTVerifier::areConnectedInForest(
    int u, int v, 
    const std::vector<std::vector<std::pair<int, double>>>& adj) {
    
    if (u == v) return true;
    if (u >= adj.size() || v >= adj.size() || u < 0 || v < 0) return false;
    
    std::vector<bool> visited(adj.size(), false);
    std::queue<int> q;
    
    q.push(u);
    visited[u] = true;
    
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        
        if (current == v) return true;
        
        for (const auto& neighbor : adj[current]) {
            int next = neighbor.first;
            if (!visited[next]) {
                visited[next] = true;
                q.push(next);
            }
        }
    }
    
    return false;
}