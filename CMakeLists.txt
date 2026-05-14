cmake_minimum_required(VERSION 3.10)
project(NanoDB VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/common/Logger.cpp
    src/common/Types.cpp
    src/storage/Page.cpp
    src/storage/Pager.cpp
    src/storage/LRUCache.cpp
    src/catalog/SystemCatalog.cpp
    src/parser/Tokenizer.cpp
    src/parser/ShuntingYard.cpp
    src/parser/ExpressionEvaluator.cpp
    src/index/AVLTree.cpp
    src/index/IndexManager.cpp
    src/optimizer/Graph.cpp
    src/optimizer/MST.cpp
    src/engine/QueryExecutor.cpp
    src/engine/PriorityQueue.cpp
    src/loader/TPCHLoader.cpp
    src/benchmark/Benchmark.cpp
    src/main.cpp
)

# Create executable
add_executable(nanodb ${SOURCES})

# Test executable
set(TEST_SOURCES
    src/common/Logger.cpp
    src/common/Types.cpp
    src/storage/Page.cpp
    src/storage/Pager.cpp
    src/storage/LRUCache.cpp
    src/catalog/SystemCatalog.cpp
    src/parser/Tokenizer.cpp
    src/parser/ShuntingYard.cpp
    src/parser/ExpressionEvaluator.cpp
    src/index/AVLTree.cpp
    src/index/IndexManager.cpp
    src/optimizer/Graph.cpp
    src/optimizer/MST.cpp
    src/engine/QueryExecutor.cpp
    src/engine/PriorityQueue.cpp
    src/loader/TPCHLoader.cpp
    src/benchmark/Benchmark.cpp
    tests/test_runner.cpp
)

add_executable(test_runner ${TEST_SOURCES})
