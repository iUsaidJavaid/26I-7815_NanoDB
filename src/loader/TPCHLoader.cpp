#include "loader/TPCHLoader.h"
#include <cstring>
#include <cstdlib>

namespace NanoDB {

TPCHLoader::TPCHLoader() {
}

TPCHLoader::~TPCHLoader() {
}

int TPCHLoader::parseInt(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    while (str[i] == ' ') {
        ++i;
    }
    
    if (str[i] == '-') {
        sign = -1;
        ++i;
    }
    
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        ++i;
    }
    
    return result * sign;
}

float TPCHLoader::parseFloat(const char* str) {
    float result = 0.0f;
    float decimal = 0.1f;
    int i = 0;
    int sign = 1;
    
    while (str[i] == ' ') {
        ++i;
    }
    
    if (str[i] == '-') {
        sign = -1;
        ++i;
    }
    
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10.0f + (str[i] - '0');
        ++i;
    }
    
    if (str[i] == '.') {
        ++i;
        while (str[i] >= '0' && str[i] <= '9') {
            result = result + (str[i] - '0') * decimal;
            decimal *= 0.1f;
            ++i;
        }
    }
    
    return result * sign;
}

void TPCHLoader::trimWhitespace(char* str) {
    if (str == nullptr) {
        return;
    }
    
    int len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    
    int start = 0;
    while (start < len && str[start] == ' ') {
        ++start;
    }
    
    int end = len - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\n' || str[end] == '\r')) {
        --end;
    }
    
    if (start > 0) {
        int j = 0;
        for (int i = start; i <= end; ++i) {
            str[j++] = str[i];
        }
        str[j] = '\0';
    } else {
        str[end + 1] = '\0';
    }
}

void TPCHLoader::splitLine(char* line, char** fields, int& fieldCount) {
    fieldCount = 0;
    char* token = line;
    
    for (int i = 0; line[i] != '\0' && fieldCount < MAX_FIELDS; ++i) {
        if (line[i] == '|') {
            line[i] = '\0';
            fields[fieldCount++] = token;
            token = line + i + 1;
        }
    }
    
    if (fieldCount < MAX_FIELDS && *token != '\0') {
        fields[fieldCount++] = token;
    }
    
    for (int i = 0; i < fieldCount; ++i) {
        trimWhitespace(fields[i]);
    }
}

void TPCHLoader::loadCustomers(const char* filePath, QueryExecutor& executor) {
    FILE* fp = fopen(filePath, "r");
    if (fp == nullptr) {
        printf("[ERROR] Failed to open file: %s\n", filePath);
        return;
    }
    
    printf("[TPCHLoader] Loading customers from: %s\n", filePath);
    
    char line[BUFFER_SIZE];
    char* fields[MAX_FIELDS];
    int fieldCount = 0;
    int rowCount = 0;
    
    while (fgets(line, BUFFER_SIZE, fp) != nullptr) {
        splitLine(line, fields, fieldCount);
        
        if (fieldCount < 5) {
            continue;
        }
        
        int c_custkey = parseInt(fields[0]);
        const char* c_name = fields[1];
        float c_acctbal = parseFloat(fields[2]);
        const char* c_mktsegment = fields[3];
        int c_nationkey = parseInt(fields[4]);
        
        Field* values[5];
        values[0] = new IntField(c_custkey);
        values[1] = new StringField(c_name);
        values[2] = new FloatField(c_acctbal);
        values[3] = new StringField(c_mktsegment);
        values[4] = new IntField(c_nationkey);
        
        executor.executeInsert("customer", values, 5);
        
        delete values[0];
        delete values[1];
        delete values[2];
        delete values[3];
        delete values[4];
        
        ++rowCount;
        
        if (rowCount % 1000 == 0) {
            printf("[TPCHLoader] Loaded %d rows into customer\n", rowCount);
        }
    }
    
    fclose(fp);
    printf("[TPCHLoader] Completed loading %d rows into customer\n", rowCount);
}

void TPCHLoader::loadOrders(const char* filePath, QueryExecutor& executor) {
    FILE* fp = fopen(filePath, "r");
    if (fp == nullptr) {
        printf("[ERROR] Failed to open file: %s\n", filePath);
        return;
    }
    
    printf("[TPCHLoader] Loading orders from: %s\n", filePath);
    
    char line[BUFFER_SIZE];
    char* fields[MAX_FIELDS];
    int fieldCount = 0;
    int rowCount = 0;
    
    while (fgets(line, BUFFER_SIZE, fp) != nullptr) {
        splitLine(line, fields, fieldCount);
        
        if (fieldCount < 4) {
            continue;
        }
        
        int o_orderkey = parseInt(fields[0]);
        int o_custkey = parseInt(fields[1]);
        const char* o_orderstatus = fields[2];
        float o_totalprice = parseFloat(fields[3]);
        
        Field* values[4];
        values[0] = new IntField(o_orderkey);
        values[1] = new IntField(o_custkey);
        values[2] = new StringField(o_orderstatus);
        values[3] = new FloatField(o_totalprice);
        
        executor.executeInsert("orders", values, 4);
        
        delete values[0];
        delete values[1];
        delete values[2];
        delete values[3];
        
        ++rowCount;
        
        if (rowCount % 1000 == 0) {
            printf("[TPCHLoader] Loaded %d rows into orders\n", rowCount);
        }
    }
    
    fclose(fp);
    printf("[TPCHLoader] Completed loading %d rows into orders\n", rowCount);
}

void TPCHLoader::loadLineItems(const char* filePath, QueryExecutor& executor) {
    FILE* fp = fopen(filePath, "r");
    if (fp == nullptr) {
        printf("[ERROR] Failed to open file: %s\n", filePath);
        return;
    }
    
    printf("[TPCHLoader] Loading lineitems from: %s\n", filePath);
    
    char line[BUFFER_SIZE];
    char* fields[MAX_FIELDS];
    int fieldCount = 0;
    int rowCount = 0;
    
    while (fgets(line, BUFFER_SIZE, fp) != nullptr) {
        splitLine(line, fields, fieldCount);
        
        if (fieldCount < 4) {
            continue;
        }
        
        int l_orderkey = parseInt(fields[0]);
        int l_partkey = parseInt(fields[1]);
        float l_quantity = parseFloat(fields[2]);
        float l_extendedprice = parseFloat(fields[3]);
        
        Field* values[4];
        values[0] = new IntField(l_orderkey);
        values[1] = new IntField(l_partkey);
        values[2] = new FloatField(l_quantity);
        values[3] = new FloatField(l_extendedprice);
        
        executor.executeInsert("lineitem", values, 4);
        
        delete values[0];
        delete values[1];
        delete values[2];
        delete values[3];
        
        ++rowCount;
        
        if (rowCount % 1000 == 0) {
            printf("[TPCHLoader] Loaded %d rows into lineitem\n", rowCount);
        }
    }
    
    fclose(fp);
    printf("[TPCHLoader] Completed loading %d rows into lineitem\n", rowCount);
}

} // namespace NanoDB
