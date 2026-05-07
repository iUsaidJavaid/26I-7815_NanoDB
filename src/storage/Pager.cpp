#include "storage/Pager.h"

namespace NanoDB {

Pager::Pager(const char* filename) 
    : filename_(nullptr), file_descriptor_(-1), num_pages_(0) {
}

Pager::~Pager() {
    close();
}

bool Pager::open() {
    return false;
}

void Pager::close() {
}

Page* Pager::getPage(int page_num) {
    return nullptr;
}

void Pager::flushPage(int page_num) {
}

int Pager::getNumPages() const {
    return num_pages_;
}

} // namespace NanoDB
