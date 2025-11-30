#include "kkt.hpp"
#include "../algorithms/kruskal.hpp"
#include "../data_structures/union_find.hpp"
#include "../utils/timer.hpp"
#include "../utils/memory_monitor.hpp"
#include "verifier.hpp"
#include <iostream>
#include <map>
#include <unordered_map>
#include <random>
#include <algorithm>

MSTResult KKT::solve(const Graph& graph) {
    MSTResult result;
    result.algorithmName = getName();
    
    Timer timer;
    timer.start();
    size_t initialMemory = MemoryMonitor::getCurrentMemoryUsage();
    
    KKTProblem P(graph.getVertices(), graph.getEdgeListWithIds());
    std::random_device rd;
    std::mt19937 rng(rd());
    
    auto mstEdgeIds = kktAlgorithm(P, rng());
    
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

std::unordered_set<int> KKT::kktAlgorithm(KKTProblem& P, unsigned int seed) {
    std::unordered_set<int> result;
    
    if (P.graph_edges.empty()) return result;

    if (P.num_vertices <= 10 || P.graph_edges.size() <= P.num_vertices * 2) {
        Graph tempGraph(P.num_vertices, false);
        for (const auto& edge : P.graph_edges) {
            tempGraph.addEdgeWithId(std::get<0>(edge), std::get<1>(edge), 
                                   std::get<2>(edge), std::get<3>(edge));
        }
        Kruskal kruskal;
        MSTResult kruskalResult = kruskal.solve(tempGraph);
        
        std::unordered_set<int> base_result;
        for (const auto& mst_edge : kruskalResult.edges) {
            int mst_u = std::get<0>(mst_edge);
            int mst_v = std::get<1>(mst_edge);
            
            for (const auto& orig_edge : P.graph_edges) {
                int orig_u = std::get<0>(orig_edge);
                int orig_v = std::get<1>(orig_edge);
                int orig_id = std::get<3>(orig_edge);
                
                if ((orig_u == mst_u && orig_v == mst_v) || 
                    (orig_u == mst_v && orig_v == mst_u)) {
                    base_result.insert(orig_id);
                    break;
                }
            }
        }
        return base_result;
    }
    
    auto [edges1, P1] = boruvkaStep(P);
    auto [edges2, G] = boruvkaStep(P1);
    
    result.insert(edges1.begin(), edges1.end());
    result.insert(edges2.begin(), edges2.end());
    
    if (G.num_vertices == 0 || G.graph_edges.empty()) {
        return result;
    }
    
    KKTProblem H = randomSampling(G, seed);
    std::unordered_set<int> F_H = kktAlgorithm(H, seed);
    
    std::vector<std::tuple<int, int, double, int>> forest_F;
    for (const auto& edge : G.graph_edges) {
        if (F_H.find(std::get<3>(edge)) != F_H.end()) {
            forest_F.push_back(edge);
        }
    }
    
    auto heavy_edges = findHeavyEdges(G.graph_edges, forest_F, G.num_vertices);
    
    std::vector<std::tuple<int, int, double, int>> remaining_edges;
    for (const auto& edge : G.graph_edges) {
        if (heavy_edges.find(std::get<3>(edge)) == heavy_edges.end()) {
            remaining_edges.push_back(edge);
        }
    }
    
    KKTProblem G_remaining(G.num_vertices, remaining_edges);
    G_remaining = removeIsolatedVertices(G_remaining);
    
    auto F_prime = kktAlgorithm(G_remaining, seed);
    result.insert(F_prime.begin(), F_prime.end());
    
    return result;
}

std::pair<std::unordered_set<int>, KKTProblem> KKT::boruvkaStep(const KKTProblem& P) {
    int n = P.num_vertices;
    UnionFind uf(n);
    std::unordered_set<int> mst_edges;
    
    std::unordered_map<int, std::tuple<int, double, int>> min_edge;
    
    for (size_t i = 0; i < P.graph_edges.size(); ++i) {
        const auto& edge = P.graph_edges[i];
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        int id = std::get<3>(edge);
        
        int comp_u = uf.find(u);
        int comp_v = uf.find(v);
        
        if (comp_u != comp_v) {
            if (min_edge.find(comp_u) == min_edge.end() || weight < std::get<1>(min_edge[comp_u])) {
                min_edge[comp_u] = {i, weight, comp_v};
            }
            if (min_edge.find(comp_v) == min_edge.end() || weight < std::get<1>(min_edge[comp_v])) {
                min_edge[comp_v] = {i, weight, comp_u};
            }
        }
    }
    
    for (const auto& [comp, edge_info] : min_edge) {
        int edge_idx = std::get<0>(edge_info);
        const auto& edge = P.graph_edges[edge_idx];
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int id = std::get<3>(edge);
        
        if (!uf.connected(u, v)) {
            uf.unite(u, v);
            mst_edges.insert(id);
        }
    }
    
    std::unordered_map<int, int> comp_to_new_id;
    int new_id = 0;
    for (int i = 0; i < n; ++i) {
        int comp = uf.find(i);
        if (comp_to_new_id.find(comp) == comp_to_new_id.end()) {
            comp_to_new_id[comp] = new_id++;
        }
    }
    
    std::map<std::pair<int, int>, std::tuple<int, double, int>> inter_comp_edges;
    for (size_t i = 0; i < P.graph_edges.size(); ++i) {
        const auto& edge = P.graph_edges[i];
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        double weight = std::get<2>(edge);
        int id = std::get<3>(edge);
        
        if (mst_edges.find(id) != mst_edges.end()) continue;
        
        int comp_u = comp_to_new_id[uf.find(u)];
        int comp_v = comp_to_new_id[uf.find(v)];
        
        if (comp_u != comp_v) {
            auto key = std::make_pair(std::min(comp_u, comp_v), std::max(comp_u, comp_v));
            if (inter_comp_edges.find(key) == inter_comp_edges.end() || 
                weight < std::get<1>(inter_comp_edges[key])) {
                inter_comp_edges[key] = {i, weight, id};
            }
        }
    }
    
    std::vector<std::tuple<int, int, double, int>> contracted_edges;
    for (const auto& [comp_pair, edge_info] : inter_comp_edges) {
        double weight = std::get<1>(edge_info);
        int id = std::get<2>(edge_info);
        contracted_edges.emplace_back(comp_pair.first, comp_pair.second, weight, id);
    }
    
    return {mst_edges, KKTProblem(new_id, contracted_edges)};
}

KKTProblem KKT::removeIsolatedVertices(const KKTProblem& P) {
    std::vector<int> degree(P.num_vertices, 0);
    
    for (const auto& e : P.graph_edges) {
        int from = std::get<0>(e);
        int to = std::get<1>(e);
        degree[from]++;
        degree[to]++;
    }
    
    std::vector<int> new_node_id(P.num_vertices, -1);
    int next_id = 0;
    for (int i = 0; i < P.num_vertices; ++i) {
        if (degree[i] > 0) {
            new_node_id[i] = next_id++;
        }
    }
    
    std::vector<std::tuple<int, int, double, int>> new_edges;
    for (const auto& e : P.graph_edges) {
        int from = std::get<0>(e);
        int to = std::get<1>(e);
        double cost = std::get<2>(e);
        int id = std::get<3>(e);
        
        if (new_node_id[from] != -1 && new_node_id[to] != -1) {
            new_edges.emplace_back(new_node_id[from], new_node_id[to], cost, id);
        }
    }
    
    return KKTProblem(next_id, new_edges);
}

KKTProblem KKT::randomSampling(const KKTProblem& P, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    std::vector<std::tuple<int, int, double, int>> H;
    for (const auto& E : P.graph_edges) {
        if (dist(rng) < 0.5) {
            H.push_back(E);
        }
    }
    
    KKTProblem P_H(P.num_vertices, H);
    return removeIsolatedVertices(P_H);
}

std::unordered_set<int> KKT::findHeavyEdges(
    const std::vector<std::tuple<int, int, double, int>>& graph,
    const std::vector<std::tuple<int, int, double, int>>& forest,
    int n) {
    return MSTVerifier::findHeavyEdges(graph, forest, n);
}