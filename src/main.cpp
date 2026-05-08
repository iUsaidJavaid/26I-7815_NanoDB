#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include "common/Types.h"
#include "common/Logger.h"
#include "common/MemTracker.h"
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
#include "loader/TPCHLoader.h"
#include "benchmark/Benchmark.h"

using namespace NanoDB;

// Global pointers for signal handler
static Pager* g_pager = nullptr;
static SystemCatalog* g_catalog = nullptr;
static Logger* g_logger = nullptr;
static volatile sig_atomic_t g_shutdownRequested = 0;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        printf("\n[SIGNAL] SIGINT received. Shutting down gracefully...\n");
        g_shutdownRequested = 1;
        
        if (g_pager != nullptr) {
            printf("[SIGNAL] Flushing all pages to disk...\n");
            g_pager->flushAll();
        }
        
        if (g_catalog != nullptr) {
            printf("[SIGNAL] Saving catalog to disk...\n");
            g_catalog->saveToDisk("data/catalog.bin");
        }
        
        if (g_logger != nullptr) {
            printf("[SIGNAL] Closing logger...\n");
            g_logger->shutdown();
        }
        
        printf("[SIGNAL] Shutdown complete.\n");
        exit(0);
    }
}

void printBanner(const char* version, int poolSize, int totalPages) {
    printf("=================================================\n");
    printf("           NanoDB - Mini Database Engine\n");
    printf("=================================================\n");
    printf("Version: %s\n", version);
    printf("Buffer Pool Size: %d pages (%.2f MB)\n", poolSize, (poolSize * 4.0) / 1024.0);
    printf("Total Pages Loaded: %d\n", totalPages);
    printf("=================================================\n\n");
}

bool strEquals(const char* a, const char* b) {
    if (a == nullptr || b == nullptr) {
        return false;
    }
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }
        ++i;
    }
    return a[i] == '\0' && b[i] == '\0';
}

