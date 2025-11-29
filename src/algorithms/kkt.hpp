#ifndef KKT_HPP
#define KKT_HPP

#include "mst_algorithm.hpp"
#include "../data_structures/union_find.hpp"
#include "../generators/graph_generator.hpp"
#include <vector>
#include <random>
#include <algorithm>

class KKT : public MSTAlgorithm {
public:
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { return "Karger-Klein-Tarjan"; }
    
private:
    std::vector<std::tuple<int, int, double>> kktRecursive(
        const Graph& graph, 
        UnionFind& uf,
        std::mt19937& rng);

    void boruvkaStep(
        const Graph& graph, 
        UnionFind& uf,
        std::vector<std::tuple<int, int, double>>& mstEdges);
    
    Graph sampleGraph(const Graph& graph, double probability, std::mt19937& rng);
    
    void filterEdges(
        const Graph& graph,
        const std::vector<std::tuple<int, int, double>>& sampledMST,
        std::vector<std::tuple<int, int, double>>& remainingEdges);
    
    Graph contractGraph(const Graph& graph, const UnionFind& uf);
};

#endif