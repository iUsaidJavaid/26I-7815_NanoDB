#ifndef NANODB_PAGER_H
#define NANODB_PAGER_H

#include <cstdio>
#include "storage/Page.h"
#include "storage/LRUCache.h"

namespace NanoDB {

class Pager {
public:
    static const int MAX_PAGES = 1024;
    
    Pager(const char* filePath, int poolSize);
    ~Pager();
    
    Page* fetchPage(int pageId);
    Page* getPage(int pageId) { return fetchPage(pageId); }
    void writePage(int pageId);
    Page* allocateNewPage();
    void flushAll();
    int getTotalPages() const;
    int getNumPages() const { return getTotalPages(); }
    void close();
    int getPageFaultCount() const;
    int getLRUEvictionCount() const;
    
private:
    char filePath_[256];
    FILE* file_;
    int poolSize_;
    int totalPages_;
    Page* pagePool_;
    LRUCache* lruCache_;
    
    void readSuperblock();
    void writeSuperblock();
};

} // namespace NanoDB

#endif // NANODB_PAGER_H
