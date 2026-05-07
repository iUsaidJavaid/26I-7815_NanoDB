#ifndef NANODB_SYSTEM_CATALOG_H
#define NANODB_SYSTEM_CATALOG_H

namespace NanoDB {

class SystemCatalog {
public:
    struct TableInfo {
        int table_id;
        char name[64];
        int num_columns;
        int root_page;
    };
    
    struct ColumnInfo {
        int column_id;
        char name[64];
        int type;
        int table_id;
    };
    
    SystemCatalog();
    ~SystemCatalog();
    
    bool createTable(const char* name, int num_columns);
    bool dropTable(const char* name);
    
    TableInfo* getTable(const char* name);
    TableInfo* getTableById(int table_id);
    
    bool addColumn(int table_id, const char* name, int type);
    ColumnInfo* getColumn(int table_id, const char* name);
    
    void load();
    void save();
    
private:
    TableInfo* tables_;
    ColumnInfo* columns_;
    int num_tables_;
    int num_columns_;
    int max_tables_;
    int max_columns_;
};

} // namespace NanoDB

#endif // NANODB_SYSTEM_CATALOG_H
