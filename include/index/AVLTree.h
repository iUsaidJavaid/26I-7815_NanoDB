#ifndef NANODB_AVL_TREE_H
#define NANODB_AVL_TREE_H

namespace NanoDB {

class AVLTree {
public:
    struct AVLNode {
        int key;
        int value;
        AVLNode* left;
        AVLNode* right;
        int height;
    };
    
    AVLTree();
    ~AVLTree();
    
    bool insert(int key, int value);
    bool remove(int key);
    bool search(int key, int* value);
    
    void clear();
    int size() const;
    
private:
    AVLNode* root_;
    int size_;
    
    int height(AVLNode* node);
    int balanceFactor(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* insertNode(AVLNode* node, int key, int value);
    AVLNode* removeNode(AVLNode* node, int key);
    AVLNode* findMin(AVLNode* node);
    AVLNode* balance(AVLNode* node);
    void destroy(AVLNode* node);
};

} // namespace NanoDB

#endif // NANODB_AVL_TREE_H
