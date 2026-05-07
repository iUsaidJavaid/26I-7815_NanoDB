#include "storage/LRUCache.h"

namespace NanoDB {

LRUCache::LRUCache(int capacity) 
    : capacity_(capacity), size_(0), head_(nullptr), tail_(nullptr), hash_table_(nullptr) {
}

LRUCache::~LRUCache() {
    clear();
}

Page* LRUCache::get(int key) {
    return nullptr;
}

void LRUCache::put(int key, Page* value) {
}

void LRUCache::clear() {
}

void LRUCache::moveToHead(CacheNode* node) {
}

void LRUCache::removeNode(CacheNode* node) {
}

LRUCache::CacheNode* LRUCache::addNodeToHead(int key, Page* value) {
    return nullptr;
}

void LRUCache::removeTail() {
}

} // namespace NanoDB
