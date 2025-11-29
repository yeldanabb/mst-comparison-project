#ifndef PRIM_PARALLEL_HPP
#define PRIM_PARALLEL_HPP

#include "mst_algorithm.hpp"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

class PrimParallel : public MSTAlgorithm {
private:
    int numThreads;
    
public:
    PrimParallel(int threads = 4) : numThreads(threads) {}
    
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { 
        return "Prim_Parallel_" + std::to_string(numThreads) + "threads"; 
    }
    
private:
    void processVertexRange(const Graph& graph, 
                          std::vector<bool>& inMST,
                          std::vector<double>& key,
                          std::vector<int>& parent,
                          std::atomic<int>& nextVertex,
                          std::mutex& queueMutex,
                          std::priority_queue<std::pair<double, int>,
                                            std::vector<std::pair<double, int>>,
                                            std::greater<std::pair<double, int>>>& pq);
};

#endif