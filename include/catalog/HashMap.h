#ifndef NANODB_HASH_MAP_H
#define NANODB_HASH_MAP_H

namespace NanoDB {

class HashMap {
public:
    struct KeyValuePair {
        int key;
        int value;
        KeyValuePair* next;
    };
    
    HashMap(int capacity);
    ~HashMap();
    
    bool put(int key, int value);
    bool get(int key, int* value);
    bool remove(int key);
    bool contains(int key);
    
    void clear();
    int size() const;
    
private:
    KeyValuePair** buckets_;
    int capacity_;
    int size_;
    
    int hash(int key) const;
};

} // namespace NanoDB

#endif // NANODB_HASH_MAP_H
