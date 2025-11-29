#ifndef PRIM_HPP
#define PRIM_HPP

#include "mst_algorithm.hpp"
#include <string>

class Prim : public MSTAlgorithm {
public:
    Prim() = default;
    
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { 
        return "Prim_BinaryHeap"; 
    }
};

#endif