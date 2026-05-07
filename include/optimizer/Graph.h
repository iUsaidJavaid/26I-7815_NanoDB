#ifndef NANODB_GRAPH_H
#define NANODB_GRAPH_H

namespace NanoDB {

class Graph {
public:
    struct Edge {
        int from;
        int to;
        int weight;
        Edge* next;
    };
    
    struct Vertex {
        int id;
        Edge* edges;
        Vertex* next;
    };
    
    Graph(int max_vertices);
    ~Graph();
    
    bool addVertex(int id);
    bool addEdge(int from, int to, int weight);
    bool removeEdge(int from, int to);
    bool hasEdge(int from, int to);
    
    int getDegree(int vertex);
    Edge* getEdges(int vertex);
    
    void clear();
    
private:
    Vertex* vertices_;
    int max_vertices_;
    int num_vertices_;
    
    Vertex* findVertex(int id);
};

} // namespace NanoDB

#endif // NANODB_GRAPH_H
