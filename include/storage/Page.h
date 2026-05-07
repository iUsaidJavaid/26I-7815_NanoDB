#ifndef NANODB_PAGE_H
#define NANODB_PAGE_H

#include <cstdio>
#include "common/Types.h"
#include "common/Logger.h"

namespace NanoDB {

struct Page {
    static const int PAGE_SIZE = 4096;
    
    int pageId;
    bool isDirty;
    bool isPinned;
    char data[PAGE_SIZE];
    int usedBytes;
    
    Page() : pageId(0), isDirty(false), isPinned(false), usedBytes(0) {
        for (int i = 0; i < PAGE_SIZE; ++i) {
            data[i] = 0;
        }
    }
    
    void writeRow(const Row& row, int& offset);
    bool readRow(Row& out, int& offset, const ColumnSchema* schema, int colCount);
    void clear();
    void serialize(FILE* f) const;
    void deserialize(FILE* f);
};

} // namespace NanoDB

#endif // NANODB_PAGE_H
