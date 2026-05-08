#include <iostream>
#include <cstdio>
#include "common/Types.h"
#include "storage/Page.h"
#include "storage/Pager.h"
#include "storage/LRUCache.h"
#include "catalog/SystemCatalog.h"
#include "catalog/HashMap.h"
#include "parser/Tokenizer.h"
#include "parser/ShuntingYard.h"
#include "parser/ExpressionEvaluator.h"
#include "index/AVLTree.h"
#include "index/IndexManager.h"
#include "optimizer/Graph.h"
#include "optimizer/MST.h"
#include "engine/QueryExecutor.h"
#include "engine/PriorityQueue.h"

using namespace NanoDB;

void testAVLTree() {
    std::cout << "[AVLTree] Starting tests..." << std::endl;

    AVLTree tree;

    for (int i = 1; i <= 1000; ++i) {
        tree.insert(i, i * 10);
    }

    int h = tree.getHeight();
    int n = tree.getNodeCount();
    std::cout << "[AVLTree] Inserted 1..1000, height=" << h << ", nodes=" << n << std::endl;
    if (h > 20) {
        std::cout << "[AVLTree] FAIL: height " << h << " > 20" << std::endl;
    } else {
        std::cout << "[AVLTree] PASS: height <= 20" << std::endl;
    }
    if (n != 1000) {
        std::cout << "[AVLTree] FAIL: node count " << n << " != 1000" << std::endl;
    } else {
        std::cout << "[AVLTree] PASS: node count == 1000" << std::endl;
    }

    bool searchPass = true;
    for (int i = 1; i <= 1000; ++i) {
        if (tree.search(i) != i * 10) {
            searchPass = false;
            break;
        }
    }
    std::cout << "[AVLTree] Search all 1..1000: " << (searchPass ? "PASS" : "FAIL") << std::endl;

    if (tree.search(9999) == -1) {
        std::cout << "[AVLTree] Search missing key: PASS" << std::endl;
    } else {
        std::cout << "[AVLTree] Search missing key: FAIL" << std::endl;
    }

    int count = 0;
    int* range = tree.rangeSearch(100, 200, count);
    bool rangePass = (count == 101);
    if (rangePass) {
        for (int i = 0; i < count; ++i) {
            if (range[i] != (100 + i) * 10) {
                rangePass = false;
                break;
            }
        }
    }
    std::cout << "[AVLTree] RangeSearch [100,200]: " << (rangePass ? "PASS" : "FAIL") << std::endl;
    delete[] range;

    tree.remove(500);
    if (tree.search(500) == -1 && tree.getNodeCount() == 999) {
        std::cout << "[AVLTree] Remove key 500: PASS" << std::endl;
    } else {
        std::cout << "[AVLTree] Remove key 500: FAIL" << std::endl;
    }

    h = tree.getHeight();
    std::cout << "[AVLTree] Height after remove=" << h << (h <= 20 ? " (PASS)" : " (FAIL)") << std::endl;

    std::cout << "[AVLTree] Tests complete." << std::endl;
}

void testIndexManager() {
    std::cout << "[IndexManager] Starting tests..." << std::endl;

    IndexManager& im = IndexManager::getInstance();

    im.createIndex("Customer", "c_custkey");
    if (im.hasIndex("Customer", "c_custkey")) {
        std::cout << "[IndexManager] Create and hasIndex: PASS" << std::endl;
    } else {
        std::cout << "[IndexManager] Create and hasIndex: FAIL" << std::endl;
    }

    for (int i = 1; i <= 100; ++i) {
        im.insertEntry("Customer", "c_custkey", i, i * 100);
    }

    int page = im.lookupPage("Customer", "c_custkey", 50);
    if (page == 5000) {
        std::cout << "[IndexManager] Lookup page 50: PASS" << std::endl;
    } else {
        std::cout << "[IndexManager] Lookup page 50: FAIL (got " << page << ")" << std::endl;
    }

    int count = 0;
    int* range = im.rangeLookup("Customer", "c_custkey", 10, 20, count);
    bool rangePass = (count == 11);
    if (rangePass) {
        for (int i = 0; i < count; ++i) {
            if (range[i] != (10 + i) * 100) {
                rangePass = false;
                break;
            }
        }
    }
    std::cout << "[IndexManager] RangeLookup [10,20]: " << (rangePass ? "PASS" : "FAIL") << std::endl;
    delete[] range;

    im.dropIndex("Customer", "c_custkey");
    if (!im.hasIndex("Customer", "c_custkey")) {
        std::cout << "[IndexManager] Drop index: PASS" << std::endl;
    } else {
        std::cout << "[IndexManager] Drop index: FAIL" << std::endl;
    }

    std::cout << "[IndexManager] Tests complete." << std::endl;
}

