#ifndef KKT_HPP
#define KKT_HPP

#include "mst_algorithm.hpp"
#include <vector>
#include <tuple>
#include <unordered_set>

struct KKTProblem {
    int num_vertices;
    std::vector<std::tuple<int, int, double, int>> graph_edges; 
    
    KKTProblem(int nv = 0, std::vector<std::tuple<int, int, double, int>> ge = {}) 
        : num_vertices(nv), graph_edges(ge) {}
    
    bool operator!=(const KKTProblem& rhs) const {
        if (num_vertices != rhs.num_vertices) return true;
        if (graph_edges.size() != rhs.graph_edges.size()) return true;
        return false;
    }
};


class KKT : public MSTAlgorithm {
public:
    MSTResult solve(const Graph& graph) override;
    std::string getName() const override { return "KKT"; }
    
private:
    std::unordered_set<int> kktAlgorithm(KKTProblem& P, unsigned int seed = 0);
    std::pair<std::unordered_set<int>, KKTProblem> boruvkaStep(const KKTProblem& P);
    KKTProblem removeIsolatedVertices(const KKTProblem& P);
    KKTProblem randomSampling(const KKTProblem& P, unsigned int seed = 0);
    std::unordered_set<int> findHeavyEdges(
        const std::vector<std::tuple<int, int, double, int>>& graph,
        const std::vector<std::tuple<int, int, double, int>>& forest,
        int n);
    int getSolutionCost(const KKTProblem& P, const std::unordered_set<int>& used);
    std::vector<std::tuple<int, int, double, int>> getMSTEdgesFromProblem(
        const std::unordered_set<int>& mst_id, const KKTProblem& P);
};

#endif