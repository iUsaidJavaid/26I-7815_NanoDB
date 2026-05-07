#include "catalog/HashMap.h"

namespace NanoDB {

HashMap::HashMap(int capacity) 
    : buckets_(nullptr), capacity_(capacity), size_(0) {
}

HashMap::~HashMap() {
}

bool HashMap::put(int key, int value) {
    return false;
}

bool HashMap::get(int key, int* value) {
    return false;
}

bool HashMap::remove(int key) {
    return false;
}

bool HashMap::contains(int key) {
    return false;
}

void HashMap::clear() {
}

int HashMap::size() const {
    return size_;
}

int HashMap::hash(int key) const {
    return 0;
}

} // namespace NanoDB
