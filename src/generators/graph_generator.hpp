#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "../data_structures/graph.hpp"
#include <random>

class GraphGenerator {
private:
    std::mt19937 rng;
    
public:
    GraphGenerator(unsigned seed = std::random_device{}());
    
    Graph generateSparseGraph(int V, double averageDegree = 6.0);
    Graph generateDenseGraph(int V, double density = 0.5);
    Graph generateCompleteGraph(int V);
    Graph generateGridGraph(int rows, int cols);
    
    Graph generateGraphWithParameters(int V, int E);
};

#endif