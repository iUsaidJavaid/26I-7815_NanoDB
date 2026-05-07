#ifndef NANODB_LRU_CACHE_H
#define NANODB_LRU_CACHE_H

#include "storage/Page.h"

namespace NanoDB {

class LRUCache {
public:
    struct CacheNode {
        int key;
        Page* value;
        CacheNode* prev;
        CacheNode* next;
    };
    
    LRUCache(int capacity);
    ~LRUCache();
    
    Page* get(int key);
    void put(int key, Page* value);
    
    void clear();
    
private:
    int capacity_;
    int size_;
    CacheNode* head_;
    CacheNode* tail_;
    CacheNode** hash_table_;
    int hash_table_size_;
    
    void moveToHead(CacheNode* node);
    void removeNode(CacheNode* node);
    CacheNode* addNodeToHead(int key, Page* value);
    void removeTail();
};

} // namespace NanoDB

#endif // NANODB_LRU_CACHE_H
