#ifndef VERIFIER_HPP
#define VERIFIER_HPP

#include <vector>
#include <tuple>
#include <unordered_set>
#include <functional>
#include <cmath>

const double EPSILON = 1e-9;

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

class MSTVerifier {
public:
    static std::unordered_set<int> findHeavyEdges(
        const std::vector<std::tuple<int, int, double, int>>& graph,
        const std::vector<std::tuple<int, int, double, int>>& forest,
        int numVertices);
    
private:
    static void buildTreeAdjacency(
        const std::vector<std::tuple<int, int, double, int>>& forest,
        int numVertices,
        std::vector<std::vector<std::pair<int, double>>>& adj);
    
    static bool areConnectedInForest(
        int u, int v, 
        const std::vector<std::vector<std::pair<int, double>>>& adj);
};

#endif