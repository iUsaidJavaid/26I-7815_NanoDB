#ifndef NANODB_QUERY_EXECUTOR_H
#define NANODB_QUERY_EXECUTOR_H

#include "common/Types.h"

namespace NanoDB {

class QueryExecutor {
public:
    QueryExecutor();
    ~QueryExecutor();
    
    bool execute(const char* query);
    Value* getResult(int* num_rows);
    
    void reset();
    
private:
    Value* result_;
    int num_rows_;
    int num_columns_;
};

} // namespace NanoDB

#endif // NANODB_QUERY_EXECUTOR_H
