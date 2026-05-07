#include "storage/Pager.h"
#include "common/Logger.h"

namespace NanoDB {

Pager::Pager(const char* filePath, int poolSize) 
    : file_(nullptr), poolSize_(poolSize), totalPages_(0), pagePool_(nullptr), lruCache_(nullptr) {
    int i = 0;
    while (filePath[i] != '\0' && i < 255) {
        filePath_[i] = filePath[i];
        ++i;
    }
    filePath_[i] = '\0';
    
    file_ = fopen(filePath, "rb+");
    if (file_ == nullptr) {
        file_ = fopen(filePath, "wb+");
    }
    
    if (file_ != nullptr) {
        readSuperblock();
    }
    
    pagePool_ = new Page[poolSize_];
    lruCache_ = new LRUCache(poolSize_, this);
}

Pager::~Pager() {
    close();
    if (pagePool_ != nullptr) {
        delete[] pagePool_;
    }
    if (lruCache_ != nullptr) {
        delete lruCache_;
    }
}

Page* Pager::fetchPage(int pageId) {
    if (pageId < 1 || pageId >= MAX_PAGES) {
        return nullptr;
    }
    
    Page* cachedPage = lruCache_->getPage(pageId);
    if (cachedPage != nullptr) {
        return cachedPage;
    }
    
    if (file_ == nullptr) {
        return nullptr;
    }
    
    int poolIndex = pageId % poolSize_;
    Page* page = &pagePool_[poolIndex];
    
    long offset = pageId * Page::PAGE_SIZE;
    fseek(file_, offset, SEEK_SET);
    
    page->deserialize(file_);
    page->pageId = pageId;
    page->isDirty = false;
    
    lruCache_->putPage(page);
    
    return page;
}

void Pager::writePage(int pageId) {
    Page* page = lruCache_->getPage(pageId);
    if (page == nullptr || file_ == nullptr) {
        return;
    }
    
    long offset = pageId * Page::PAGE_SIZE;
    fseek(file_, offset, SEEK_SET);
    
    page->serialize(file_);
    page->isDirty = false;
    
    fflush(file_);
}

Page* Pager::allocateNewPage() {
    int newPageId = totalPages_ + 1;
    if (newPageId >= MAX_PAGES) {
        return nullptr;
    }
    
    int poolIndex = newPageId % poolSize_;
    Page* page = &pagePool_[poolIndex];
    
    page->pageId = newPageId;
    page->clear();
    page->isDirty = true;
    
    lruCache_->putPage(page);
    
    ++totalPages_;
    writeSuperblock();
    
    return page;
}

void Pager::flushAll() {
    if (file_ == nullptr) {
        return;
    }
    
    for (int i = 1; i < MAX_PAGES; ++i) {
        Page* page = lruCache_->getPage(i);
        if (page != nullptr && page->isDirty) {
            writePage(i);
        }
    }
    
    writeSuperblock();
    fflush(file_);
}

int Pager::getTotalPages() const {
    return totalPages_;
}

void Pager::close() {
    if (file_ != nullptr) {
        flushAll();
        fclose(file_);
        file_ = nullptr;
    }
}

void Pager::readSuperblock() {
    if (file_ == nullptr) {
        return;
    }
    
    fseek(file_, 0, SEEK_SET);
    fread(&totalPages_, sizeof(int), 1, file_);
    
    if (totalPages_ < 1) {
        totalPages_ = 0;
    }
}

void Pager::writeSuperblock() {
    if (file_ == nullptr) {
        return;
    }
    
    fseek(file_, 0, SEEK_SET);
    fwrite(&totalPages_, sizeof(int), 1, file_);
    fflush(file_);
}

} // namespace NanoDB
