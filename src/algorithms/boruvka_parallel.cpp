#include "boruvka_parallel.hpp"
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <mutex>
#include <unordered_set>  

MSTResult BoruvkaParallel::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    
    Timer timer;
    timer.start();
    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    
    int V = graph.getVertices();
    const auto& edges = graph.getEdgeListWithIds();
    UnionFind uf(V);
    std::unordered_set<int> mstEdgeIds;
    std::mutex mstMutex;
    int components = V;
    
    while (components > 1) {
        std::vector<EdgeInfo> cheapestEdges(V);
        auto findCheapestEdges = [&](int start, int end) {  
            for (int i = start; i < end; ++i) {
                const auto& edge = edges[i];
                int u = std::get<0>(edge);
                int v = std::get<1>(edge);
                double weight = std::get<2>(edge);
                int id = std::get<3>(edge);
                if (mstEdgeIds.find(id) != mstEdgeIds.end()) {
                    continue;
                }
                int compU = uf.find(u);
                int compV = uf.find(v);
                
                if (compU != compV) {
                    if (weight < cheapestEdges[compU].weight) {
                        cheapestEdges[compU] = EdgeInfo(id, weight, compV, v);
                    }
                    if (weight < cheapestEdges[compV].weight) {
                        cheapestEdges[compV] = EdgeInfo(id, weight, compU, u);
                    }
                }
            }
        };
        int edgesPerThread = (edges.size() + numThreads - 1) / numThreads;
        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            int start = i * edgesPerThread;
            int end = std::min(start + edgesPerThread, static_cast<int>(edges.size()));
            if (start < static_cast<int>(edges.size())) {
                threads.emplace_back(findCheapestEdges, start, end);
            }
        }
        for (auto& thread : threads) {
            thread.join();
        }
        
        int edgesAdded = 0;
        for (int comp = 0; comp < V; ++comp) {
            const EdgeInfo& edgeInfo = cheapestEdges[comp];
            if (edgeInfo.id != -1) {
                int compU = comp;
                int compV = edgeInfo.target_component;
                if (!uf.connected(compU, compV)) {
                    uf.unite(compU, compV);
                    {
                        std::lock_guard<std::mutex> lock(mstMutex);
                        mstEdgeIds.insert(edgeInfo.id);
                    }
                    edgesAdded++;
                }
            }
        }
        
        if (edgesAdded == 0) {
            break;
        }
        components -= edgesAdded;
    }
    const auto& idToEdgeMap = graph.getIdToEdgeMap();
    for (int id : mstEdgeIds) {
        auto it = idToEdgeMap.find(id);
        if (it != idToEdgeMap.end()) {
            const auto& edge = it->second;
            result.edges.push_back({std::get<0>(edge), std::get<1>(edge), std::get<2>(edge)});
            result.totalWeight += std::get<2>(edge);
        }
    }
    
    timer.stop();
    result.executionTime = timer.elapsedMilliseconds();
    result.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() - initialMemory;
    return result;
}