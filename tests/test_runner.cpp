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

void runTests() {
    std::cout << "Running NanoDB Tests..." << std::endl;
    std::cout << "Test skeleton - no tests implemented yet." << std::endl;
}

int main() {
    runTests();
    return 0;
}
