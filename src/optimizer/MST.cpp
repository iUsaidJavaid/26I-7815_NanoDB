#include "optimizer/MST.h"
#include <cstdio>

namespace NanoDB {

UnionFind::UnionFind(int n) : size_(n) {
    parent_ = new int[n];
    rank_ = new int[n];
    for (int i = 0; i < n; ++i) {
        parent_[i] = i;
        rank_[i] = 0;
    }
}

UnionFind::~UnionFind() {
    delete[] parent_;
    delete[] rank_;
}

int UnionFind::find(int x) {
    if (parent_[x] != x) {
        parent_[x] = find(parent_[x]);
    }
    return parent_[x];
}

void UnionFind::unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    if (rootX == rootY) {
        return;
    }
    if (rank_[rootX] < rank_[rootY]) {
        parent_[rootX] = rootY;
    } else if (rank_[rootX] > rank_[rootY]) {
        parent_[rootY] = rootX;
    } else {
        parent_[rootY] = rootX;
        rank_[rootX]++;
    }
}

bool UnionFind::connected(int x, int y) {
    return find(x) == find(y);
}

void MSTOptimizer::insertionSort(GraphEdge* edges, int count) {
    for (int i = 1; i < count; ++i) {
        GraphEdge key = edges[i];
        int j = i - 1;
        while (j >= 0 && edges[j].weight > key.weight) {
            edges[j + 1] = edges[j];
            --j;
        }
        edges[j + 1] = key;
    }
}

GraphEdge* MSTOptimizer::computeMST(Graph& graph, int& edgeCount) {
    int totalEdges = graph.getEdgeCount();
    int nodes = graph.getNodeCount();
    if (totalEdges == 0 || nodes == 0) {
        edgeCount = 0;
        return nullptr;
    }

    GraphEdge* allEdges = graph.getEdges();
    GraphEdge* sortedEdges = new GraphEdge[totalEdges];
    for (int i = 0; i < totalEdges; ++i) {
        sortedEdges[i] = allEdges[i];
    }
    insertionSort(sortedEdges, totalEdges);

    UnionFind uf(nodes);
    GraphEdge* mst = new GraphEdge[nodes - 1];
    int mstCount = 0;

    for (int i = 0; i < totalEdges && mstCount < nodes - 1; ++i) {
        int u = sortedEdges[i].src;
        int v = sortedEdges[i].dst;
        if (!uf.connected(u, v)) {
            uf.unite(u, v);
            mst[mstCount] = sortedEdges[i];
            mstCount++;
        }
    }

    delete[] sortedEdges;
    edgeCount = mstCount;
    return mst;
}

char* MSTOptimizer::buildJoinPath(GraphEdge* mstEdges, int edgeCount, Graph& graph) {
    if (edgeCount == 0 || mstEdges == nullptr) {
        char* empty = new char[1];
        empty[0] = '\0';
        return empty;
    }

    int nodes = edgeCount + 1;
    int* degree = new int[nodes];
    int** adj = new int*[nodes];
    for (int i = 0; i < nodes; ++i) {
        degree[i] = 0;
        adj[i] = new int[nodes];
        for (int j = 0; j < nodes; ++j) {
            adj[i][j] = -1;
        }
    }

    for (int i = 0; i < edgeCount; ++i) {
        int u = mstEdges[i].src;
        int v = mstEdges[i].dst;
        adj[u][degree[u]++] = v;
        adj[v][degree[v]++] = u;
    }

    int start = 0;
    for (int i = 0; i < nodes; ++i) {
        if (degree[i] == 1) {
            start = i;
            break;
        }
    }

    int* path = new int[nodes];
    bool* visited = new bool[nodes];
    for (int i = 0; i < nodes; ++i) {
        visited[i] = false;
    }

    int pathLen = 0;
    int current = start;
    visited[current] = true;
    path[pathLen++] = current;

    while (pathLen < nodes) {
        for (int i = 0; i < degree[current]; ++i) {
            int neighbor = adj[current][i];
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                path[pathLen++] = neighbor;
                current = neighbor;
                break;
            }
        }
    }

    int totalLen = 0;
    for (int i = 0; i < pathLen; ++i) {
        const char* name = graph.getNodeName(path[i]);
        int len = 0;
        while (name[len] != '\0') {
            len++;
        }
        totalLen += len;
        if (i < pathLen - 1) {
            totalLen += 4;
        }
    }
    totalLen += 1;

    char* result = new char[totalLen];
    int pos = 0;
    for (int i = 0; i < pathLen; ++i) {
        const char* name = graph.getNodeName(path[i]);
        int j = 0;
        while (name[j] != '\0') {
            result[pos++] = name[j++];
        }
        if (i < pathLen - 1) {
            result[pos++] = ' ';
            result[pos++] = '-';
            result[pos++] = '>';
            result[pos++] = ' ';
        }
    }
    result[pos] = '\0';

    for (int i = 0; i < nodes; ++i) {
        delete[] adj[i];
    }
    delete[] adj;
    delete[] degree;
    delete[] path;
    delete[] visited;

    return result;
}

void MSTOptimizer::logMSTDecision(const char* path) {
    char msg[512];
    int pos = 0;
    const char* prefix = "[LOG] Multi-table join routed via MST: ";
    while (prefix[pos]) { msg[pos] = prefix[pos]; pos++; }
    int pi = 0;
    while (path[pi] && pos < 510) { msg[pos++] = path[pi++]; }
    msg[pos++] = '\n'; msg[pos] = '\0';
    Logger::getInstance()->logLog(msg);
}

float MSTOptimizer::computeTotalCost(GraphEdge* mstEdges, int edgeCount) const {
    float total = 0.0f;
    for (int i = 0; i < edgeCount; ++i) {
        total += mstEdges[i].weight;
    }
    return total;
}

Row** JoinExecutor::executeJoin(const char* table1, const char* table2, const char* table3,
                                Pager& pager, SystemCatalog& catalog, int& resultCount) {
    resultCount = 0;

    Graph graph(10);
    graph.registerDefaultEdges();
    MSTOptimizer optimizer;
    int mstEdgeCount = 0;
    GraphEdge* mst = optimizer.computeMST(graph, mstEdgeCount);

    if (mst != nullptr && mstEdgeCount > 0) {
        char* path = optimizer.buildJoinPath(mst, mstEdgeCount, graph);
        optimizer.logMSTDecision(path);
        delete[] path;
    }

    if (mst != nullptr) {
        delete[] mst;
    }

    return nullptr;
}

} // namespace NanoDB
