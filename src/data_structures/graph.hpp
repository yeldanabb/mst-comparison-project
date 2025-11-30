#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <tuple>
#include <unordered_map>

class Graph {
private:
    int V; 
    std::vector<std::vector<std::pair<int, double>>> adjList; 
    std::vector<std::tuple<int, int, double>> edgeList; 
    std::vector<std::tuple<int, int, double, int>> edgeListWithIds; 
    std::unordered_map<int, std::tuple<int, int, double>> idToEdgeMap; 
    bool directed;
    int nextEdgeId;

public:
    Graph(int vertices, bool isDirected = false);
    void addEdge(int u, int v, double weight);
    void addEdgeWithId(int u, int v, double weight, int id);
    int getVertices() const { return V; }
    int getEdges() const { return edgeList.size(); }
    const std::vector<std::vector<std::pair<int, double>>>& getAdjList() const { return adjList; }
    const std::vector<std::tuple<int, int, double>>& getEdgeList() const { return edgeList; }
    const std::vector<std::tuple<int, int, double, int>>& getEdgeListWithIds() const { return edgeListWithIds; }
    const std::unordered_map<int, std::tuple<int, int, double>>& getIdToEdgeMap() const { return idToEdgeMap; }
    static Graph generateRandomGraph(int V, double density, double minWeight = 1.0, double maxWeight = 100.0);
    bool isConnected() const;
};

#endif