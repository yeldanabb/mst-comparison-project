#include "../data_structures/graph.hpp"
#include "../algorithms/kruskal.hpp"
#include "../algorithms/prim.hpp"
#include "../algorithms/kkt.hpp"  
#include "../algorithms/boruvka_parallel.hpp"  
#include "../generators/graph_generator.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void testGraphBasic() {
    std::cout << "Testing..." << std::endl;
    
    Graph graph(5, false);
    graph.addEdge(0, 1, 1.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 3.0);
    graph.addEdge(3, 4, 4.0);
    
    assert(graph.getVertices() == 5);
    assert(graph.getEdges() == 4);
    assert(graph.isConnected());
    
    std::cout << "Graph basic operations passed" << std::endl;
}

void testUnionFind() {
    UnionFind uf(5);
    assert(uf.connected(0, 0));
    assert(!uf.connected(0, 1));
    uf.unite(0, 1);
    assert(uf.connected(0, 1));
    uf.unite(1, 2);
    assert(uf.connected(0, 2));
    std::cout << "Union-Find passed" << std::endl;
}

void testKruskalSmall() {
    Graph graph(4, false);
    graph.addEdge(0, 1, 1.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 1.0);
    graph.addEdge(0, 3, 3.0);
    graph.addEdge(0, 2, 4.0);
    Kruskal kruskal;
    MSTResult result = kruskal.solve(graph);
    assert(result.edges.size() == 3);
    assert(std::abs(result.totalWeight - 4.0) < 1e-9); 
    std::cout << "Kruskal small test passed" << std::endl;
}

void testPrimSmall() {
    Graph graph(4, false);
    graph.addEdge(0, 1, 1.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 1.0);
    graph.addEdge(0, 3, 3.0);
    graph.addEdge(0, 2, 4.0);
    
    Prim prim;
    MSTResult result = prim.solve(graph);
    assert(result.edges.size() == 3);
    assert(std::abs(result.totalWeight - 4.0) < 1e-9); 
    std::cout << "Prim small test passed" << std::endl;
}


void testKKTSmall() {
    Graph graph(4, false);
    graph.addEdge(0, 1, 1.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 1.0);
    graph.addEdge(0, 3, 3.0);
    graph.addEdge(0, 2, 4.0);
    
    KKT kkt;
    MSTResult result = kkt.solve(graph);
    assert(result.edges.size() == 3);
    assert(std::abs(result.totalWeight - 4.0) < 1e-9); 
    std::cout << "KKT small test passed" << std::endl;
}

void testBoruvkaSmall() {
    Graph graph(4, false);
    graph.addEdge(0, 1, 1.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 1.0);
    graph.addEdge(0, 3, 3.0);
    graph.addEdge(0, 2, 4.0);
    
    BoruvkaParallel boruvka(2);  
    MSTResult result = boruvka.solve(graph);
    assert(result.edges.size() == 3);
    assert(std::abs(result.totalWeight - 4.0) < 1e-9); 
    std::cout << "Boruvka small test passed" << std::endl;
}

void testEdgeCases() {
    Graph single(1, false);
    Kruskal kruskal;
    MSTResult result = kruskal.solve(single);
    assert(result.edges.size() == 0);
    assert(std::abs(result.totalWeight - 0.0) < 1e-9);
    
    Graph two(2, false);
    two.addEdge(0, 1, 5.0);
    result = kruskal.solve(two);
    assert(result.edges.size() == 1);
    assert(std::abs(result.totalWeight - 5.0) < 1e-9);
    
    std::cout << "Edge cases test passed" << std::endl;
}

void testAllAlgorithmConsistency() {
    GraphGenerator generator(42);
    Graph graph = generator.generateSparseGraph(50, 4.0);
    
    Kruskal kruskal;
    Prim prim;  
    KKT kkt;
    BoruvkaParallel boruvka(2); 
    
    MSTResult kruskalResult = kruskal.solve(graph);
    MSTResult primResult = prim.solve(graph);
    MSTResult kktResult = kkt.solve(graph);
    MSTResult boruvkaResult = boruvka.solve(graph);
    
    assert(std::abs(kruskalResult.totalWeight - primResult.totalWeight) < 1e-9);
    assert(std::abs(kruskalResult.totalWeight - kktResult.totalWeight) < 1e-9);
    assert(std::abs(kruskalResult.totalWeight - boruvkaResult.totalWeight) < 1e-9); 
    
    std::cout << "consistency test passed" << std::endl;
}

void testGraphGenerator() {
    GraphGenerator generator(42); 
    
    Graph sparse = generator.generateSparseGraph(100, 4.0);
    assert(sparse.isConnected());
    assert(sparse.getEdges() >= 99); 
    Graph dense = generator.generateDenseGraph(50, 0.5);
    assert(dense.isConnected());
    std::cout << "Graph generator tests passed" << std::endl;
}

void testPerformanceSmall() {
    GraphGenerator generator(123);
    Graph graph = generator.generateDenseGraph(100, 0.3);
    
    Kruskal kruskal;
    Prim prim;
    BoruvkaParallel boruvka2(2);   
    BoruvkaParallel boruvka4(4);

    MSTResult kruskalResult = kruskal.solve(graph);
    MSTResult primResult = prim.solve(graph);
    MSTResult boruvka2Result = boruvka2.solve(graph);
    MSTResult boruvka4Result = boruvka4.solve(graph);
    
    std::cout << "Kruskal time: " << kruskalResult.executionTime << " ms" << std::endl;
    std::cout << "Prim time: " << primResult.executionTime << " ms" << std::endl;
    std::cout << "Boruvka (2 threads) time: " << boruvka2Result.executionTime << " ms" << std::endl;
    std::cout << "Boruvka (4 threads) time: " << boruvka4Result.executionTime << " ms" << std::endl;
}

void runAllTests() {
    testGraphBasic();
    testUnionFind();
    testKruskalSmall();
    testPrimSmall(); 
    testKKTSmall();
    testBoruvkaSmall();
    testAllAlgorithmConsistency();  
    testGraphGenerator();
    testEdgeCases(); 
    testPerformanceSmall();
    
    std::cout << "\nAll basic tests passed!" << std::endl;
}

int main() {
    runAllTests();
    return 0;
}