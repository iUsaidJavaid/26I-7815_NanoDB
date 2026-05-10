#ifndef NANODB_QUERY_EXECUTOR_H
#define NANODB_QUERY_EXECUTOR_H

#include "common/Types.h"
#include "storage/Pager.h"
#include "catalog/SystemCatalog.h"
#include "index/IndexManager.h"
#include "optimizer/MST.h"
#include "engine/PriorityQueue.h"
#include "parser/Tokenizer.h"
#include "parser/ShuntingYard.h"
#include "parser/ExpressionEvaluator.h"

namespace NanoDB {

class QueryExecutor {
public:
    QueryExecutor(Pager& pager, SystemCatalog& catalog, IndexManager& indexManager,
                  MSTOptimizer& optimizer, PriorityQueue& queue);
    ~QueryExecutor();

    void execute(const char* queryString);
    void enqueueTask(QueryTask* task);
    QueryTask* dequeueTask();
    void executeTask(QueryTask* task);
    void executeSelect(const char* tableName, const char* whereClause,
                       const char* joinTable2, const char* joinTable3,
                       bool forceSequential = false, bool forceIndexed = false, bool isStress = false);
    void executeInsert(const char* tableName, Field** values, int valueCount);
    void executeUpdate(const char* tableName, const char* whereClause,
                       const char* setClause);
    void executeJoin(const char* t1, const char* t2, const char* t3,
                     const char* whereClause);

private:
    Pager& pager_;
    SystemCatalog& catalog_;
    IndexManager& indexManager_;
    MSTOptimizer& optimizer_;
    PriorityQueue& queue_;
    ShuntingYard shuntingYard_;
    ExpressionEvaluator evaluator_;

    int taskIdCounter_;

    bool startsWith(const char* str, const char* prefix) const;
    bool strEquals(const char* a, const char* b) const;
    void copyString(char* dest, int destSize, const char* src) const;
    int strLen(const char* s) const;
    void toUpper(char* s) const;
    bool parseWhereForIndexedColumn(const char* whereClause, char* outColumn,
                                    int columnSize, int* outValue) const;
    bool extractTableFromSelect(const char* query, char* tableName, int tableSize,
                                char* whereClause, int whereSize) const;
    bool extractTableFromInsert(const char* query, char* tableName, int tableSize) const;
    bool extractTableFromUpdate(const char* query, char* tableName, int tableSize,
                                char* setClause, int setSize,
                                char* whereClause, int whereSize) const;
    bool extractTablesFromJoin(const char* query, char* t1, int s1,
                               char* t2, int s2, char* t3, int s3,
                               char* whereClause, int whereSize) const;
};

} // namespace NanoDB

#endif // NANODB_QUERY_EXECUTOR_H
