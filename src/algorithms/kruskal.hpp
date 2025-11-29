#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "mst_algorithm.hpp"
#include "../data_structures/union_find.hpp"
#include <algorithm>

class Kruskal : public MSTAlgorithm {
public:
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { return "Kruskal"; }
    
private:
    static bool compareEdges(const std::tuple<int, int, double>& a, 
                           const std::tuple<int, int, double>& b);
};

#endif