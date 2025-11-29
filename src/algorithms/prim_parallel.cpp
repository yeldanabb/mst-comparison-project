#include "prim_parallel.hpp"
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include <limits>
#include <iostream>

MSTResult PrimParallel::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    Timer timer;
    timer.start();
    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    int V = graph.getVertices();
    const auto& adjList = graph.getAdjList();
    std::vector<bool> inMST(V, false);
    std::vector<double> key(V, std::numeric_limits<double>::max());
    std::vector<int> parent(V, -1);
    
    std::priority_queue<std::pair<double, int>, 
                       std::vector<std::pair<double, int>>,
                       std::greater<std::pair<double, int>>> pq;
    std::atomic<int> nextVertex(0);
    std::mutex queueMutex;
    key[0] = 0.0;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        pq.push({0.0, 0});
    }
    
    auto worker = [&]() {
        while (true) {
            int u = -1;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (pq.empty()) break;
                auto top = pq.top();
                u = top.second;
                pq.pop();
            }
            
            if (u == -1) continue;
            if (inMST[u]) continue;
            inMST[u] = true;
            if (parent[u] != -1) {
                std::lock_guard<std::mutex> lock(queueMutex);
                result.edges.push_back({parent[u], u, key[u]});
                result.totalWeight += key[u];
            }
            for (const auto& neighbor : adjList[u]) {
                int v = neighbor.first;
                double weight = neighbor.second;
                if (!inMST[v] && weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                    std::lock_guard<std::mutex> lock(queueMutex);
                    pq.push({key[v], v});
                }
            }
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    
    timer.stop();
    result.executionTime = timer.elapsedMilliseconds();
    result.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() - initialMemory;
    return result;
}