#ifndef NANODB_SYSTEM_CATALOG_H
#define NANODB_SYSTEM_CATALOG_H

#include <cstdio>
#include "common/Types.h"
#include "catalog/HashMap.h"

namespace NanoDB {

struct TableSchema {
    char tableName[64];
    char filePath[256];
    ColumnSchema columns[16];
    int columnCount;
    int rootPageId;
    int totalRows;
    
    TableSchema() : columnCount(0), rootPageId(0), totalRows(0) {
        for (int i = 0; i < 64; ++i) {
            tableName[i] = '\0';
        }
        for (int i = 0; i < 256; ++i) {
            filePath[i] = '\0';
        }
    }
    
    int getColumnIndex(const char* name) const {
        for (int i = 0; i < columnCount; ++i) {
            int j = 0;
            bool match = true;
            while (name[j] != '\0' && columns[i].name[j] != '\0') {
                if (name[j] != columns[i].name[j]) {
                    match = false;
                    break;
                }
                ++j;
            }
            if (match && name[j] == '\0' && columns[i].name[j] == '\0') {
                return i;
            }
        }
        return -1;
    }
};

class SystemCatalog {
public:
    static SystemCatalog* getInstance();
    static void destroy();
    
    void registerTable(TableSchema* schema);
    TableSchema* getTable(const char* name);
    bool tableExists(const char* name) const;
    void loadFromDisk(const char* catalogFile);
    void saveToDisk(const char* catalogFile);
    void printAll() const;
    
private:
    SystemCatalog();
    ~SystemCatalog();
    
    SystemCatalog(const SystemCatalog&);
    SystemCatalog& operator=(const SystemCatalog&);
    
    void initializeTPCHTables();
    
    HashMap<char*, TableSchema*>* tableMap_;
    
    static SystemCatalog* instance_;
};

} // namespace NanoDB

#endif // NANODB_SYSTEM_CATALOG_H
