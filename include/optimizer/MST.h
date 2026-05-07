#ifndef NANODB_MST_H
#define NANODB_MST_H

#include "optimizer/Graph.h"

namespace NanoDB {

class MST {
public:
    struct MSTEdge {
        int from;
        int to;
        int weight;
    };
    
    MST();
    ~MST();
    
    MSTEdge* computeMST(Graph* graph, int* num_edges);
    
private:
    struct DisjointSet {
        int parent;
        int rank;
    };
    
    DisjointSet* sets_;
    int capacity_;
    
    void makeSet(int x);
    int find(int x);
    void unionSets(int x, int y);
};

} // namespace NanoDB

#endif // NANODB_MST_H
