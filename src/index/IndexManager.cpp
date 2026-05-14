#include "index/IndexManager.h"
#include <cstdio>
#include <ctime>

namespace NanoDB {

IndexManager::IndexManager() : size_(0) {
    for (int i = 0; i < HASH_CAPACITY; ++i) {
        buckets_[i] = nullptr;
    }
}

IndexManager::~IndexManager() {
    for (int i = 0; i < HASH_CAPACITY; ++i) {
        HashNode* curr = buckets_[i];
        while (curr != nullptr) {
            HashNode* next = curr->next;
            delete[] curr->key;
            delete curr->tree;
            delete curr;
            curr = next;
        }
    }
}

IndexManager& IndexManager::getInstance() {
    static IndexManager instance;
    return instance;
}

unsigned int IndexManager::hashString(const char* str) const {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

char* IndexManager::buildIndexKey(const char* tableName, const char* columnName) const {
    int tlen = 0;
    while (tableName[tlen] != '\0') {
        tlen++;
    }
    int clen = 0;
    while (columnName[clen] != '\0') {
        clen++;
    }

    int total = tlen + 1 + clen + 1;
    char* key = new char[total];

    for (int i = 0; i < tlen; ++i) {
        key[i] = tableName[i];
    }
    key[tlen] = '.';
    for (int i = 0; i < clen; ++i) {
        key[tlen + 1 + i] = columnName[i];
    }
    key[tlen + 1 + clen] = '\0';

    return key;
}

bool IndexManager::strEquals(const char* a, const char* b) {
    while (*a && *b) {
        if (*a++ != *b++) {
            return false;
        }
    }
    return *a == *b;
}

AVLTree* IndexManager::getTree(const char* tableName, const char* columnName) const {
    char* key = buildIndexKey(tableName, columnName);
    unsigned int h = hashString(key) % HASH_CAPACITY;

    HashNode* curr = buckets_[h];
    while (curr != nullptr) {
        if (strEquals(curr->key, key)) {
            delete[] key;
            return curr->tree;
        }
        curr = curr->next;
    }

    delete[] key;
    return nullptr;
}

void IndexManager::createIndex(const char* tableName, const char* columnName) {
    char* key = buildIndexKey(tableName, columnName);
    unsigned int h = hashString(key) % HASH_CAPACITY;

    HashNode* curr = buckets_[h];
    while (curr != nullptr) {
        if (strEquals(curr->key, key)) {
            delete[] key;
            return;
        }
        curr = curr->next;
    }

    HashNode* node = new HashNode();
    node->key = key;
    node->tree = new AVLTree();
    node->next = buckets_[h];
    buckets_[h] = node;
    size_++;
}

void IndexManager::insertEntry(const char* tableName, const char* columnName, int key, int pageId) {
    AVLTree* tree = getTree(tableName, columnName);
    if (tree != nullptr) {
        tree->insert(key, pageId);
    }
}

int IndexManager::lookupPage(const char* tableName, const char* columnName, int key) {
    AVLTree* tree = getTree(tableName, columnName);
    if (tree == nullptr) {
        return -1;
    }
    clock_t start = clock();
    int result = tree->search(key);
    clock_t end = clock();
    double timeMs = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("[BENCHMARK] AVL index found key %d in %.2fms\n", key, timeMs);
    return result;
}

int* IndexManager::rangeLookup(const char* tableName, const char* columnName, int low, int high, int& count) {
    AVLTree* tree = getTree(tableName, columnName);
    if (tree == nullptr) {
        count = 0;
        return nullptr;
    }
    return tree->rangeSearch(low, high, count);
}

void IndexManager::dropIndex(const char* tableName, const char* columnName) {
    char* key = buildIndexKey(tableName, columnName);
    unsigned int h = hashString(key) % HASH_CAPACITY;

    HashNode* curr = buckets_[h];
    HashNode* prev = nullptr;
    while (curr != nullptr) {
        if (strEquals(curr->key, key)) {
            if (prev == nullptr) {
                buckets_[h] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete[] curr->key;
            delete curr->tree;
            delete curr;
            size_--;
            delete[] key;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    delete[] key;
}

bool IndexManager::hasIndex(const char* tableName, const char* columnName) const {
    return getTree(tableName, columnName) != nullptr;
}

int SequentialScanner::scan(const char* tableName, int targetKey, Pager& pager) {
    clock_t start = clock();
    int numPages = pager.getNumPages();
    int pagesScanned = 0;

    for (int i = 0; i < numPages; ++i) {
        Page* page = pager.getPage(i);
        pagesScanned++;
        if (page != nullptr) {
            const char* data = page->data;
            for (int offset = 0; offset < Page::PAGE_SIZE - (int)sizeof(int); offset += (int)sizeof(int)) {
                int value = 0;
                const unsigned char* src = reinterpret_cast<const unsigned char*>(data + offset);
                unsigned char* dst = reinterpret_cast<unsigned char*>(&value);
                for (size_t b = 0; b < sizeof(int); ++b) {
                    dst[b] = src[b];
                }
                if (value == targetKey) {
                    clock_t end = clock();
                    double timeMs = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
                    printf("[BENCHMARK] Sequential scan found key %d in %.2fms after %d page reads\n", targetKey, timeMs, pagesScanned);
                    return i;
                }
            }
        }
    }

    clock_t end = clock();
    double timeMs = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("[BENCHMARK] Sequential scan did not find key %d in %.2fms after %d page reads\n", targetKey, timeMs, pagesScanned);
    return -1;
}

} // namespace NanoDB
