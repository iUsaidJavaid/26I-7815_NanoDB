#ifndef NANODB_LRU_CACHE_H
#define NANODB_LRU_CACHE_H

#include "storage/Page.h"
#include "storage/DoublyLinkedList.h"
#include "common/Logger.h"

namespace NanoDB {

class Pager;

struct CacheEntry {
    int pageId;
    Page* page;
    Node<int>* dllNode;
    CacheEntry* next;
    bool pinned;
    
    CacheEntry(int id, Page* p, Node<int>* node) 
        : pageId(id), page(p), dllNode(node), next(nullptr), pinned(false) {}
};

class LRUCache {
public:
    static const int HASH_TABLE_SIZE = 512;
    
    LRUCache(int capacity, Pager* pager);
    ~LRUCache();
    
    Page* getPage(int pageId);
    void putPage(Page* page);
    void pinPage(int pageId);
    void unpinPage(int pageId);
    int getEvictionCount() const;
    
    void clear();
    
private:
    int capacity_;
    int size_;
    int evictionCount_;
    int pageFaultCount_;
    Pager* pager_;
    
    DoublyLinkedList<int> lruList_;
    CacheEntry** hashTable_;
    
    int hash(int pageId) const;
    CacheEntry* findEntry(int pageId);
    void insertEntry(int pageId, Page* page, Node<int>* dllNode);
    void removeEntry(int pageId);
    void evictLRU();
};

} // namespace NanoDB

#endif // NANODB_LRU_CACHE_H
