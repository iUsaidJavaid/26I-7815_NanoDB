#ifndef NANODB_INDEX_MANAGER_H
#define NANODB_INDEX_MANAGER_H

#include "index/AVLTree.h"
#include "storage/Pager.h"

namespace NanoDB {

class IndexManager {
public:
    static IndexManager& getInstance();

    void createIndex(const char* tableName, const char* columnName);
    void insertEntry(const char* tableName, const char* columnName, int key, int pageId);
    int lookupPage(const char* tableName, const char* columnName, int key);
    int* rangeLookup(const char* tableName, const char* columnName, int low, int high, int& count);
    void dropIndex(const char* tableName, const char* columnName);
    bool hasIndex(const char* tableName, const char* columnName) const;

private:
    IndexManager();
    ~IndexManager();
    IndexManager(const IndexManager&) = delete;
    IndexManager& operator=(const IndexManager&) = delete;

    struct HashNode {
        char* key;
        AVLTree* tree;
        HashNode* next;
    };

    static const int HASH_CAPACITY = 128;
    HashNode* buckets_[HASH_CAPACITY];
    int size_;

    unsigned int hashString(const char* str) const;
    char* buildIndexKey(const char* tableName, const char* columnName) const;
    AVLTree* getTree(const char* tableName, const char* columnName) const;
    static bool strEquals(const char* a, const char* b);
};

class SequentialScanner {
public:
    int scan(const char* tableName, int targetKey, Pager& pager);
};

} // namespace NanoDB

#endif // NANODB_INDEX_MANAGER_H
