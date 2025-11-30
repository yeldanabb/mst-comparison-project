#include "../src/data_structures/graph.hpp"
#include "../src/algorithms/kruskal.hpp"
#include "../src/algorithms/prim.hpp"
#include "../src/generators/graph_generator.hpp"
#include "../src/algorithms/kkt.hpp"  
#include "../src/algorithms/boruvka_parallel.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <iomanip>

struct Experiment {
    std::string name;
    int vertices;
    double density;
    std::vector<MSTResult> results;
};

void runSimpleExperiments() {
    std::cout << "---Simple Experiment Runner---" << std::endl;
    
    GraphGenerator generator(42);
    std::vector<std::unique_ptr<MSTAlgorithm>> algorithms;
    
    algorithms.push_back(std::make_unique<Kruskal>());
    algorithms.push_back(std::make_unique<Prim>());  
    algorithms.push_back(std::make_unique<KKT>());
    algorithms.push_back(std::make_unique<BoruvkaParallel>(2)); 
    algorithms.push_back(std::make_unique<BoruvkaParallel>(4));
    
    std::vector<Experiment> experiments;
    std::vector<int> sizes = {100, 500, 1000, 2000};
    std::vector<double> densities = {0.1, 0.5, 2.0};
    
    for (int size : sizes) {
        for (double density : densities) {
            Experiment exp;
            exp.name = "V" + std::to_string(size) + "_D" + std::to_string(density);
            exp.vertices = size;
            exp.density = density;
            
            std::cout << "\nTesting " << exp.name << std::endl;
            Graph graph = (density < 1.0) 
                ? generator.generateDenseGraph(size, density)
                : generator.generateSparseGraph(size, density);
            
            std::cout << "   Graph: " << graph.getVertices() << " vertices, " 
                      << graph.getEdges() << " edges" << std::endl;

            for (auto& algo : algorithms) {
                std::cout << "   Running " << std::setw(25) << std::left << algo->getName() << "...";
                std::cout.flush();
                MSTResult result = algo->solve(graph);
                exp.results.push_back(result);
                std::cout << " Time: " << std::setw(8) << std::fixed << std::setprecision(2) 
                          << result.executionTime << " ms"
                          << " Weight: " << std::setw(10) << std::fixed << std::setprecision(2)
                          << result.totalWeight << std::endl;
            }
            
            experiments.push_back(exp);
        }
    }
    
    std::ofstream csvFile("simple_results.csv");
    csvFile << "Experiment,Vertices,Density,Algorithm,Time(ms),Memory(KB),Weight\n";
    
    for (const auto& exp : experiments) {
        for (const auto& result : exp.results) {
            csvFile << exp.name << "," << exp.vertices << "," << exp.density << ","
                   << result.algorithmName << "," << result.executionTime << ","
                   << result.memoryUsage << "," << result.totalWeight << "\n";
        }
    }
    
    csvFile.close();
}

int main() {
    runSimpleExperiments();
    return 0;
}