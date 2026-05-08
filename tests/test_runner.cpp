#include <iostream>
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

void runTests() {
    std::cout << "Running NanoDB Tests..." << std::endl;
    testAVLTree();
}

int main() {
    runTests();
    return 0;
}
