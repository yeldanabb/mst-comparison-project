#include "../src/data_structures/graph.hpp"
#include "../src/algorithms/kruskal.hpp"
#include "../src/algorithms/prim.hpp"
#include "../src/algorithms/kkt.hpp"
#include "../src/algorithms/boruvka_parallel.hpp"
#include "../src/generators/graph_generator.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map> 
#include <memory>
#include <iomanip>
#include <chrono>

struct ComprehensiveExperiment {
    std::string name;
    int vertices;
    double density;
    int edges;
    std::vector<MSTResult> results;
    std::string status;
};

size_t estimateMemoryUsage(int V, double density) {
    size_t maxEdges = static_cast<size_t>(V) * (V - 1) / 2;
    size_t edges = static_cast<size_t>(maxEdges * density);
    if (edges > maxEdges) edges = maxEdges;
    size_t adjacencyMemory = V * sizeof(std::vector<std::pair<int, double>>) + 
                           edges * sizeof(std::pair<int, double>);
    size_t edgeListMemory = edges * sizeof(std::tuple<int, int, double>);
    return adjacencyMemory + edgeListMemory + (100 * 1024 * 1024); 
}

bool shouldSkipDueToMemory(int V, double density) {
    size_t estimatedMemory = estimateMemoryUsage(V, density);
    size_t safeLimit = 800 * 1024 * 1024; 
    if (estimatedMemory > safeLimit) {
        return true;
    }
    
    if (V >= 100000 && density > 1.0) return true;
    if (V >= 50000 && density > 5.0) return true;
    if (V >= 20000 && density > 10.0) return true;
    return false;
}

