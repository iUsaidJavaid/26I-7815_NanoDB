#ifndef NANODB_TPCH_LOADER_H
#define NANODB_TPCH_LOADER_H

#include "common/Types.h"
#include "engine/QueryExecutor.h"
#include <cstdio>

namespace NanoDB {

class TPCHLoader {
public:
    TPCHLoader();
    ~TPCHLoader();

    void loadCustomers(const char* filePath, QueryExecutor& executor);
    void loadOrders(const char* filePath, QueryExecutor& executor);
    void loadLineItems(const char* filePath, QueryExecutor& executor);

private:
    static const int BUFFER_SIZE = 512;
    static const int MAX_FIELDS = 16;

    int parseInt(const char* str);
    float parseFloat(const char* str);
    void trimWhitespace(char* str);
    void splitLine(char* line, char** fields, int& fieldCount);
};

} // namespace NanoDB

#endif // NANODB_TPCH_LOADER_H
