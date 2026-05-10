#include "index/AVLTree.h"
#include <cstdio>

namespace NanoDB {

AVLTree::AVLTree() : root_(nullptr), nodeCount_(0) {
}

AVLTree::~AVLTree() {
    destroy(root_);
}

int AVLTree::height(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

int AVLTree::max(int a, int b) const {
    return (a > b) ? a : b;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

AVLTree::AVLNode* AVLTree::rotateLeft(AVLNode* node) {
    AVLNode* rightChild = node->right;
    AVLNode* leftSubtreeOfRight = rightChild->left;

    rightChild->left = node;
    node->right = leftSubtreeOfRight;

    node->height = max(height(node->left), height(node->right)) + 1;
    rightChild->height = max(height(rightChild->left), height(rightChild->right)) + 1;

    return rightChild;
}

AVLTree::AVLNode* AVLTree::rotateRight(AVLNode* node) {
    AVLNode* leftChild = node->left;
    AVLNode* rightSubtreeOfLeft = leftChild->right;

    leftChild->right = node;
    node->left = rightSubtreeOfLeft;

    node->height = max(height(node->left), height(node->right)) + 1;
    leftChild->height = max(height(leftChild->left), height(leftChild->right)) + 1;

    return leftChild;
}

AVLTree::AVLNode* AVLTree::rotateLeftRight(AVLNode* node) {
    node->left = rotateLeft(node->left);
    return rotateRight(node);
}

AVLTree::AVLNode* AVLTree::rotateRightLeft(AVLNode* node) {
    node->right = rotateRight(node->right);
    return rotateLeft(node);
}

AVLTree::AVLNode* AVLTree::rebalance(AVLNode* node) {
    int balance = getBalanceFactor(node);

    if (balance > 1) {
        if (getBalanceFactor(node->left) < 0) {
            node = rotateLeftRight(node);
        } else {
            node = rotateRight(node);
        }
    } else if (balance < -1) {
        if (getBalanceFactor(node->right) > 0) {
            node = rotateRightLeft(node);
        } else {
            node = rotateLeft(node);
        }
    }

    return node;
}

AVLTree::AVLNode* AVLTree::createNode(int key, int pageId) {
    AVLNode* node = new AVLNode();
    node->key = key;
    node->pageId = pageId;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    return node;
}

AVLTree::AVLNode* AVLTree::insertNode(AVLNode* node, int key, int pageId) {
    if (node == nullptr) {
        nodeCount_++;
        return createNode(key, pageId);
    }

    if (key < node->key) {
        node->left = insertNode(node->left, key, pageId);
    } else if (key > node->key) {
        node->right = insertNode(node->right, key, pageId);
    } else {
        node->pageId = pageId;
        return node;
    }

    node->height = max(height(node->left), height(node->right)) + 1;
    node = rebalance(node);

    return node;
}

void AVLTree::insert(int key, int pageId) {
    root_ = insertNode(root_, key, pageId);
}

int AVLTree::search(int key) {
    AVLNode* current = root_;
    while (current != nullptr) {
        if (key == current->key) {
            return current->pageId;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return -1;
}

AVLTree::AVLNode* AVLTree::findMin(AVLNode* node) const {
    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

AVLTree::AVLNode* AVLTree::removeNode(AVLNode* node, int key) {
    if (node == nullptr) {
        return nullptr;
    }

    if (key < node->key) {
        node->left = removeNode(node->left, key);
    } else if (key > node->key) {
        node->right = removeNode(node->right, key);
    } else {
        if (node->left == nullptr || node->right == nullptr) {
            AVLNode* temp = (node->left != nullptr) ? node->left : node->right;
            if (temp == nullptr) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp;
            }
            delete temp;
            nodeCount_--;
        } else {
            AVLNode* temp = findMin(node->right);
            node->key = temp->key;
            node->pageId = temp->pageId;
            node->right = removeNode(node->right, temp->key);
        }
    }

    if (node == nullptr) {
        return nullptr;
    }

    node->height = max(height(node->left), height(node->right)) + 1;
    node = rebalance(node);

    return node;
}

void AVLTree::remove(int key) {
    root_ = removeNode(root_, key);
}

void AVLTree::rangeCollect(AVLNode* node, int low, int high, int* result, int& index) const {
    if (node == nullptr) {
        return;
    }
    if (low < node->key) {
        rangeCollect(node->left, low, high, result, index);
    }
    if (low <= node->key && node->key <= high) {
        result[index++] = node->pageId;
    }
    if (high > node->key) {
        rangeCollect(node->right, low, high, result, index);
    }
}

int* AVLTree::rangeSearch(int low, int high, int& count) {
    count = 0;
    if (nodeCount_ == 0) {
        return nullptr;
    }
    
    int* result = new int[nodeCount_];
    rangeCollect(root_, low, high, result, count);
    
    if (count == 0) {
        delete[] result;
        return nullptr;
    }
    
    return result;
}

int AVLTree::getHeight() const {
    return height(root_);
}

int AVLTree::getNodeCount() const {
    return nodeCount_;
}

void AVLTree::inOrderPrint(AVLNode* node) const {
    if (node == nullptr) {
        return;
    }
    inOrderPrint(node->left);
    printf("(%d,%d) ", node->key, node->pageId);
    inOrderPrint(node->right);
}

void AVLTree::printInOrder() const {
    inOrderPrint(root_);
    printf("\n");
}

void AVLTree::destroy(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
}

} // namespace NanoDB
