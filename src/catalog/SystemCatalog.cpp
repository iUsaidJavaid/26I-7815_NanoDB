#include "catalog/SystemCatalog.h"
#include <cstring>

namespace NanoDB {

SystemCatalog* SystemCatalog::instance_ = nullptr;

SystemCatalog::SystemCatalog() {
    tableMap_ = new HashMap<char*, TableSchema*>(256);
    initializeTPCHTables();
}

SystemCatalog::~SystemCatalog() {
    if (tableMap_ != nullptr) {
        // Manually delete char* keys and TableSchema* values before deleting HashMap
        tableMap_->forEach([](char* key, TableSchema* value) {
            delete[] key;      // Delete the char* key
            delete value;      // Delete TableSchema*
        });
        delete tableMap_;
    }
}

SystemCatalog* SystemCatalog::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SystemCatalog();
    }
    return instance_;
}

void SystemCatalog::destroy() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void SystemCatalog::registerTable(TableSchema* schema) {
    if (schema == nullptr) {
        return;
    }
    
    char* tableNameCopy = new char[64];
    int i = 0;
    while (schema->tableName[i] != '\0' && i < 63) {
        tableNameCopy[i] = schema->tableName[i];
        ++i;
    }
    tableNameCopy[i] = '\0';
    
    tableMap_->insert(tableNameCopy, schema);
}

TableSchema* SystemCatalog::getTable(const char* name) {
    TableSchema** result = tableMap_->get((char*)name);
    if (result == nullptr) {
        return nullptr;
    }
    return *result;
}

bool SystemCatalog::tableExists(const char* name) const {
    return tableMap_->contains((char*)name);
}

void SystemCatalog::loadFromDisk(const char* catalogFile) {
    FILE* f = fopen(catalogFile, "rb");
    if (f == nullptr) {
        return;
    }
    
    int tableCount;
    fread(&tableCount, sizeof(int), 1, f);
    
    for (int i = 0; i < tableCount; ++i) {
        TableSchema* schema = new TableSchema();
        
        fread(schema->tableName, sizeof(char), 64, f);
        fread(schema->filePath, sizeof(char), 256, f);
        fread(&schema->columnCount, sizeof(int), 1, f);
        fread(&schema->rootPageId, sizeof(int), 1, f);
        fread(&schema->totalRows, sizeof(int), 1, f);
        
        for (int j = 0; j < schema->columnCount; ++j) {
            fread(schema->columns[j].name, sizeof(char), 64, f);
            int typeInt;
            fread(&typeInt, sizeof(int), 1, f);
            schema->columns[j].type = (DataType)typeInt;
            fread(&schema->columns[j].maxLen, sizeof(int), 1, f);
        }
        
        registerTable(schema);
    }
    
    fclose(f);
}

void SystemCatalog::saveToDisk(const char* catalogFile) {
    FILE* f = fopen(catalogFile, "wb");
    if (f == nullptr) {
        return;
    }
    
    int tableCount = tableMap_->size();
    fwrite(&tableCount, sizeof(int), 1, f);
    
    tableMap_->forEach([f](char* key, TableSchema* schema) {
        fwrite(schema->tableName, sizeof(char), 64, f);
        fwrite(schema->filePath, sizeof(char), 256, f);
        fwrite(&schema->columnCount, sizeof(int), 1, f);
        fwrite(&schema->rootPageId, sizeof(int), 1, f);
        fwrite(&schema->totalRows, sizeof(int), 1, f);
        
        for (int j = 0; j < schema->columnCount; ++j) {
            fwrite(schema->columns[j].name, sizeof(char), 64, f);
            int typeInt = (int)schema->columns[j].type;
            fwrite(&typeInt, sizeof(int), 1, f);
            fwrite(&schema->columns[j].maxLen, sizeof(int), 1, f);
        }
    });
    
    fclose(f);
}

void SystemCatalog::printAll() const {
    tableMap_->forEach([](char* key, TableSchema* schema) {
        printf("Table: %s\n", schema->tableName);
        printf("  File: %s\n", schema->filePath);
        printf("  Columns: %d\n", schema->columnCount);
        printf("  Root Page: %d\n", schema->rootPageId);
        printf("  Total Rows: %d\n", schema->totalRows);
        for (int j = 0; j < schema->columnCount; ++j) {
            printf("    %s: ", schema->columns[j].name);
            if (schema->columns[j].type == DataType::INT) {
                printf("INT\n");
            } else if (schema->columns[j].type == DataType::FLOAT) {
                printf("FLOAT\n");
            } else {
                printf("VARCHAR(%d)\n", schema->columns[j].maxLen);
            }
        }
    });
}

