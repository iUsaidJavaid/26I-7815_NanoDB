#ifndef NANODB_GRAPH_H
#define NANODB_GRAPH_H

namespace NanoDB {

struct GraphEdge {
    int src;
    int dst;
    float weight;
    char joinCondition[128];
};

class Graph {
public:
    Graph(int maxNodes);
    ~Graph();

    void addNode(int id, const char* tableName);
    void addEdge(int src, int dst, float weight, const char* condition);

    void registerDefaultEdges();

    int getNodeCount() const;
    int getEdgeCount() const;
    GraphEdge* getEdges() const;
    const char* getNodeName(int id) const;
    void printGraph() const;

private:
    int maxNodes_;
    int nodeCount_;
    int edgeCount_;
    int maxEdges_;

    float** adjMatrix_;
    GraphEdge* edgeList_;
    char** names_;

    void copyString(char* dest, int destSize, const char* src);
};

} // namespace NanoDB

#endif // NANODB_GRAPH_H
