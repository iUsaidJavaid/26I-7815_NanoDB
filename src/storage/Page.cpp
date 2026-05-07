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
        
        IntField* intField = dynamic_cast<IntField*>(field);
        FloatField* floatField = dynamic_cast<FloatField*>(field);
        StringField* stringField = dynamic_cast<StringField*>(field);
        
        if (intField != nullptr) {
            if (offset + 5 > PAGE_SIZE) {
                return;
            }
            data[offset++] = 0x01;
            int value = intField->getValue();
            data[offset++] = (value >> 24) & 0xFF;
            data[offset++] = (value >> 16) & 0xFF;
            data[offset++] = (value >> 8) & 0xFF;
            data[offset++] = value & 0xFF;
        } else if (floatField != nullptr) {
            if (offset + 5 > PAGE_SIZE) {
                return;
            }
            data[offset++] = 0x02;
            float value = floatField->getValue();
            char* floatBytes = (char*)&value;
            data[offset++] = floatBytes[0];
            data[offset++] = floatBytes[1];
            data[offset++] = floatBytes[2];
            data[offset++] = floatBytes[3];
        } else if (stringField != nullptr) {
            const char* strValue = stringField->getValue();
            int len = 0;
            while (strValue[len] != '\0') {
                ++len;
            }
            
            if (offset + 3 + len > PAGE_SIZE) {
                return;
            }
            
            data[offset++] = 0x03;
            data[offset++] = (len >> 8) & 0xFF;
            data[offset++] = len & 0xFF;
            
            for (int j = 0; j < len; ++j) {
                data[offset++] = strValue[j];
            }
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
        
        unsigned char typeTag = data[offset++];
        
        if (typeTag == 0x01) {
            if (offset + 4 > PAGE_SIZE) {
                return false;
            }
            int value = ((unsigned char)data[offset] << 24) |
                        ((unsigned char)data[offset + 1] << 16) |
                        ((unsigned char)data[offset + 2] << 8) |
                        (unsigned char)data[offset + 3];
            offset += 4;
            out.addField(new IntField(value));
        } else if (typeTag == 0x02) {
            if (offset + 4 > PAGE_SIZE) {
                return false;
            }
            char floatBytes[4];
            floatBytes[0] = data[offset];
            floatBytes[1] = data[offset + 1];
            floatBytes[2] = data[offset + 2];
            floatBytes[3] = data[offset + 3];
            offset += 4;
            float value = *((float*)floatBytes);
            out.addField(new FloatField(value));
        } else if (typeTag == 0x03) {
            if (offset + 2 > PAGE_SIZE) {
                return false;
            }
            int len = ((unsigned char)data[offset] << 8) | (unsigned char)data[offset + 1];
            offset += 2;
            
            if (offset + len > PAGE_SIZE) {
                return false;
            }
            
            char strBuffer[StringField::MAX_LENGTH];
            for (int j = 0; j < len && j < StringField::MAX_LENGTH - 1; ++j) {
                strBuffer[j] = data[offset++];
            }
            strBuffer[len] = '\0';
            offset += len - (len < StringField::MAX_LENGTH - 1 ? len : StringField::MAX_LENGTH - 1);
            
            out.addField(new StringField(strBuffer));
        }
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
