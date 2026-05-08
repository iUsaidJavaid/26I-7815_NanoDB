#include "storage/Page.h"

namespace NanoDB {

void Page::writeRow(const Row& row, int& offset) {
    if (offset + 1 > PAGE_SIZE) {
        return;
    }

    data[offset++] = (char)row.columnCount;

    for (int i = 0; i < row.columnCount; ++i) {
        Field* field = row.fields[i];
        if (field == nullptr) {
            continue;
        }

        int written = field->serialize(data + offset, PAGE_SIZE - offset);
        if (written > 0) {
            offset += written;
        }
    }

    usedBytes = offset;
    isDirty = true;
}

bool Page::readRow(Row& out, int& offset, const ColumnSchema* schema, int colCount) {
    if (offset >= PAGE_SIZE) {
        return false;
    }

    int fieldCount = (unsigned char)data[offset++];

    for (int i = 0; i < fieldCount && i < Row::MAX_COLUMNS; ++i) {
        if (offset >= PAGE_SIZE) {
            return false;
        }

        unsigned char typeTag = data[offset];
        Field* field = Field::createFromTypeTag(typeTag);

        if (field == nullptr) {
            return false;
        }

        int bytesRead = field->deserialize(data + offset, PAGE_SIZE - offset);
        if (bytesRead < 0) {
            delete field;
            return false;
        }

        offset += bytesRead;
        out.addField(field);
    }

    return true;
}

void Page::clear() {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        data[i] = 0;
    }
    usedBytes = 0;
    isDirty = false;
    isPinned = false;
}

void Page::serialize(FILE* f) const {
    if (f == nullptr) {
        return;
    }
    
    fwrite(&pageId, sizeof(int), 1, f);
    fwrite(&isDirty, sizeof(bool), 1, f);
    fwrite(&isPinned, sizeof(bool), 1, f);
    fwrite(&usedBytes, sizeof(int), 1, f);
    fwrite(data, sizeof(char), usedBytes, f);
    
    char logMessage[128];
    int pos = 0;
    const char* prefix = "[LOG] Page ";
    while (prefix[pos] != '\0') {
        logMessage[pos] = prefix[pos];
        ++pos;
    }
    
    char idStr[32];
    int idPos = 0;
    int tempId = pageId;
    if (tempId == 0) {
        idStr[idPos++] = '0';
    } else {
        char idBuffer[32];
        int idBufPos = 0;
        while (tempId > 0) {
            idBuffer[idBufPos++] = '0' + (tempId % 10);
            tempId /= 10;
        }
        for (int i = idBufPos - 1; i >= 0; --i) {
            idStr[idPos++] = idBuffer[i];
        }
    }
    idStr[idPos] = '\0';
    
    int idIdx = 0;
    while (idStr[idIdx] != '\0') {
        logMessage[pos++] = idStr[idIdx++];
    }
    
    const char* middle = " serialized to disk (";
    int midIdx = 0;
    while (middle[midIdx] != '\0') {
        logMessage[pos++] = middle[midIdx++];
    }
    
    char bytesStr[32];
    int bytesPos = 0;
    int tempBytes = usedBytes;
    if (tempBytes == 0) {
        bytesStr[bytesPos++] = '0';
    } else {
        char bytesBuffer[32];
        int bytesBufPos = 0;
        while (tempBytes > 0) {
            bytesBuffer[bytesBufPos++] = '0' + (tempBytes % 10);
            tempBytes /= 10;
        }
        for (int i = bytesBufPos - 1; i >= 0; --i) {
            bytesStr[bytesPos++] = bytesBuffer[i];
        }
    }
    bytesStr[bytesPos] = '\0';
    
    int bytesIdx = 0;
    while (bytesStr[bytesIdx] != '\0') {
        logMessage[pos++] = bytesStr[bytesIdx++];
    }
    
    const char* suffix = " bytes)\n";
    int sufIdx = 0;
    while (suffix[sufIdx] != '\0') {
        logMessage[pos++] = suffix[sufIdx++];
    }
    logMessage[pos] = '\0';
    
    Logger::getInstance()->logLog(logMessage);
}

void Page::deserialize(FILE* f) {
    if (f == nullptr) {
        return;
    }
    
    fread(&pageId, sizeof(int), 1, f);
    fread(&isDirty, sizeof(bool), 1, f);
    fread(&isPinned, sizeof(bool), 1, f);
    fread(&usedBytes, sizeof(int), 1, f);
    
    if (usedBytes > PAGE_SIZE) {
        usedBytes = PAGE_SIZE;
    }
    
    fread(data, sizeof(char), usedBytes, f);
}

} // namespace NanoDB
