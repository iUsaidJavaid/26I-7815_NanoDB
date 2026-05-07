#include "index/AVLTree.h"

namespace NanoDB {

AVLTree::AVLTree() : root_(nullptr), size_(0) {
}

AVLTree::~AVLTree() {
    clear();
}

bool AVLTree::insert(int key, int value) {
    return false;
}

bool AVLTree::remove(int key) {
    return false;
}

bool AVLTree::search(int key, int* value) {
    return false;
}

void AVLTree::clear() {
}

int AVLTree::size() const {
    return size_;
}

int AVLTree::height(AVLNode* node) {
    return 0;
}

int AVLTree::balanceFactor(AVLNode* node) {
    return 0;
}

AVLTree::AVLNode* AVLTree::rotateRight(AVLNode* y) {
    return nullptr;
}

AVLTree::AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    return nullptr;
}

AVLTree::AVLNode* AVLTree::insertNode(AVLNode* node, int key, int value) {
    return nullptr;
}

AVLTree::AVLNode* AVLTree::removeNode(AVLNode* node, int key) {
    return nullptr;
}

AVLTree::AVLNode* AVLTree::findMin(AVLNode* node) {
    return nullptr;
}

AVLTree::AVLNode* AVLTree::balance(AVLNode* node) {
    return nullptr;
}

void AVLTree::destroy(AVLNode* node) {
}

} // namespace NanoDB
