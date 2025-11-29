#include "union_find.hpp"
#include <stdexcept>

UnionFind::UnionFind(int n) : components(n) {
    parent.resize(n);
    rank.resize(n, 0);
    for (int i = 0; i < n; ++i) {
        parent[i] = i;
    }
}

int UnionFind::find(int x) const {
    if (x < 0 || x >= parent.size()) {
        throw std::out_of_range("Vertex index out of bounds");
    }
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void UnionFind::unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    if (rootX != rootY) {
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        components--;
    }
}

bool UnionFind::connected(int x, int y) const {
    return find(x) == find(y);
}