int main(int argc, char* argv[]) {
    // Install signal handler
    signal(SIGINT, signalHandler);
    
    // Determine pool size (default 200, 50 for stress test)
    int poolSize = 200;
    bool stressTestMode = false;
    
    for (int i = 1; i < argc; ++i) {
        if (strEquals(argv[i], "--stress")) {
            poolSize = 50;
            stressTestMode = true;
        }
    }
    
    // Step 1: Initialize Logger
    Logger* logger = Logger::getInstance();
    logger->initialize();
    g_logger = logger;
    logger->logInfo("NanoDB initializing...");
    
    // Step 2: Initialize SystemCatalog
    SystemCatalog* catalog = SystemCatalog::getInstance();
    g_catalog = catalog;
    
    // Try to load from disk, if fails, initialize fresh
    catalog->loadFromDisk("data/catalog.bin");
    logger->logInfo("SystemCatalog loaded from disk");
    
    // Step 3: Initialize Pager
    Pager pager("data/nanodb.db", poolSize);
    g_pager = &pager;
    logger->logInfo("Pager initialized with pool size");
    
    // Print startup banner
    printBanner("1.0.0", poolSize, pager.getTotalPages());
    
    // Step 4-5: Create indexes
    IndexManager& indexManager = IndexManager::getInstance();
    indexManager.createIndex("customer", "c_custkey");
    indexManager.createIndex("orders", "o_orderkey");
    logger->logInfo("Indexes created for customer.c_custkey and orders.o_orderkey");
    
    // Step 6: Initialize Graph with TPC-H nodes
    Graph graph(10);
    graph.addNode(0, "customer");
    graph.addNode(1, "orders");
    graph.addNode(2, "lineitem");
    graph.addEdge(0, 1, 1000.0f, "c_custkey = o_custkey");
    graph.addEdge(1, 2, 1500.0f, "o_orderkey = l_orderkey");
    logger->logInfo("Query graph initialized with TPC-H tables");
    
    // Step 7: Initialize Optimizer and PriorityQueue
    MSTOptimizer optimizer;
    PriorityQueue queue(1000);
    logger->logInfo("MST Optimizer and PriorityQueue initialized");
    
    // Step 8: Initialize QueryExecutor
    QueryExecutor executor(pager, *catalog, indexManager, optimizer, queue);
    logger->logInfo("QueryExecutor initialized");
    
    // Step 9: Check if data needs to be loaded
    TableSchema* customerTable = catalog->getTable("customer");
    bool dataLoaded = (customerTable != nullptr && customerTable->totalRows > 0);
    
    if (!dataLoaded) {
        printf("[INIT] No data found. Loading TPC-H dataset...\n");
        logger->logInfo("Loading TPC-H dataset");
        
        TPCHLoader loader;
        loader.loadCustomers("data/customer.tbl", executor);
        loader.loadOrders("data/orders.tbl", executor);
        loader.loadLineItems("data/lineitem.tbl", executor);
        
        printf("[INIT] TPC-H dataset loaded successfully.\n");
        logger->logInfo("TPC-H dataset loaded");
    } else {
        printf("[INIT] Data already loaded (%d rows in customer table).\n", customerTable->totalRows);
        logger->logInfo("Data already loaded, skipping TPCHLoader");
    }
    
    // Step 10: Check for benchmark mode
    if (argc > 1 && strEquals(argv[1], "--bench")) {
        printf("[MODE] Benchmark mode activated.\n");
        logger->logInfo("Benchmark mode started");
        
        BenchmarkRunner benchmark;
        
        // Insertion benchmark
        int insertCounts[] = {100, 500, 1000, 5000, 10000};
        int insertCount = 5;
        benchmark.runInsertionBenchmark(executor, insertCounts, insertCount);
        
        // Scan comparison benchmark
        int scanSizes[] = {100, 500, 1000, 5000, 10000};
        int scanCount = 5;
        benchmark.runIndexedVsSequentialScan(indexManager, pager, scanSizes, scanCount);
        
        // LRU stress test
        benchmark.runLRUStressTest(pager, poolSize, 1000);
        
        // Write results
        benchmark.writeCSV("benchmark_results.csv");
        benchmark.printSummary();
        
        logger->logInfo("Benchmark completed");
    } else {
        // Step 11: Run test runner logic (read queries.txt)
        printf("[MODE] Query execution mode. Reading queries from queries.txt...\n");
        logger->logInfo("Reading queries from queries.txt");
        
        FILE* queryFile = fopen("queries.txt", "r");
        if (queryFile == nullptr) {
            printf("[ERROR] Could not open queries.txt\n");
            logger->logError("Failed to open queries.txt");
        } else {
            char line[1024];
            int queryNum = 0;
            
            while (fgets(line, sizeof(line), queryFile) != nullptr && !g_shutdownRequested) {
                // Remove trailing newline
                int len = 0;
                while (line[len] != '\0' && line[len] != '\n') {
                    ++len;
                }
                line[len] = '\0';
                
                if (len == 0) {
                    continue;
                }
                
                ++queryNum;
                printf("[QUERY %d] %s\n", queryNum, line);
                
                // Check for SHUTDOWN command
                if (strEquals(line, "SHUTDOWN")) {
                    printf("[QUERY] SHUTDOWN command received.\n");
                    logger->logInfo("SHUTDOWN command received");
                    break;
                }
                
                // Execute query
                executor.execute(line);
            }
            
            fclose(queryFile);
            printf("[QUERY] Processed %d queries.\n", queryNum);
            logger->logInfo("Query processing complete");
        }
    }
    
    // Step 12: Cleanup on exit
    printf("[EXIT] Shutting down NanoDB...\n");
    logger->logInfo("NanoDB shutting down");
    
    pager.flushAll();
    catalog->saveToDisk("data/catalog.bin");
    logger->shutdown();
    
    SystemCatalog::destroy();
    Logger::destroy();
    
    // Report memory usage in debug mode
    MemTracker::report();
    
    printf("[EXIT] NanoDB shutdown complete.\n");
    
    return 0;
}
