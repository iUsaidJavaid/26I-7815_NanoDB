#ifndef NANODB_STACK_H
#define NANODB_STACK_H

namespace NanoDB {

struct StackOverflowException {
    char message[128];
    
    StackOverflowException() {
        int i = 0;
        const char* msg = "Stack overflow: cannot push to full stack";
        while (msg[i] != '\0' && i < 127) {
            message[i] = msg[i];
            ++i;
        }
        message[i] = '\0';
    }
    
    void print() const {
        printf("%s\n", message);
    }
};

struct StackUnderflowException {
    char message[128];
    
    StackUnderflowException() {
        int i = 0;
        const char* msg = "Stack underflow: cannot pop from empty stack";
        while (msg[i] != '\0' && i < 127) {
            message[i] = msg[i];
            ++i;
        }
        message[i] = '\0';
    }
    
    void print() const {
        printf("%s\n", message);
    }
};

template<typename T>
class Stack {
public:
    Stack(int capacity = 256) : capacity_(capacity), top_(-1) {
        data_ = new T[capacity_];
    }
    
    ~Stack() {
        clearAndDeleteObjects();
        delete[] data_;
    }
    
    void push(T val) {
        if (isFull()) {
            StackOverflowException ex;
            ex.print();
            return;
        }
        ++top_;
        data_[top_] = val;
    }
    
    T pop() {
        if (isEmpty()) {
            StackUnderflowException ex;
            ex.print();
            return T();
        }
        T val = data_[top_];
        --top_;
        return val;
    }
    
    T peek() const {
        if (isEmpty()) {
            StackUnderflowException ex;
            ex.print();
            return T();
        }
        return data_[top_];
    }
    
    bool isEmpty() const {
        return top_ == -1;
    }
    
    bool isFull() const {
        return top_ == capacity_ - 1;
    }
    
    int size() const {
        return top_ + 1;
    }
    
    void clear() {
        top_ = -1;
    }
    
    // Clear stack and delete objects if T is a pointer type
    // This is a no-op for non-pointer types
    void clearAndDeleteObjects() {
        for (int i = 0; i <= top_; ++i) {
            delete data_[i];
        }
        top_ = -1;
    }
    
private:
    T* data_;
    int capacity_;
    int top_;
};

} // namespace NanoDB

#endif // NANODB_STACK_H