void runComprehensiveExperiments() {
    std::cout << "---Comprehensive experiment runner---" << std::endl;
    std::cout << "Testing 30+ graphs for statistical significance:" << std::endl;
    std::cout << "Memory limit: 800 MB per graph" << std::endl;
    
    GraphGenerator generator(42);
    std::vector<std::unique_ptr<MSTAlgorithm>> algorithms;
    
    algorithms.push_back(std::make_unique<Kruskal>());
    algorithms.push_back(std::make_unique<Prim>());
    algorithms.push_back(std::make_unique<KKT>());
    algorithms.push_back(std::make_unique<BoruvkaParallel>(2));   
    algorithms.push_back(std::make_unique<BoruvkaParallel>(4)); 
    std::vector<ComprehensiveExperiment> experiments;
    
    std::vector<int> sizes = {
        100, 200, 500,         
        1000, 2000, 5000,        
        10000, 20000, 50000,     
        100000                   
    };
    
    std::vector<double> densities = {
        0.01, 0.02, 0.05,        
        0.1, 0.2, 0.5,           
        1.0, 2.0, 5.0,          
        10.0, 20.0             
    };
    
    int totalExperiments = sizes.size() * densities.size();
    int currentExperiment = 0;
    int graphsTested = 0;
    int graphsSkipped = 0;
    std::cout << "Total possible graphs: " << totalExperiments << std::endl;
    for (int size : sizes) {
        for (double density : densities) {
            currentExperiment++;
            ComprehensiveExperiment exp;
            exp.name = "V" + std::to_string(size) + "_D" + std::to_string(density);
            exp.vertices = size;
            exp.density = density;
            std::cout << "\n[" << currentExperiment << "] Testing " << exp.name;
            if (shouldSkipDueToMemory(size, density)) {
                std::cout << " - SKIPPED (memory constraints)" << std::endl;
                exp.status = "SKIPPED_MEMORY";
                experiments.push_back(exp);
                graphsSkipped++;
                continue;
            }
            
            std::cout << std::endl;
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
                    std::cout << "   Graph is not connected, adding edges." << std::endl;
                    for (int i = 1; i < size; i++) {
                        graph.addEdge(i-1, i, 1.0 + (i * 0.1));
                    }
                }
                
                bool allSuccessful = true;
                for (auto& algo : algorithms) {
                    std::cout << "   Running " << std::setw(25) << std::left << algo->getName() << "...";
                    std::cout.flush();
                    try {
                        auto algoStart = std::chrono::high_resolution_clock::now();
                        MSTResult result = algo->solve(graph);
                        auto algoEnd = std::chrono::high_resolution_clock::now();
                        double measuredTime = std::chrono::duration<double, std::milli>(algoEnd - algoStart).count();
                        result.executionTime = measuredTime; 
                        exp.results.push_back(result);
                        std::cout << " Time: " << std::setw(8) << std::fixed << std::setprecision(2) 
                                  << result.executionTime << " ms";
                        
                        if (result.totalWeight < 0) {
                            std::cout << " [INVALID_WEIGHT]";
                            allSuccessful = false;
                        }
                        std::cout << std::endl;
                        
                    } catch (const std::bad_alloc& e) {
                        std::cout << " MEMORY_ERROR" << std::endl;
                        MSTResult errorResult;
                        errorResult.algorithmName = algo->getName() + "_MEMORY_ERROR";
                        errorResult.executionTime = -1;
                        errorResult.totalWeight = -1;
                        errorResult.memoryUsage = -1;
                        exp.results.push_back(errorResult);
                        allSuccessful = false;
                    } catch (const std::exception& e) {
                        std::cout << " ERROR: " << e.what() << std::endl;
                        MSTResult errorResult;
                        errorResult.algorithmName = algo->getName() + "_ERROR";
                        errorResult.executionTime = -1;
                        errorResult.totalWeight = -1;
                        errorResult.memoryUsage = -1;
                        exp.results.push_back(errorResult);
                        allSuccessful = false;
                    }
                }
                exp.status = allSuccessful ? "COMPLETED" : "PARTIAL_ERROR";
                experiments.push_back(exp);
                graphsTested++;
                
            } catch (const std::bad_alloc& e) {
                std::cout << "   MEMORY ERROR generating graph: " << e.what() << std::endl;
                exp.status = "GENERATION_MEMORY_ERROR";
                experiments.push_back(exp);
                graphsSkipped++;
            } catch (const std::exception& e) {
                std::cout << "   ERROR generating graph: " << e.what() << std::endl;
                exp.status = "GENERATION_ERROR";
                experiments.push_back(exp);
                graphsSkipped++;
            }
        }
    }
    
    std::ofstream csvFile("comprehensive_results.csv");
    csvFile << "Experiment,Vertices,Edges,Density,Status,Algorithm,Time(ms),Memory(KB),Weight\n";
    
    for (const auto& exp : experiments) {
        for (const auto& result : exp.results) {
            csvFile << exp.name << "," << exp.vertices << "," << exp.edges << "," << exp.density << ","
                   << exp.status << "," << result.algorithmName << "," << result.executionTime << ","
                   << result.memoryUsage << "," << result.totalWeight << "\n";
        }
    }
    
    csvFile.close();
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Experiment summary" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "Total graphs successfully tested: " << graphsTested << "/" << totalExperiments 
              << " (" << (graphsTested * 100 / totalExperiments) << "%)" << std::endl;
    std::cout << "Total graphs skipped/failed: " << graphsSkipped << "/" << totalExperiments 
              << " (" << (graphsSkipped * 100 / totalExperiments) << "%)" << std::endl;
    std::cout << "Maximum vertices: " << sizes.back() << std::endl;
    std::map<std::string, int> statusCount;
    for (const auto& exp : experiments) {
        statusCount[exp.status]++;
    }
    
    std::cout << "\nStatus breakdown:" << std::endl;
    for (const auto& [status, count] : statusCount) {
        std::cout << "  " << std::setw(20) << std::left << status << ": " << count << std::endl;
    }
    
    std::cout << "Algorithms tested: " << algorithms.size() << std::endl;
    std::cout << "Algorithm list: ";
    for (size_t i = 0; i < algorithms.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << algorithms[i]->getName();
    }
    std::cout << std::endl;
}

int main() {
    runComprehensiveExperiments();
    return 0;
}