#include "../src/data_structures/graph.hpp"
#include "../src/algorithms/kruskal.hpp"
#include "../src/algorithms/prim.hpp"
#include "../src/algorithms/kkt.hpp"
#include "../src/algorithms/boruvka_parallel.hpp"
#include "../src/generators/graph_generator.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <iomanip>

void runFocusedKKTExperiments() {
    std::cout << "--- KKT Analysis Runner ---" << std::endl;
    
    GraphGenerator generator(42);
    std::vector<std::unique_ptr<MSTAlgorithm>> algorithms;
    
    algorithms.push_back(std::make_unique<Kruskal>());
    algorithms.push_back(std::make_unique<Prim>());
    algorithms.push_back(std::make_unique<KKT>());
    algorithms.push_back(std::make_unique<BoruvkaParallel>(2));
    algorithms.push_back(std::make_unique<BoruvkaParallel>(4));
    
    std::vector<int> sizes = {100, 500, 1000, 2000, 5000};
    std::vector<double> densities = {0.01, 0.1, 1.0, 5.0};
    
    std::ofstream csvFile("focused_kkt_results.csv");
    csvFile << "Experiment,Vertices,Edges,Density,Algorithm,Time(ms),Memory(KB),Weight\n";
    
    int total = sizes.size() * densities.size();
    int current = 0;
    
    for (int size : sizes) {
        for (double density : densities) {
            current++;
            std::cout << "\n[" << current << "/" << total << "] Testing V" 
                      << size << "_D" << density << std::endl;
            
            Graph graph = (density < 1.0) 
                ? generator.generateDenseGraph(size, density)
                : generator.generateSparseGraph(size, density);
            
            std::cout << "   Graph: " << graph.getVertices() << " vertices, " 
                      << graph.getEdges() << " edges" << std::endl;
            
            for (auto& algo : algorithms) {
                std::cout << "   Running " << std::setw(25) << std::left 
                          << algo->getName() << "...";
                std::cout.flush();
                
                try {
                    MSTResult result = algo->solve(graph);
                    std::cout << " Time: " << std::setw(8) << std::fixed 
                              << std::setprecision(2) << result.executionTime 
                              << " ms" << std::endl;
                    
                    csvFile << "V" << size << "_D" << density << "," 
                           << size << "," << graph.getEdges() << "," << density << ","
                           << result.algorithmName << "," << result.executionTime << ","
                           << result.memoryUsage << "," << result.totalWeight << "\n";
                    
                } catch (const std::exception& e) {
                    std::cout << " ERROR: " << e.what() << std::endl;
                }
            }
        }
    }
    
    csvFile.close();
}

int main() {
    runFocusedKKTExperiments();
    return 0;
}