#ifndef NANODB_BENCHMARK_H
#define NANODB_BENCHMARK_H

#include "engine/QueryExecutor.h"
#include "index/IndexManager.h"
#include "storage/Pager.h"
#include <cstdio>
#include <ctime>

namespace NanoDB {

struct BenchmarkResult {
    char operation[64];
    int size;
    double timeMs;
};

class BenchmarkRunner {
public:
    BenchmarkRunner();
    ~BenchmarkRunner();

    void runInsertionBenchmark(QueryExecutor& exec, int recordCounts[], int count);
    void runIndexedVsSequentialScan(IndexManager& idx, Pager& pager, int sizes[], int count);
    void runLRUStressTest(Pager& pager, int poolSize, int recordsToScan);
    void writeCSV(const char* filename);
    void printSummary();

private:
    static const int MAX_RESULTS = 100;
    BenchmarkResult results[MAX_RESULTS];
    int resultCount;
    
    void addResult(const char* operation, int size, double timeMs);
    double measureInsertion(QueryExecutor& exec, int count);
    double measureIndexedScan(IndexManager& idx, int size);
    double measureSequentialScan(Pager& pager, int size);
};

} // namespace NanoDB

#endif // NANODB_BENCHMARK_H
