#include "engine/PriorityQueue.h"
#include <cstdio>

namespace NanoDB {

PriorityQueue::PriorityQueue(int capacity) 
    : heapSize_(0), capacity_(capacity) {
    heap_ = new QueryTask*[capacity_];
    for (int i = 0; i < capacity_; ++i) {
        heap_[i] = nullptr;
    }
}

PriorityQueue::~PriorityQueue() {
    for (int i = 0; i < heapSize_; ++i) {
        if (heap_[i] != nullptr) {
            delete heap_[i];
        }
    }
    delete[] heap_;
}

void PriorityQueue::enqueue(QueryTask* task) {
    if (task == nullptr || heapSize_ >= capacity_) {
        return;
    }
    
    heap_[heapSize_] = task;
    siftUp(heapSize_);
    ++heapSize_;
}

QueryTask* PriorityQueue::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }
    
    QueryTask* result = heap_[0];
    
    if (result->isAdmin() && heapSize_ > 1) {
        int userCount = 0;
        for (int i = 1; i < heapSize_; ++i) {
            if (heap_[i] != nullptr && !heap_[i]->isAdmin()) {
                ++userCount;
            }
        }
        
        if (userCount > 0) {
            char logMessage[256];
            int pos = 0;
            const char* prefix = "[LOG] Priority Queue: Admin query intercepted, executing before ";
            int prefixIdx = 0;
            while (prefix[prefixIdx] != '\0') {
                logMessage[pos++] = prefix[prefixIdx++];
            }
            
            char countStr[32];
            int countPos = 0;
            int temp = userCount;
            if (temp == 0) {
                countStr[countPos++] = '0';
            } else {
                char countBuffer[32];
                int countBufPos = 0;
                while (temp > 0) {
                    countBuffer[countBufPos++] = '0' + (temp % 10);
                    temp /= 10;
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
            
            const char* suffix = " pending reads\n";
            int sufIdx = 0;
            while (suffix[sufIdx] != '\0') {
                logMessage[pos++] = suffix[sufIdx++];
            }
            logMessage[pos] = '\0';
            
            Logger::getInstance()->logLog(logMessage);
        }
    }
    
    heap_[0] = heap_[heapSize_ - 1];
    heap_[heapSize_ - 1] = nullptr;
    --heapSize_;
    
    if (heapSize_ > 0) {
        siftDown(0);
    }
    
    return result;
}

QueryTask* PriorityQueue::peek() const {
    if (isEmpty()) {
        return nullptr;
    }
    return heap_[0];
}

bool PriorityQueue::isEmpty() const {
    return heapSize_ == 0;
}

int PriorityQueue::size() const {
    return heapSize_;
}

void PriorityQueue::siftUp(int index) {
    while (index > 0) {
        int p = parent(index);
        if (hasHigherPriority(heap_[index], heap_[p])) {
            swap(index, p);
            index = p;
        } else {
            break;
        }
    }
}

void PriorityQueue::siftDown(int index) {
    while (true) {
        int left = leftChild(index);
        int right = rightChild(index);
        int smallest = index;
        
        if (left < heapSize_ && hasHigherPriority(heap_[left], heap_[smallest])) {
            smallest = left;
        }
        
        if (right < heapSize_ && hasHigherPriority(heap_[right], heap_[smallest])) {
            smallest = right;
        }
        
        if (smallest != index) {
            swap(index, smallest);
            index = smallest;
        } else {
            break;
        }
    }
}

int PriorityQueue::parent(int index) {
    return (index - 1) / 2;
}

int PriorityQueue::leftChild(int index) {
    return 2 * index + 1;
}

int PriorityQueue::rightChild(int index) {
    return 2 * index + 2;
}

void PriorityQueue::swap(int i, int j) {
    QueryTask* temp = heap_[i];
    heap_[i] = heap_[j];
    heap_[j] = temp;
}

bool PriorityQueue::hasHigherPriority(QueryTask* a, QueryTask* b) const {
    if (a == nullptr) return false;
    if (b == nullptr) return true;
    
    if (a->priority < b->priority) {
        return true;
    }
    
    if (a->priority == b->priority && a->taskId < b->taskId) {
        return true;
    }
    
    return false;
}

#ifdef RUN_PRIORITY_QUEUE_TEST
void testPriorityQueue() {
    PriorityQueue pq(100);
    
    for (int i = 1; i <= 50; ++i) {
        QueryTask* task = new QueryTask();
        task->priority = USER;
        task->taskId = i;
        pq.enqueue(task);
    }
    
    QueryTask* adminTask = new QueryTask();
    adminTask->priority = ADMIN;
    adminTask->taskId = 51;
    pq.enqueue(adminTask);
    
    QueryTask* first = pq.dequeue();
    printf("First task: priority=%d, taskId=%d\n", first->priority, first->taskId);
    
    int adminCount = 0;
    if (first->priority == ADMIN) {
        ++adminCount;
    }
    delete first;
    
    while (!pq.isEmpty()) {
        QueryTask* task = pq.dequeue();
        if (task->priority == ADMIN) {
            ++adminCount;
        }
        delete task;
    }
    
    printf("Total admin tasks: %d\n", adminCount);
}
#endif

} // namespace NanoDB
