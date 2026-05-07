#ifndef NANODB_PAGER_H
#define NANODB_PAGER_H

#include "storage/Page.h"

namespace NanoDB {

class Pager {
public:
    Pager(const char* filename);
    ~Pager();
    
    bool open();
    void close();
    
    Page* getPage(int page_num);
    void flushPage(int page_num);
    
    int getNumPages() const;
    
private:
    char* filename_;
    int file_descriptor_;
    int num_pages_;
};

} // namespace NanoDB

#endif // NANODB_PAGER_H
