#ifndef NANODB_PAGE_H
#define NANODB_PAGE_H

namespace NanoDB {

class Page {
public:
    static const int PAGE_SIZE = 4096;
    
    Page();
    ~Page();
    
    char* getData();
    const char* getData() const;
    
    int getPageNumber() const;
    void setPageNumber(int pageNum);
    
    bool isDirty() const;
    void setDirty(bool dirty);
    
    void clear();
    
private:
    char data_[PAGE_SIZE];
    int page_number_;
    bool dirty_;
};

} // namespace NanoDB

#endif // NANODB_PAGE_H
