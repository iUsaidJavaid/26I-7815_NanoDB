#include "storage/LRUCache.h"
#include "storage/Pager.h"

namespace NanoDB {

LRUCache::LRUCache(int capacity, Pager* pager) 
    : capacity_(capacity), size_(0), evictionCount_(0), pageFaultCount_(0), pager_(pager) {
    hashTable_ = new CacheEntry*[HASH_TABLE_SIZE];
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable_[i] = nullptr;
    }
}

LRUCache::~LRUCache() {
    clear();
    delete[] hashTable_;
}

Page* LRUCache::getPage(int pageId) {
    CacheEntry* entry = findEntry(pageId);
    
    if (entry == nullptr) {
        ++pageFaultCount_;
        
        char logMessage[128];
        int pos = 0;
        const char* prefix = "[LOG] Cache page-fault #";
        while (prefix[pos] != '\0') {
            logMessage[pos] = prefix[pos];
            ++pos;
        }
        
        char countStr[32];
        int countPos = 0;
        int tempCount = pageFaultCount_;
        if (tempCount == 0) {
            countStr[countPos++] = '0';
        } else {
            char countBuffer[32];
            int countBufPos = 0;
            while (tempCount > 0) {
                countBuffer[countBufPos++] = '0' + (tempCount % 10);
                tempCount /= 10;
            }
            for (int i = countBufPos - 1; i >= 0; --i) {
                countStr[countPos++] = countBuffer[i];
            }
        }
        countStr[countPos] = '\0';
        
        int countIdx = 0;
        while (countStr[countIdx] != '\0') {
            logMessage[pos++] = countStr[countIdx++];
        }
        
        const char* middle = ": Page ";
        int midIdx = 0;
        while (middle[midIdx] != '\0') {
            logMessage[pos++] = middle[midIdx++];
        }
        
        char idStr[32];
        int idPos = 0;
        int tempId = pageId;
        if (tempId == 0) {
            idStr[idPos++] = '0';
        } else {
            char idBuffer[32];
            int idBufPos = 0;
            while (tempId > 0) {
                idBuffer[idBufPos++] = '0' + (tempId % 10);
                tempId /= 10;
            }
            for (int i = idBufPos - 1; i >= 0; --i) {
                idStr[idPos++] = idBuffer[i];
            }
        }
        idStr[idPos] = '\0';
        
        int idIdx = 0;
        while (idStr[idIdx] != '\0') {
            logMessage[pos++] = idStr[idIdx++];
        }
        
        const char* suffix = " loaded from disk\n";
        int sufIdx = 0;
        while (suffix[sufIdx] != '\0') {
            logMessage[pos++] = suffix[sufIdx++];
        }
        logMessage[pos] = '\0';
        
        Logger::getInstance()->logLog(logMessage);
        
        return nullptr;
    }
    
    lruList_.moveToFront(entry->dllNode);
    return entry->page;
}

void LRUCache::putPage(Page* page) {
    if (page == nullptr) {
        return;
    }
    
    int pageId = page->pageId;
    
    CacheEntry* existing = findEntry(pageId);
    if (existing != nullptr) {
        lruList_.moveToFront(existing->dllNode);
        return;
    }
    
    if (size_ >= capacity_) {
        evictLRU();
    }
    
    lruList_.pushFront(pageId);
    Node<int>* dllNode = lruList_.getHead();
    insertEntry(pageId, page, dllNode);
    ++size_;
}

void LRUCache::pinPage(int pageId) {
    CacheEntry* entry = findEntry(pageId);
    if (entry != nullptr) {
        entry->pinned = true;
        entry->page->isPinned = true;
    }
}

void LRUCache::unpinPage(int pageId) {
    CacheEntry* entry = findEntry(pageId);
    if (entry != nullptr) {
        entry->pinned = false;
        entry->page->isPinned = false;
    }
}

int LRUCache::getEvictionCount() const {
    return evictionCount_;
}

void LRUCache::clear() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        CacheEntry* current = hashTable_[i];
        while (current != nullptr) {
            CacheEntry* next = current->next;
            delete current;
            current = next;
        }
        hashTable_[i] = nullptr;
    }
    size_ = 0;
    evictionCount_ = 0;
    pageFaultCount_ = 0;
}

int LRUCache::hash(int pageId) const {
    return pageId % HASH_TABLE_SIZE;
}

CacheEntry* LRUCache::findEntry(int pageId) {
    int bucket = hash(pageId);
    CacheEntry* current = hashTable_[bucket];
    
    while (current != nullptr) {
        if (current->pageId == pageId) {
            return current;
        }
        current = current->next;
    }
    
    return nullptr;
}

void LRUCache::insertEntry(int pageId, Page* page, Node<int>* dllNode) {
    int bucket = hash(pageId);
    CacheEntry* newEntry = new CacheEntry(pageId, page, dllNode);
    
    newEntry->next = hashTable_[bucket];
    hashTable_[bucket] = newEntry;
}

void LRUCache::removeEntry(int pageId) {
    int bucket = hash(pageId);
    CacheEntry* current = hashTable_[bucket];
    CacheEntry* prev = nullptr;
    
    while (current != nullptr) {
        if (current->pageId == pageId) {
            if (prev == nullptr) {
                hashTable_[bucket] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void LRUCache::evictLRU() {
    Node<int>* tail = lruList_.getTail();
    if (tail == nullptr) {
        return;
    }
    
    int pageId = tail->data;
    CacheEntry* entry = findEntry(pageId);
    
    if (entry == nullptr || entry->pinned) {
        return;
    }
    
    Page* page = entry->page;
    if (page != nullptr && page->isDirty) {
        if (pager_ != nullptr) {
            pager_->writePage(pageId);
        }
        
        char logMessage[128];
        int pos = 0;
        const char* prefix = "[LOG] Page ";
        while (prefix[pos] != '\0') {
            logMessage[pos] = prefix[pos];
            ++pos;
        }
        
        char idStr[32];
        int idPos = 0;
        int tempId = pageId;
        if (tempId == 0) {
            idStr[idPos++] = '0';
        } else {
            char idBuffer[32];
            int idBufPos = 0;
            while (tempId > 0) {
                idBuffer[idBufPos++] = '0' + (tempId % 10);
                tempId /= 10;
            }
            for (int i = idBufPos - 1; i >= 0; --i) {
                idStr[idPos++] = idBuffer[i];
            }
        }
        idStr[idPos] = '\0';
        
        int idIdx = 0;
        while (idStr[idIdx] != '\0') {
            logMessage[pos++] = idStr[idIdx++];
        }
        
        const char* suffix = " evicted via LRU, written to disk\n";
        int sufIdx = 0;
        while (suffix[sufIdx] != '\0') {
            logMessage[pos++] = suffix[sufIdx++];
        }
        logMessage[pos] = '\0';
        
        Logger::getInstance()->logLog(logMessage);
    }
    
    lruList_.removeNode(tail);
    removeEntry(pageId);
    --size_;
    ++evictionCount_;
}

} // namespace NanoDB
