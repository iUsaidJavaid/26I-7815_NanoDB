#ifndef NANODB_HASH_MAP_H
#define NANODB_HASH_MAP_H

namespace NanoDB {

template<typename KeyType, typename ValueType>
struct HashNode {
    KeyType key;
    ValueType value;
    HashNode* next;
    
    HashNode(KeyType k, ValueType v) : key(k), value(v), next(nullptr) {}
};

template<typename KeyType, typename ValueType>
class HashMap {
public:
    HashMap(int capacity = 256) : capacity_(capacity), size_(0) {
        buckets_ = new HashNode<KeyType, ValueType>*[capacity_];
        for (int i = 0; i < capacity_; ++i) {
            buckets_[i] = nullptr;
        }
    }
    
    ~HashMap() {
        for (int i = 0; i < capacity_; ++i) {
            HashNode<KeyType, ValueType>* current = buckets_[i];
            while (current != nullptr) {
                HashNode<KeyType, ValueType>* next = current->next;
                delete current;
                current = next;
            }
            buckets_[i] = nullptr;
        }
        delete[] buckets_;
    }
    
    void insert(KeyType key, ValueType value) {
        int index = hash(key);
        
        HashNode<KeyType, ValueType>* current = buckets_[index];
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }
        
        HashNode<KeyType, ValueType>* newNode = new HashNode<KeyType, ValueType>(key, value);
        newNode->next = buckets_[index];
        buckets_[index] = newNode;
        ++size_;
        
        if (size_ > capacity_ * 3 / 4) {
            rehash(capacity_ * 2);
        }
    }
    
    ValueType* get(KeyType key) {
        int index = hash(key);
        HashNode<KeyType, ValueType>* current = buckets_[index];
        
        while (current != nullptr) {
            if (current->key == key) {
                return &current->value;
            }
            current = current->next;
        }
        
        return nullptr;
    }
    
    bool remove(KeyType key) {
        int index = hash(key);
        HashNode<KeyType, ValueType>* current = buckets_[index];
        HashNode<KeyType, ValueType>* prev = nullptr;
        
        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    buckets_[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                --size_;
                return true;
            }
            prev = current;
            current = current->next;
        }
        
        return false;
    }
    
    bool contains(KeyType key) const {
        int index = hash(key);
        HashNode<KeyType, ValueType>* current = buckets_[index];
        
        while (current != nullptr) {
            if (current->key == key) {
                return true;
            }
            current = current->next;
        }
        
        return false;
    }
    
    int size() const {
        return size_;
    }
    
private:
    HashNode<KeyType, ValueType>** buckets_;
    int capacity_;
    int size_;
    
    int hash(KeyType key) const;
    void rehash(int newCapacity);
};

template<>
int HashMap<int, int>::hash(int key) const {
    return (key * 2654435761u) % capacity_;
}

template<>
int HashMap<const char*, int>::hash(const char* key) const {
    unsigned long hash = 5381;
    int c;
    int i = 0;
    while ((c = key[i]) != '\0') {
        hash = ((hash << 5) + hash) + c;
        ++i;
    }
    return hash % capacity_;
}

template<>
int HashMap<char*, int>::hash(char* key) const {
    unsigned long hash = 5381;
    int c;
    int i = 0;
    while ((c = key[i]) != '\0') {
        hash = ((hash << 5) + hash) + c;
        ++i;
    }
    return hash % capacity_;
}

// Template specialization for char* key comparison using strcmp
template<typename ValueType>
void HashMap<char*, ValueType>::insert(char* key, ValueType value) {
    int index = hash(key);
    
    HashNode<char*, ValueType>* current = buckets_[index];
    while (current != nullptr) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    HashNode<char*, ValueType>* newNode = new HashNode<char*, ValueType>(key, value);
    newNode->next = buckets_[index];
    buckets_[index] = newNode;
    ++size_;
    
    if (size_ > capacity_ * 3 / 4) {
        rehash(capacity_ * 2);
    }
}

template<typename ValueType>
ValueType* HashMap<char*, ValueType>::get(char* key) {
    int index = hash(key);
    HashNode<char*, ValueType>* current = buckets_[index];
    
    while (current != nullptr) {
        if (strcmp(current->key, key) == 0) {
            return &current->value;
        }
        current = current->next;
    }
    
    return nullptr;
}

template<typename ValueType>
bool HashMap<char*, ValueType>::remove(char* key) {
    int index = hash(key);
    HashNode<char*, ValueType>* current = buckets_[index];
    HashNode<char*, ValueType>* prev = nullptr;
    
    while (current != nullptr) {
        if (strcmp(current->key, key) == 0) {
            if (prev == nullptr) {
                buckets_[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            --size_;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

template<typename ValueType>
bool HashMap<char*, ValueType>::contains(char* key) const {
    int index = hash(key);
    HashNode<char*, ValueType>* current = buckets_[index];
    
    while (current != nullptr) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

template<typename KeyType, typename ValueType>
void HashMap<KeyType, ValueType>::rehash(int newCapacity) {
    HashNode<KeyType, ValueType>** newBuckets = new HashNode<KeyType, ValueType>*[newCapacity];
    for (int i = 0; i < newCapacity; ++i) {
        newBuckets[i] = nullptr;
    }
    
    int oldCapacity = capacity_;
    HashNode<KeyType, ValueType>** oldBuckets = buckets_;
    
    capacity_ = newCapacity;
    buckets_ = newBuckets;
    size_ = 0;
    
    for (int i = 0; i < oldCapacity; ++i) {
        HashNode<KeyType, ValueType>* current = oldBuckets[i];
        while (current != nullptr) {
            HashNode<KeyType, ValueType>* next = current->next;
            insert(current->key, current->value);
            delete current;
            current = next;
        }
    }
    
    delete[] oldBuckets;
}

#ifdef RUN_HASHMAP_TEST
void testHashMap() {
    HashMap<const char*, int> map(256);
    
    const char* keys[100];
    for (int i = 0; i < 100; ++i) {
        keys[i] = new char[16];
        int pos = 0;
        int temp = i;
        if (temp == 0) {
            keys[i][pos++] = '0';
        } else {
            char buffer[16];
            int bufPos = 0;
            while (temp > 0) {
                buffer[bufPos++] = '0' + (temp % 10);
                temp /= 10;
            }
            for (int j = bufPos - 1; j >= 0; --j) {
                keys[i][pos++] = buffer[j];
            }
        }
        keys[i][pos] = '\0';
        
        map.insert(keys[i], i * 10);
    }
    
    bool allSuccess = true;
    for (int i = 0; i < 100; ++i) {
        int* value = map.get(keys[i]);
        if (value == nullptr || *value != i * 10) {
            allSuccess = false;
            break;
        }
    }
    
    for (int i = 0; i < 100; ++i) {
        delete[] keys[i];
    }
}
#endif

} // namespace NanoDB

#endif // NANODB_HASH_MAP_H
