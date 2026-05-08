#ifndef NANODB_AVL_TREE_H
#define NANODB_AVL_TREE_H

namespace NanoDB {

class AVLTree {
public:
    struct AVLNode {
        int key;
        int pageId;
        AVLNode* left;
        AVLNode* right;
        int height;
    };

    AVLTree();
    ~AVLTree();

    void insert(int key, int pageId);
    int search(int key);
    void remove(int key);
    int* rangeSearch(int low, int high, int& count);
    int getHeight() const;
    int getNodeCount() const;
    void printInOrder() const;

    int getBalanceFactor(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* node);
    AVLNode* rotateRight(AVLNode* node);
    AVLNode* rotateLeftRight(AVLNode* node);
    AVLNode* rotateRightLeft(AVLNode* node);
    AVLNode* rebalance(AVLNode* node);

private:
    AVLNode* root_;
    int nodeCount_;

    int height(AVLNode* node) const;
    int max(int a, int b) const;
    AVLNode* createNode(int key, int pageId);
    AVLNode* insertNode(AVLNode* node, int key, int pageId);
    AVLNode* removeNode(AVLNode* node, int key);
    AVLNode* findMin(AVLNode* node) const;
    void destroy(AVLNode* node);
    void inOrderPrint(AVLNode* node) const;
    void rangeCollect(AVLNode* node, int low, int high, int* result, int& index) const;
};

} // namespace NanoDB

#endif // NANODB_AVL_TREE_H
