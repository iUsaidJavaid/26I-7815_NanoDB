#include "storage/Page.h"

namespace NanoDB {

Page::Page() : page_number_(0), dirty_(false) {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        data_[i] = 0;
    }
}

Page::~Page() {
}

char* Page::getData() {
    return data_;
}

const char* Page::getData() const {
    return data_;
}

int Page::getPageNumber() const {
    return page_number_;
}

void Page::setPageNumber(int pageNum) {
    page_number_ = pageNum;
}

bool Page::isDirty() const {
    return dirty_;
}

void Page::setDirty(bool dirty) {
    dirty_ = dirty;
}

void Page::clear() {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        data_[i] = 0;
    }
    dirty_ = false;
}

} // namespace NanoDB
