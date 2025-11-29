#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <tuple>

class Graph {
private:
    int V; 
    std::vector<std::vector<std::pair<int, double>>> adjList; 
    std::vector<std::tuple<int, int, double>> edgeList; 
    bool directed;

public:
    Graph(int vertices, bool isDirected = false);
    
    void addEdge(int u, int v, double weight);
    int getVertices() const { return V; }
    int getEdges() const { return edgeList.size(); }
    const std::vector<std::vector<std::pair<int, double>>>& getAdjList() const { return adjList; }
    const std::vector<std::tuple<int, int, double>>& getEdgeList() const { return edgeList; }
    static Graph generateRandomGraph(int V, double density, double minWeight = 1.0, double maxWeight = 100.0);
    bool isConnected() const;
};

#endif