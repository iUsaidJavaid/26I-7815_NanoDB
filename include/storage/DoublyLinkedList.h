#ifndef NANODB_DOUBLY_LINKED_LIST_H
#define NANODB_DOUBLY_LINKED_LIST_H

namespace NanoDB {

template<typename T>
struct Node {
    T data;
    Node* prev;
    Node* next;
    
    Node(T val) : data(val), prev(nullptr), next(nullptr) {}
};

template<typename T>
class DoublyLinkedList {
public:
    DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {
    }
    
    ~DoublyLinkedList() {
        Node<T>* current = head_;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }
    
    void pushFront(T val) {
        Node<T>* newNode = new Node<T>(val);
        
        if (head_ == nullptr) {
            head_ = newNode;
            tail_ = newNode;
        } else {
            newNode->next = head_;
            head_->prev = newNode;
            head_ = newNode;
        }
        
        ++size_;
    }
    
    void pushBack(T val) {
        Node<T>* newNode = new Node<T>(val);
        
        if (tail_ == nullptr) {
            head_ = newNode;
            tail_ = newNode;
        } else {
            newNode->prev = tail_;
            tail_->next = newNode;
            tail_ = newNode;
        }
        
        ++size_;
    }
    
    T popBack() {
        if (tail_ == nullptr) {
            return T();
        }
        
        Node<T>* toRemove = tail_;
        T value = toRemove->data;
        
        if (head_ == tail_) {
            head_ = nullptr;
            tail_ = nullptr;
        } else {
            tail_ = tail_->prev;
            tail_->next = nullptr;
        }
        
        delete toRemove;
        --size_;
        
        return value;
    }
    
    void moveToFront(Node<T>* node) {
        if (node == nullptr || node == head_) {
            return;
        }
        
        if (node == tail_) {
            tail_ = node->prev;
            tail_->next = nullptr;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        
        node->prev = nullptr;
        node->next = head_;
        head_->prev = node;
        head_ = node;
    }
    
    void removeNode(Node<T>* node) {
        if (node == nullptr) {
            return;
        }
        
        if (node == head_ && node == tail_) {
            head_ = nullptr;
            tail_ = nullptr;
        } else if (node == head_) {
            head_ = head_->next;
            head_->prev = nullptr;
        } else if (node == tail_) {
            tail_ = tail_->prev;
            tail_->next = nullptr;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        
        delete node;
        --size_;
    }
    
    Node<T>* find(T val) {
        Node<T>* current = head_;
        while (current != nullptr) {
            if (current->data == val) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    int size() const {
        return size_;
    }
    
    bool isEmpty() const {
        return size_ == 0;
    }
    
    Node<T>* getHead() const {
        return head_;
    }
    
    Node<T>* getTail() const {
        return tail_;
    }
    
private:
    Node<T>* head_;
    Node<T>* tail_;
    int size_;
};

#ifdef RUN_DLL_TEST
void testDoublyLinkedList() {
    DoublyLinkedList<int> list;
    
    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);
    
    if (list.size() != 3) {
        return;
    }
    
    if (list.popBack() != 3) {
        return;
    }
    
    if (list.size() != 2) {
        return;
    }
    
    list.pushFront(0);
    
    if (list.size() != 3) {
        return;
    }
    
    Node<int>* node = list.find(2);
    if (node == nullptr) {
        return;
    }
    
    list.moveToFront(node);
    
    if (list.popBack() != 1) {
        return;
    }
    
    if (list.popBack() != 0) {
        return;
    }
    
    if (list.popBack() != 2) {
        return;
    }
    
    if (!list.isEmpty()) {
        return;
    }
}
#endif

} // namespace NanoDB

#endif // NANODB_DOUBLY_LINKED_LIST_H
