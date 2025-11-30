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
#include <chrono>

struct LargeExperiment {
    std::string name;
    int vertices;
    double density;
    int edges;
    std::vector<MSTResult> results;
};

void runLargeScaleExperiments() {
    std::cout << "---Large scale experiment runner---" << std::endl;
    std::cout << "Testing asymptotic behavior with large graphs:" << std::endl;
    
    GraphGenerator generator(42);
    std::vector<std::unique_ptr<MSTAlgorithm>> algorithms;
    algorithms.push_back(std::make_unique<Kruskal>());
    algorithms.push_back(std::make_unique<Prim>());   
    algorithms.push_back(std::make_unique<KKT>());
    algorithms.push_back(std::make_unique<BoruvkaParallel>(2));   
    algorithms.push_back(std::make_unique<BoruvkaParallel>(4)); 
    std::vector<LargeExperiment> experiments;
    
    std::vector<int> sizes = {1000, 5000, 10000, 25000, 50000};
    std::vector<double> densities = {0.01, 0.1, 1.0, 5.0};
    
    int totalExperiments = sizes.size() * densities.size();
    int currentExperiment = 0;
    for (int size : sizes) {
        for (double density : densities) {
            currentExperiment++;
            LargeExperiment exp;
            exp.name = "V" + std::to_string(size) + "_D" + std::to_string(density);
            exp.vertices = size;
            exp.density = density;
            
            std::cout << "\n [" << currentExperiment << "/" << totalExperiments 
                      << "] Testing " << exp.name << std::endl;
            
            if (size >= 50000 && density > 5.0) {
                std::cout << "   Skipping (memory constraints)" << std::endl;
                continue;
            }
            auto startGen = std::chrono::high_resolution_clock::now();
            
            try {
                Graph graph = (density < 1.0) 
                    ? generator.generateDenseGraph(size, density)
                    : generator.generateSparseGraph(size, density);
                
                auto endGen = std::chrono::high_resolution_clock::now();
                double genTime = std::chrono::duration<double, std::milli>(endGen - startGen).count();
                exp.edges = graph.getEdges();
                std::cout << "   Graph: " << graph.getVertices() << " vertices, " 
                          << graph.getEdges() << " edges (generated in " 
                          << std::fixed << std::setprecision(2) << genTime << " ms)" << std::endl;
                
                if (!graph.isConnected()) {
                    for (int i = 1; i < size; i++) {
                        graph.addEdge(i-1, i, 1.0);
                    }
                }
                
                for (auto& algo : algorithms) {
                    std::cout << "   Running " << std::setw(25) << std::left << algo->getName() << "...";
                    std::cout.flush();
                    
                    try {
                        MSTResult result = algo->solve(graph);
                        exp.results.push_back(result);
                        std::cout << " Time: " << std::setw(8) << std::fixed << std::setprecision(2) 
                                  << result.executionTime << " ms" << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << " ERROR: " << e.what() << std::endl;
                        MSTResult errorResult;
                        errorResult.algorithmName = algo->getName() + "_ERROR";
                        errorResult.executionTime = -1;
                        errorResult.totalWeight = -1;
                        exp.results.push_back(errorResult);
                    }
                }
                
                experiments.push_back(exp);
                
            } catch (const std::exception& e) {
                std::cout << "   ERROR: " << e.what() << std::endl;
            }
        }
    }
    
    std::ofstream csvFile("large_scale_results.csv");
    csvFile << "Experiment,Vertices,Edges,Density,Algorithm,Time(ms),Memory(KB),Weight\n";
    
    for (const auto& exp : experiments) {
        for (const auto& result : exp.results) {
            csvFile << exp.name << "," << exp.vertices << "," << exp.edges << "," << exp.density << ","
                   << result.algorithmName << "," << result.executionTime << ","
                   << result.memoryUsage << "," << result.totalWeight << "\n";
        }
    }
    
    csvFile.close();
    
    std::cout << "\nExperiment Summary:" << std::endl;
    std::cout << "Total graphs tested: " << experiments.size() << std::endl;
    std::cout << "Maximum vertices: " << sizes.back() << std::endl;
    std::cout << "Algorithms tested: " << algorithms.size() << std::endl;
}

int main() {
    runLargeScaleExperiments();
    return 0;
}