#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

#include <vector>

class UnionFind {
private:
    mutable std::vector<int> parent;  
    std::vector<int> rank;
    int components;

public:
    UnionFind(int n);
    
    int find(int x) const;  
    void unite(int x, int y);
    bool connected(int x, int y) const;  
    int getComponents() const { return components; }
};

#endif