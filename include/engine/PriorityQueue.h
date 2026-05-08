#ifndef NANODB_PRIORITY_QUEUE_H
#define NANODB_PRIORITY_QUEUE_H

#include "common/Logger.h"

namespace NanoDB {

enum QueryPriority {
    ADMIN = 0,
    USER = 1
};

struct QueryTask {
    char queryString[512];
    QueryPriority priority;
    int taskId;
    
    QueryTask() : priority(USER), taskId(0) {
        for (int i = 0; i < 512; ++i) {
            queryString[i] = '\0';
        }
    }
    
    bool isAdmin() const {
        return priority == ADMIN;
    }
};

class PriorityQueue {
public:
    PriorityQueue(int capacity);
    ~PriorityQueue();
    
    void enqueue(QueryTask* task);
    QueryTask* dequeue();
    QueryTask* peek() const;
    bool isEmpty() const;
    int size() const;
    
private:
    QueryTask** heap_;
    int heapSize_;
    int capacity_;
    
    void siftUp(int index);
    void siftDown(int index);
    int parent(int index);
    int leftChild(int index);
    int rightChild(int index);
    void swap(int i, int j);
    bool hasHigherPriority(QueryTask* a, QueryTask* b) const;
};

} // namespace NanoDB

#endif // NANODB_PRIORITY_QUEUE_H
