#include "optimizer/Graph.h"
#include <iostream>

namespace NanoDB {

static int strLen(const char* s) {
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

Graph::Graph(int maxNodes)
    : maxNodes_(maxNodes), nodeCount_(0), edgeCount_(0), maxEdges_(maxNodes * (maxNodes - 1) / 2) {
    adjMatrix_ = new float*[maxNodes_];
    for (int i = 0; i < maxNodes_; ++i) {
        adjMatrix_[i] = new float[maxNodes_];
        for (int j = 0; j < maxNodes_; ++j) {
            adjMatrix_[i][j] = 0.0f;
        }
    }

    edgeList_ = new GraphEdge[maxEdges_];

    names_ = new char*[maxNodes_];
    for (int i = 0; i < maxNodes_; ++i) {
        names_[i] = new char[64];
        names_[i][0] = '\0';
    }

    registerDefaultEdges();
}

Graph::~Graph() {
    for (int i = 0; i < maxNodes_; ++i) {
        delete[] adjMatrix_[i];
    }
    delete[] adjMatrix_;
    delete[] edgeList_;
    for (int i = 0; i < maxNodes_; ++i) {
        delete[] names_[i];
    }
    delete[] names_;
}

void Graph::copyString(char* dest, int destSize, const char* src) {
    int i = 0;
    while (src[i] != '\0' && i < destSize - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void Graph::addNode(int id, const char* tableName) {
    if (id < 0 || id >= maxNodes_) {
        return;
    }
    copyString(names_[id], 64, tableName);
    if (id >= nodeCount_) {
        nodeCount_ = id + 1;
    }
}

void Graph::addEdge(int src, int dst, float weight, const char* condition) {
    if (src < 0 || src >= maxNodes_ || dst < 0 || dst >= maxNodes_) {
        return;
    }

    adjMatrix_[src][dst] = weight;
    adjMatrix_[dst][src] = weight;

    for (int i = 0; i < edgeCount_; ++i) {
        if ((edgeList_[i].src == src && edgeList_[i].dst == dst) ||
            (edgeList_[i].src == dst && edgeList_[i].dst == src)) {
            edgeList_[i].weight = weight;
            copyString(edgeList_[i].joinCondition, 128, condition);
            return;
        }
    }

    if (edgeCount_ < maxEdges_) {
        edgeList_[edgeCount_].src = src;
        edgeList_[edgeCount_].dst = dst;
        edgeList_[edgeCount_].weight = weight;
        copyString(edgeList_[edgeCount_].joinCondition, 128, condition);
        edgeCount_++;
    }
}

int Graph::getNodeCount() const {
    return nodeCount_;
}

int Graph::getEdgeCount() const {
    return edgeCount_;
}

GraphEdge* Graph::getEdges() const {
    return edgeList_;
}

const char* Graph::getNodeName(int id) const {
    if (id < 0 || id >= maxNodes_) {
        return nullptr;
    }
    return names_[id];
}

void Graph::printGraph() const {
    std::cout << "Adjacency Matrix:" << std::endl;
    std::cout << "        ";
    for (int j = 0; j < nodeCount_; ++j) {
        std::cout << names_[j];
        for (int k = 0; k < 10 - strLen(names_[j]); ++k) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;

    for (int i = 0; i < nodeCount_; ++i) {
        std::cout << names_[i];
        for (int k = 0; k < 8 - strLen(names_[i]); ++k) {
            std::cout << " ";
        }
        for (int j = 0; j < nodeCount_; ++j) {
            std::cout << "  " << adjMatrix_[i][j];
        }
        std::cout << std::endl;
    }

    std::cout << "Edges:" << std::endl;
    for (int i = 0; i < edgeCount_; ++i) {
        std::cout << "  " << names_[edgeList_[i].src]
                  << " -- " << names_[edgeList_[i].dst]
                  << "  weight=" << edgeList_[i].weight
                  << "  condition=" << edgeList_[i].joinCondition << std::endl;
    }
}

void Graph::registerDefaultEdges() {
    addNode(0, "customer");
    addNode(1, "orders");
    addNode(2, "lineitem");

    addEdge(0, 1, 2.0f, "orders.o_custkey = customer.c_custkey");
    addEdge(1, 2, 3.0f, "lineitem.l_orderkey = orders.o_orderkey");
    addEdge(0, 2, 8.0f, "high cost -- no direct key");
}

} // namespace NanoDB
