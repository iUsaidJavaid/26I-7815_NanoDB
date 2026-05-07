#ifndef NANODB_PRIORITY_QUEUE_H
#define NANODB_PRIORITY_QUEUE_H

namespace NanoDB {

class PriorityQueue {
public:
    struct QueueNode {
        int priority;
        int value;
    };
    
    PriorityQueue(int capacity);
    ~PriorityQueue();
    
    bool enqueue(int priority, int value);
    bool dequeue(int* value);
    bool peek(int* value);
    
    bool isEmpty();
    int size();
    
    void clear();
    
private:
    QueueNode* heap_;
    int capacity_;
    int size_;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    int parent(int index);
    int leftChild(int index);
    int rightChild(int index);
    void swap(int i, int j);
};

} // namespace NanoDB

#endif // NANODB_PRIORITY_QUEUE_H
