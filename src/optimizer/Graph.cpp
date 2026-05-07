#include "optimizer/Graph.h"

namespace NanoDB {

Graph::Graph(int max_vertices) 
    : vertices_(nullptr), max_vertices_(max_vertices), num_vertices_(0) {
}

Graph::~Graph() {
    clear();
}

bool Graph::addVertex(int id) {
    return false;
}

bool Graph::addEdge(int from, int to, int weight) {
    return false;
}

bool Graph::removeEdge(int from, int to) {
    return false;
}

bool Graph::hasEdge(int from, int to) {
    return false;
}

int Graph::getDegree(int vertex) {
    return 0;
}

Graph::Edge* Graph::getEdges(int vertex) {
    return nullptr;
}

void Graph::clear() {
}

Graph::Vertex* Graph::findVertex(int id) {
    return nullptr;
}

} // namespace NanoDB
