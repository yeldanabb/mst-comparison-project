#ifndef MST_ALGORITHM_HPP
#define MST_ALGORITHM_HPP

#include "../data_structures/graph.hpp"
#include <vector>
#include <string>

struct MSTResult {
    std::vector<std::tuple<int, int, double>> edges; 
    double totalWeight;
    double executionTime;
    size_t memoryUsage;
    std::string algorithmName;
    
    MSTResult() : totalWeight(0.0), executionTime(0.0), memoryUsage(0) {}
};

class MSTAlgorithm {
public:
    virtual ~MSTAlgorithm() = default;
    virtual MSTResult solve(const Graph& graph) = 0;
    virtual std::string getName() const = 0;
};

#endif