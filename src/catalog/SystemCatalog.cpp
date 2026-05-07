#include "catalog/SystemCatalog.h"

namespace NanoDB {

SystemCatalog::SystemCatalog() 
    : tables_(nullptr), columns_(nullptr), num_tables_(0), 
      num_columns_(0), max_tables_(0), max_columns_(0) {
}

SystemCatalog::~SystemCatalog() {
}

bool SystemCatalog::createTable(const char* name, int num_columns) {
    return false;
}

bool SystemCatalog::dropTable(const char* name) {
    return false;
}

SystemCatalog::TableInfo* SystemCatalog::getTable(const char* name) {
    return nullptr;
}

SystemCatalog::TableInfo* SystemCatalog::getTableById(int table_id) {
    return nullptr;
}

bool SystemCatalog::addColumn(int table_id, const char* name, int type) {
    return false;
}

SystemCatalog::ColumnInfo* SystemCatalog::getColumn(int table_id, const char* name) {
    return nullptr;
}

void SystemCatalog::load() {
}

void SystemCatalog::save() {
}

} // namespace NanoDB
