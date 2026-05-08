#ifndef NANODB_MST_H
#define NANODB_MST_H

#include "optimizer/Graph.h"
#include "storage/Pager.h"
#include "catalog/SystemCatalog.h"
#include "common/Types.h"

namespace NanoDB {

class UnionFind {
public:
    UnionFind(int n);
    ~UnionFind();

    int find(int x);
    void unite(int x, int y);
    bool connected(int x, int y);

private:
    int* parent_;
    int* rank_;
    int size_;
};

class MSTOptimizer {
public:
    GraphEdge* computeMST(Graph& graph, int& edgeCount);
    char* buildJoinPath(GraphEdge* mstEdges, int edgeCount, Graph& graph);
    void logMSTDecision(const char* path);
    float computeTotalCost(GraphEdge* mstEdges, int edgeCount) const;

private:
    void insertionSort(GraphEdge* edges, int count);
};

class JoinExecutor {
public:
    Row** executeJoin(const char* table1, const char* table2, const char* table3,
                      Pager& pager, SystemCatalog& catalog, int& resultCount);
};

} // namespace NanoDB

#endif // NANODB_MST_H
