#include "engine/PriorityQueue.h"

namespace NanoDB {

PriorityQueue::PriorityQueue(int capacity) 
    : heap_(nullptr), capacity_(capacity), size_(0) {
}

PriorityQueue::~PriorityQueue() {
}

bool PriorityQueue::enqueue(int priority, int value) {
    return false;
}

bool PriorityQueue::dequeue(int* value) {
    return false;
}

bool PriorityQueue::peek(int* value) {
    return false;
}

bool PriorityQueue::isEmpty() {
    return size_ == 0;
}

int PriorityQueue::size() {
    return size_;
}

void PriorityQueue::clear() {
}

void PriorityQueue::heapifyUp(int index) {
}

void PriorityQueue::heapifyDown(int index) {
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
}

} // namespace NanoDB