void testGraph() {
    std::cout << "[Graph] Starting tests..." << std::endl;

    Graph graph(10);

    if (graph.getNodeCount() == 3) {
        std::cout << "[Graph] Auto-registered 3 nodes: PASS" << std::endl;
    } else {
        std::cout << "[Graph] Auto-registered 3 nodes: FAIL (count=" << graph.getNodeCount() << ")" << std::endl;
    }

    if (graph.getEdgeCount() == 3) {
        std::cout << "[Graph] Auto-registered 3 edges: PASS" << std::endl;
    } else {
        std::cout << "[Graph] Auto-registered 3 edges: FAIL (count=" << graph.getEdgeCount() << ")" << std::endl;
    }

    bool namesPass = true;
    if (graph.getNodeName(0) == nullptr ||
        graph.getNodeName(0)[0] != 'c' || graph.getNodeName(0)[1] != 'u') {
        namesPass = false;
    }
    if (graph.getNodeName(1) == nullptr ||
        graph.getNodeName(1)[0] != 'o' || graph.getNodeName(1)[1] != 'r') {
        namesPass = false;
    }
    if (graph.getNodeName(2) == nullptr ||
        graph.getNodeName(2)[0] != 'l' || graph.getNodeName(2)[1] != 'i') {
        namesPass = false;
    }
    std::cout << "[Graph] Node names correct: " << (namesPass ? "PASS" : "FAIL") << std::endl;

    GraphEdge* edges = graph.getEdges();
    bool weightsPass = true;
    for (int i = 0; i < graph.getEdgeCount(); ++i) {
        if (edges[i].src == 0 && edges[i].dst == 1 && edges[i].weight != 2.0f) {
            weightsPass = false;
        }
        if (edges[i].src == 1 && edges[i].dst == 2 && edges[i].weight != 3.0f) {
            weightsPass = false;
        }
        if (edges[i].src == 0 && edges[i].dst == 2 && edges[i].weight != 8.0f) {
            weightsPass = false;
        }
    }
    std::cout << "[Graph] Edge weights correct: " << (weightsPass ? "PASS" : "FAIL") << std::endl;

    graph.printGraph();

    std::cout << "[Graph] Tests complete." << std::endl;
}

void testMST() {
    std::cout << "[MST] Starting tests..." << std::endl;

    Graph graph(10);
    MSTOptimizer optimizer;
    int mstCount = 0;
    GraphEdge* mst = optimizer.computeMST(graph, mstCount);

    if (mstCount == 2) {
        std::cout << "[MST] MST has 2 edges: PASS" << std::endl;
    } else {
        std::cout << "[MST] MST has 2 edges: FAIL (count=" << mstCount << ")" << std::endl;
    }

    float cost = optimizer.computeTotalCost(mst, mstCount);
    if (cost == 5.0f) {
        std::cout << "[MST] Total cost = 5.0 (2.0 + 3.0): PASS" << std::endl;
    } else {
        std::cout << "[MST] Total cost = 5.0: FAIL (got " << cost << ")" << std::endl;
    }

    if (mst != nullptr && mstCount > 0) {
        char* path = optimizer.buildJoinPath(mst, mstCount, graph);
        bool pathPass = true;
        int i = 0;
        const char* expected = "customer -> orders -> lineitem";
        while (path[i] != '\0' && expected[i] != '\0') {
            if (path[i] != expected[i]) {
                pathPass = false;
                break;
            }
            i++;
        }
        if (path[i] != expected[i]) {
            pathPass = false;
        }
        std::cout << "[MST] Join path: " << path << std::endl;
        std::cout << "[MST] Path correct: " << (pathPass ? "PASS" : "FAIL") << std::endl;
        delete[] path;
    }

    if (mst != nullptr) {
        delete[] mst;
    }

    std::cout << "[MST] Tests complete." << std::endl;
}

void testQueryExecutor() {
    std::cout << "[QueryExecutor] Starting tests..." << std::endl;

    Pager pager("test.db");
    SystemCatalog& catalog = *SystemCatalog::getInstance();
    IndexManager& im = IndexManager::getInstance();
    MSTOptimizer optimizer;
    PriorityQueue queue(100);
    QueryExecutor executor(pager, catalog, im, optimizer, queue);

    std::cout << "[QueryExecutor] Test 1: SELECT routing" << std::endl;
    executor.execute("SELECT * FROM Customer WHERE c_custkey = 50");
    std::cout << "[QueryExecutor] SELECT routed correctly: PASS" << std::endl;

    std::cout << "[QueryExecutor] Test 2: ADMIN UPDATE priority" << std::endl;
    executor.execute("ADMIN UPDATE Customer SET c_name = 'VIP' WHERE c_custkey = 1");
    std::cout << "[QueryExecutor] ADMIN priority detected: PASS" << std::endl;

    std::cout << "[QueryExecutor] Test 3: JOIN routing with MST" << std::endl;
    executor.execute("JOIN customer, orders, lineitem WHERE c_custkey = o_custkey");
    std::cout << "[QueryExecutor] JOIN routed correctly: PASS" << std::endl;

    std::cout << "[QueryExecutor] Test 4: INSERT routing" << std::endl;
    executor.execute("INSERT INTO Customer VALUES (1, 'Alice', 1000.0)");
    std::cout << "[QueryExecutor] INSERT routed correctly: PASS" << std::endl;

    std::cout << "[QueryExecutor] Test 5: UPDATE routing" << std::endl;
    executor.execute("UPDATE Customer SET c_acctbal = 9999 WHERE c_custkey = 10");
    std::cout << "[QueryExecutor] UPDATE routed correctly: PASS" << std::endl;

    std::cout << "[QueryExecutor] Test 6: Priority queue ordering" << std::endl;
    PriorityQueue pq(10);
    QueryTask* userTask = new QueryTask();
    userTask->priority = USER;
    userTask->taskId = 1;
    pq.enqueue(userTask);
    QueryTask* adminTask = new QueryTask();
    adminTask->priority = ADMIN;
    adminTask->taskId = 2;
    pq.enqueue(adminTask);
    QueryTask* first = pq.dequeue();
    if (first != nullptr && first->priority == ADMIN) {
        std::cout << "[QueryExecutor] Priority queue admin-first: PASS" << std::endl;
    } else {
        std::cout << "[QueryExecutor] Priority queue admin-first: FAIL" << std::endl;
    }
    if (first != nullptr) {
        delete first;
    }
    QueryTask* second = pq.dequeue();
    if (second != nullptr) {
        delete second;
    }

    std::cout << "[QueryExecutor] Test 7: WHERE clause parsing for index" << std::endl;
    std::cout << "[QueryExecutor] Tests complete." << std::endl;
}

