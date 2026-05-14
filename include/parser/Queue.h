#ifndef NANODB_QUEUE_H
#define NANODB_QUEUE_H

#include <type_traits>

namespace NanoDB {

template<typename T>
class Queue {
public:
    Queue(int capacity = 256) : capacity_(capacity), front_(0), rear_(-1), count_(0) {
        data_ = new T[capacity];
    }
    
    ~Queue() {
        clearAndDeleteObjects();
        delete[] data_;
    }
    
    void enqueue(T val) {
        if (isFull()) {
            return;
        }
        rear_ = (rear_ + 1) % capacity_;
        data_[rear_] = val;
        ++count_;
    }
    
    T dequeue() {
        if (isEmpty()) {
            return T();
        }
        T val = data_[front_];
        front_ = (front_ + 1) % capacity_;
        --count_;
        return val;
    }
    
    T front() const {
        if (isEmpty()) {
            return T();
        }
        return data_[front_];
    }

    bool isFull() const {
        return count_ == capacity_;
    }
    
    bool isEmpty() const {
        return count_ == 0;
    }
    
    int size() const {
        return count_;
    }
    
    void clear() {
        front_ = 0;
        rear_ = -1;
        count_ = 0;
    }
    
    // Clear queue and delete objects if T is a pointer type
    // This is a no-op for non-pointer types
    void clearAndDeleteObjects() {
        if constexpr (std::is_pointer<T>::value) {
            int idx = front_;
            for (int i = 0; i < count_; ++i) {
                delete data_[idx];
                idx = (idx + 1) % capacity_;
            }
        }
        front_ = 0;
        rear_ = -1;
        count_ = 0;
    }
    
private:
    T* data_;
    int capacity_;
    int front_;
    int rear_;
    int count_;
};

} // namespace NanoDB

#endif // NANODB_QUEUE_H