void SystemCatalog::initializeTPCHTables() {
    TableSchema* customer = new TableSchema();
    int i = 0;
    const char* customerName = "customer";
    while (customerName[i] != '\0') {
        customer->tableName[i] = customerName[i];
        ++i;
    }
    customer->tableName[i] = '\0';
    
    i = 0;
    const char* customerPath = "data/customer.tbl";
    while (customerPath[i] != '\0') {
        customer->filePath[i] = customerPath[i];
        ++i;
    }
    customer->filePath[i] = '\0';
    
    customer->columnCount = 5;
    
    i = 0;
    const char* col1 = "c_custkey";
    while (col1[i] != '\0') {
        customer->columns[0].name[i] = col1[i];
        ++i;
    }
    customer->columns[0].name[i] = '\0';
    customer->columns[0].type = DataType::INT;
    customer->columns[0].maxLen = 0;
    
    i = 0;
    const char* col2 = "c_name";
    while (col2[i] != '\0') {
        customer->columns[1].name[i] = col2[i];
        ++i;
    }
    customer->columns[1].name[i] = '\0';
    customer->columns[1].type = DataType::VARCHAR;
    customer->columns[1].maxLen = 100;
    
    i = 0;
    const char* col3 = "c_acctbal";
    while (col3[i] != '\0') {
        customer->columns[2].name[i] = col3[i];
        ++i;
    }
    customer->columns[2].name[i] = '\0';
    customer->columns[2].type = DataType::FLOAT;
    customer->columns[2].maxLen = 0;
    
    i = 0;
    const char* col4 = "c_mktsegment";
    while (col4[i] != '\0') {
        customer->columns[3].name[i] = col4[i];
        ++i;
    }
    customer->columns[3].name[i] = '\0';
    customer->columns[3].type = DataType::VARCHAR;
    customer->columns[3].maxLen = 50;
    
    i = 0;
    const char* col5 = "c_nationkey";
    while (col5[i] != '\0') {
        customer->columns[4].name[i] = col5[i];
        ++i;
    }
    customer->columns[4].name[i] = '\0';
    customer->columns[4].type = DataType::INT;
    customer->columns[4].maxLen = 0;
    
    registerTable(customer);
    
    TableSchema* orders = new TableSchema();
    i = 0;
    const char* ordersName = "orders";
    while (ordersName[i] != '\0') {
        orders->tableName[i] = ordersName[i];
        ++i;
    }
    orders->tableName[i] = '\0';
    
    i = 0;
    const char* ordersPath = "data/orders.tbl";
    while (ordersPath[i] != '\0') {
        orders->filePath[i] = ordersPath[i];
        ++i;
    }
    orders->filePath[i] = '\0';
    
    orders->columnCount = 4;
    
    i = 0;
    const char* ocol1 = "o_orderkey";
    while (ocol1[i] != '\0') {
        orders->columns[0].name[i] = ocol1[i];
        ++i;
    }
    orders->columns[0].name[i] = '\0';
    orders->columns[0].type = DataType::INT;
    orders->columns[0].maxLen = 0;
    
    i = 0;
    const char* ocol2 = "o_custkey";
    while (ocol2[i] != '\0') {
        orders->columns[1].name[i] = ocol2[i];
        ++i;
    }
    orders->columns[1].name[i] = '\0';
    orders->columns[1].type = DataType::INT;
    orders->columns[1].maxLen = 0;
    
    i = 0;
    const char* ocol3 = "o_orderstatus";
    while (ocol3[i] != '\0') {
        orders->columns[2].name[i] = ocol3[i];
        ++i;
    }
    orders->columns[2].name[i] = '\0';
    orders->columns[2].type = DataType::VARCHAR;
    orders->columns[2].maxLen = 10;
    
    i = 0;
    const char* ocol4 = "o_totalprice";
    while (ocol4[i] != '\0') {
        orders->columns[3].name[i] = ocol4[i];
        ++i;
    }
    orders->columns[3].name[i] = '\0';
    orders->columns[3].type = DataType::FLOAT;
    orders->columns[3].maxLen = 0;
    
    registerTable(orders);
    
    TableSchema* lineitem = new TableSchema();
    i = 0;
    const char* lineitemName = "lineitem";
    while (lineitemName[i] != '\0') {
        lineitem->tableName[i] = lineitemName[i];
        ++i;
    }
    lineitem->tableName[i] = '\0';
    
    i = 0;
    const char* lineitemPath = "data/lineitem.tbl";
    while (lineitemPath[i] != '\0') {
        lineitem->filePath[i] = lineitemPath[i];
        ++i;
    }
    lineitem->filePath[i] = '\0';
    
    lineitem->columnCount = 4;
    
    i = 0;
    const char* lcol1 = "l_orderkey";
    while (lcol1[i] != '\0') {
        lineitem->columns[0].name[i] = lcol1[i];
        ++i;
    }
    lineitem->columns[0].name[i] = '\0';
    lineitem->columns[0].type = DataType::INT;
    lineitem->columns[0].maxLen = 0;
    
    i = 0;
    const char* lcol2 = "l_partkey";
    while (lcol2[i] != '\0') {
        lineitem->columns[1].name[i] = lcol2[i];
        ++i;
    }
    lineitem->columns[1].name[i] = '\0';
    lineitem->columns[1].type = DataType::INT;
    lineitem->columns[1].maxLen = 0;
    
    i = 0;
    const char* lcol3 = "l_quantity";
    while (lcol3[i] != '\0') {
        lineitem->columns[2].name[i] = lcol3[i];
        ++i;
    }
    lineitem->columns[2].name[i] = '\0';
    lineitem->columns[2].type = DataType::FLOAT;
    lineitem->columns[2].maxLen = 0;
    
    i = 0;
    const char* lcol4 = "l_extendedprice";
    while (lcol4[i] != '\0') {
        lineitem->columns[3].name[i] = lcol4[i];
        ++i;
    }
    lineitem->columns[3].name[i] = '\0';
    lineitem->columns[3].type = DataType::FLOAT;
    lineitem->columns[3].maxLen = 0;
    
    registerTable(lineitem);
}

} // namespace NanoDB