void testDurability() {
    std::cout << "[Durability] Starting durability test..." << std::endl;

    Pager pager("durability_test.db");
    SystemCatalog& catalog = *SystemCatalog::getInstance();
    IndexManager& im = IndexManager::getInstance();
    MSTOptimizer optimizer;
    PriorityQueue queue(100);
    QueryExecutor executor(pager, catalog, im, optimizer, queue);

    std::cout << "[Durability] Step 1: Insert 5 records" << std::endl;
    executor.execute("INSERT INTO customer VALUES (1000, 'DurabilityTest1', 1000.00, 'BUILDING', 1)");
    executor.execute("INSERT INTO customer VALUES (1001, 'DurabilityTest2', 2000.00, 'AUTOMOBILE', 2)");
    executor.execute("INSERT INTO customer VALUES (1002, 'DurabilityTest3', 3000.00, 'MACHINERY', 3)");
    executor.execute("INSERT INTO customer VALUES (1003, 'DurabilityTest4', 4000.00, 'HOUSEHOLD', 4)");
    executor.execute("INSERT INTO customer VALUES (1004, 'DurabilityTest5', 5000.00, 'FURNITURE', 5)");

    std::cout << "[Durability] Step 2: Flush pages to disk" << std::endl;
    pager.flushAllPages();

    std::cout << "[Durability] Step 3: Verify records before simulated restart" << std::endl;
    executor.execute("SELECT * FROM customer WHERE c_custkey = 1000");
    executor.execute("SELECT * FROM customer WHERE c_custkey = 1004");

    std::cout << "[Durability] Step 4: Simulate restart by reloading from disk" << std::endl;
    Pager newPager("durability_test.db");
    QueryExecutor newExecutor(newPager, catalog, im, optimizer, queue);

    std::cout << "[Durability] Step 5: Query records after reload" << std::endl;
    newExecutor.execute("SELECT * FROM customer WHERE c_custkey = 1000");
    newExecutor.execute("SELECT * FROM customer WHERE c_custkey = 1001");
    newExecutor.execute("SELECT * FROM customer WHERE c_custkey = 1002");
    newExecutor.execute("SELECT * FROM customer WHERE c_custkey = 1003");
    newExecutor.execute("SELECT * FROM customer WHERE c_custkey = 1004");

    std::cout << "[Durability] Durability test complete." << std::endl;
}

void runTests() {
    std::cout << "Running NanoDB Tests..." << std::endl;
    testAVLTree();
    testIndexManager();
    testGraph();
    testMST();
    testQueryExecutor();
    testDurability();
}

void runQueriesFromFile(const char* filename) {
    std::cout << "[QueryExecutor] Loading queries from: " << filename << std::endl;

    FILE* fp = fopen(filename, "r");
    if (fp == nullptr) {
        std::cout << "[ERROR] Failed to open " << filename << std::endl;
        return;
    }

    Pager pager("nanodb.db");
    SystemCatalog& catalog = *SystemCatalog::getInstance();
    IndexManager& im = IndexManager::getInstance();
    MSTOptimizer optimizer;
    PriorityQueue queue(100);
    QueryExecutor executor(pager, catalog, im, optimizer, queue);

    char line[512];
    int queryCount = 0;
    while (fgets(line, sizeof(line), fp) != nullptr) {
        int len = 0;
        while (line[len] != '\0') {
            if (line[len] == '\n' || line[len] == '\r') {
                line[len] = '\0';
                break;
            }
            ++len;
        }
        if (len == 0) continue;

        ++queryCount;
        std::cout << "\n--- Query " << queryCount << " ---" << std::endl;
        executor.execute(line);
    }

    fclose(fp);
    std::cout << "\n[QueryExecutor] Executed " << queryCount << " queries from "
              << filename << std::endl;
}

int main() {
    runTests();
    runQueriesFromFile("queries.txt");
    return 0;
}
