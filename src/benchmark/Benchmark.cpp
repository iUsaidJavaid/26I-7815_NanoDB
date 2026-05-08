#include "benchmark/Benchmark.h"
#include "common/Types.h"
#include <cstring>

namespace NanoDB {

BenchmarkRunner::BenchmarkRunner() : resultCount(0) {
}

BenchmarkRunner::~BenchmarkRunner() {
}

void BenchmarkRunner::addResult(const char* operation, int size, double timeMs) {
    if (resultCount < MAX_RESULTS) {
        int i = 0;
        while (operation[i] != '\0' && i < 63) {
            results[resultCount].operation[i] = operation[i];
            ++i;
        }
        results[resultCount].operation[i] = '\0';
        results[resultCount].size = size;
        results[resultCount].timeMs = timeMs;
        ++resultCount;
    }
}

double BenchmarkRunner::measureInsertion(QueryExecutor& exec, int count) {
    clock_t start = clock();
    
    for (int i = 1; i <= count; ++i) {
        Field* values[5];
        values[0] = new IntField(i);
        values[1] = new StringField("BenchmarkCustomer");
        values[2] = new FloatField(1000.0f);
        values[3] = new StringField("BUILDING");
        values[4] = new IntField(1);
        
        exec.executeInsert("customer", values, 5);
        
        delete values[0];
        delete values[1];
        delete values[2];
        delete values[3];
        delete values[4];
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    return elapsed;
}

double BenchmarkRunner::measureIndexedScan(IndexManager& idx, int size) {
    clock_t start = clock();
    
    for (int i = 1; i <= size; ++i) {
        idx.lookupPage("customer", "c_custkey", i);
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    return elapsed;
}

double BenchmarkRunner::measureSequentialScan(Pager& pager, int size) {
    clock_t start = clock();
    
    for (int i = 1; i <= size; ++i) {
        pager.getPage(i);
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    return elapsed;
}

void BenchmarkRunner::runInsertionBenchmark(QueryExecutor& exec, int recordCounts[], int count) {
    printf("[Benchmark] Running insertion benchmark...\n");
    
    for (int i = 0; i < count; ++i) {
        printf("[Benchmark] Testing insertion of %d records...\n", recordCounts[i]);
        
        double timeMs = measureInsertion(exec, recordCounts[i]);
        addResult("insertion", recordCounts[i], timeMs);
        
        printf("[Benchmark] Inserted %d records in %.2f ms\n", recordCounts[i], timeMs);
    }
}

void BenchmarkRunner::runIndexedVsSequentialScan(IndexManager& idx, Pager& pager, int sizes[], int count) {
    printf("[Benchmark] Running indexed vs sequential scan benchmark...\n");
    
    for (int i = 0; i < count; ++i) {
        printf("[Benchmark] Testing scan of %d records...\n", sizes[i]);
        
        double indexedTime = measureIndexedScan(idx, sizes[i]);
        addResult("indexed_scan", sizes[i], indexedTime);
        printf("[Benchmark] Indexed scan of %d records: %.2f ms\n", sizes[i], indexedTime);
        
        double sequentialTime = measureSequentialScan(pager, sizes[i]);
        addResult("sequential_scan", sizes[i], sequentialTime);
        printf("[Benchmark] Sequential scan of %d records: %.2f ms\n", sizes[i], sequentialTime);
        
        double speedup = sequentialTime / indexedTime;
        printf("[Benchmark] Speedup: %.2fx\n", speedup);
    }
}

void BenchmarkRunner::runLRUStressTest(Pager& pager, int poolSize, int recordsToScan) {
    printf("[Benchmark] Running LRU stress test...\n");
    printf("[Benchmark] Pool size: %d, Records to scan: %d\n", poolSize, recordsToScan);
    
    clock_t start = clock();
    
    for (int i = 1; i <= recordsToScan; ++i) {
        pager.getPage(i);
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    
    int pageFaults = pager.getPageFaultCount();
    int evictions = pager.getLRUEvictionCount();
    
    printf("[Benchmark] Scanned %d records in %.2f ms\n", recordsToScan, elapsed);
    printf("[Benchmark] Page faults: %d\n", pageFaults);
    printf("[Benchmark] LRU evictions: %d\n", evictions);
    
    double faultRate = ((double)pageFaults / recordsToScan) * 100.0;
    printf("[Benchmark] Page fault rate: %.2f%%\n", faultRate);
    
    addResult("lru_scan", recordsToScan, elapsed);
}

void BenchmarkRunner::writeCSV(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == nullptr) {
        printf("[ERROR] Failed to open %s for writing\n", filename);
        return;
    }
    
    fprintf(fp, "operation,size,time_ms\n");
    
    for (int i = 0; i < resultCount; ++i) {
        fprintf(fp, "%s,%d,%.2f\n", 
                results[i].operation, 
                results[i].size, 
                results[i].timeMs);
    }
    
    fclose(fp);
    printf("[Benchmark] Results written to %s\n", filename);
}

void BenchmarkRunner::printSummary() {
    printf("\n[Benchmark] =============================\n");
    printf("[Benchmark] Benchmark Summary\n");
    printf("[Benchmark] =============================\n");
    printf("[Benchmark] %-20s %-10s %-10s\n", "Operation", "Size", "Time(ms)");
    printf("[Benchmark] %-20s %-10s %-10s\n", "---------", "----", "--------");
    
    for (int i = 0; i < resultCount; ++i) {
        printf("[Benchmark] %-20s %-10d %-10.2f\n", 
               results[i].operation, 
               results[i].size, 
               results[i].timeMs);
    }
    
    printf("[Benchmark] =============================\n");
}

} // namespace NanoDB
