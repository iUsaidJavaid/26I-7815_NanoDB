#include "engine/QueryExecutor.h"

namespace NanoDB {

QueryExecutor::QueryExecutor() 
    : result_(nullptr), num_rows_(0), num_columns_(0) {
}

QueryExecutor::~QueryExecutor() {
}

bool QueryExecutor::execute(const char* query) {
    return false;
}

Value* QueryExecutor::getResult(int* num_rows) {
    return nullptr;
}

void QueryExecutor::reset() {
}

} // namespace NanoDB
