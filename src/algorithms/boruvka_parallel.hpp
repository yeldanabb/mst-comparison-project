#ifndef BORUVKA_PARALLEL_HPP
#define BORUVKA_PARALLEL_HPP

#include "mst_algorithm.hpp"
#include "../data_structures/union_find.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <limits>
#include <unordered_set>  

class BoruvkaParallel : public MSTAlgorithm {
private:
    int numThreads;
    
public:
    BoruvkaParallel(int threads = std::thread::hardware_concurrency()) 
        : numThreads(threads) {}
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { 
        return "Boruvka_Parallel_" + std::to_string(numThreads) + "threads"; 
    }
    
private:
    struct EdgeInfo {
        int id;
        double weight;
        int target_component;
        int target_vertex;
        EdgeInfo() : id(-1), weight(std::numeric_limits<double>::max()), 
                    target_component(-1), target_vertex(-1) {}
        
        EdgeInfo(int i, double w, int tc, int tv) 
            : id(i), weight(w), target_component(tc), target_vertex(tv) {}
    };
};

#endif