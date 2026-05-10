#ifndef NANODB_TYPES_H
#define NANODB_TYPES_H

#include <cstdio>

namespace NanoDB {

enum class DataType {
    INT,
    FLOAT,
    VARCHAR
};

struct ColumnSchema {
    char name[64];
    DataType type;
    int maxLen;
};

class Field {
public:
    virtual bool equals(const Field* other) const = 0;
    virtual bool lessThan(const Field* other) const = 0;
    virtual void print() const = 0;
    virtual Field* clone() const = 0;
    virtual int serialize(char* buffer, int maxLen) const = 0;
    virtual int deserialize(const char* buffer, int maxLen) = 0;
    virtual ~Field() = default;
    
    static Field* createFromTypeTag(unsigned char typeTag);
    
    bool operator==(const Field* other) const {
        return equals(other);
    }
    
    bool operator!=(const Field* other) const {
        return !equals(other);
    }
    
    bool operator<(const Field* other) const {
        return lessThan(other);
    }
    
    bool operator>(const Field* other) const {
        return other->lessThan(this);
    }
    
    bool operator<=(const Field* other) const {
        return lessThan(other) || equals(other);
    }
    
    bool operator>=(const Field* other) const {
        return other->lessThan(this) || equals(other);
    }
};

class IntField : public Field {
public:
    IntField(int value) : value_(value) {}
    
    bool equals(const Field* other) const override {
        const IntField* otherInt = dynamic_cast<const IntField*>(other);
        if (otherInt == nullptr) {
            return false;
        }
        return value_ == otherInt->value_;
    }
    
    bool lessThan(const Field* other) const override {
        const IntField* otherInt = dynamic_cast<const IntField*>(other);
        if (otherInt == nullptr) {
            return false;
        }
        return value_ < otherInt->value_;
    }
    
    void print() const override {
        printf("%d", value_);
    }
    
    Field* clone() const override {
        return new IntField(value_);
    }

    int serialize(char* buffer, int maxLen) const override {
        if (maxLen < 5) return -1;
        buffer[0] = 0x01;
        buffer[1] = (value_ >> 24) & 0xFF;
        buffer[2] = (value_ >> 16) & 0xFF;
        buffer[3] = (value_ >> 8) & 0xFF;
        buffer[4] = value_ & 0xFF;
        return 5;
    }

    int deserialize(const char* buffer, int maxLen) override {
        if (maxLen < 5 || buffer[0] != 0x01) return -1;
        value_ = ((unsigned char)buffer[1] << 24) |
                 ((unsigned char)buffer[2] << 16) |
                 ((unsigned char)buffer[3] << 8) |
                 (unsigned char)buffer[4];
        return 5;
    }

    int getValue() const { return value_; }
    void setValue(int value) { value_ = value; }

private:
    int value_;
};

class FloatField : public Field {
public:
    FloatField(float value) : value_(value) {}
    
    bool equals(const Field* other) const override {
        const FloatField* otherFloat = dynamic_cast<const FloatField*>(other);
        if (otherFloat == nullptr) {
            return false;
        }
        return value_ == otherFloat->value_;
    }
    
    bool lessThan(const Field* other) const override {
        const FloatField* otherFloat = dynamic_cast<const FloatField*>(other);
        if (otherFloat == nullptr) {
            return false;
        }
        return value_ < otherFloat->value_;
    }
    
    void print() const override {
        printf("%f", value_);
    }
    
    Field* clone() const override {
        return new FloatField(value_);
    }

    int serialize(char* buffer, int maxLen) const override {
        if (maxLen < 5) return -1;
        buffer[0] = 0x02;
        char* floatBytes = (char*)&value_;
        buffer[1] = floatBytes[0];
        buffer[2] = floatBytes[1];
        buffer[3] = floatBytes[2];
        buffer[4] = floatBytes[3];
        return 5;
    }

    int deserialize(const char* buffer, int maxLen) override {
        if (maxLen < 5 || buffer[0] != 0x02) return -1;
        char floatBytes[4];
        floatBytes[0] = buffer[1];
        floatBytes[1] = buffer[2];
        floatBytes[2] = buffer[3];
        floatBytes[3] = buffer[4];
        value_ = *((float*)floatBytes);
        return 5;
    }

    float getValue() const { return value_; }
    void setValue(float value) { value_ = value; }

private:
    float value_;
};

class StringField : public Field {
public:
    static const int MAX_LENGTH = 256;
    
    StringField(const char* value) {
        int i = 0;
        while (value[i] != '\0' && i < MAX_LENGTH - 1) {
            value_[i] = value[i];
            ++i;
        }
        value_[i] = '\0';
    }
    
    bool equals(const Field* other) const override {
        const StringField* otherString = dynamic_cast<const StringField*>(other);
        if (otherString == nullptr) {
            return false;
        }
        int i = 0;
        while (value_[i] != '\0' && otherString->value_[i] != '\0') {
            if (value_[i] != otherString->value_[i]) {
                return false;
            }
            ++i;
        }
        return value_[i] == '\0' && otherString->value_[i] == '\0';
    }
    
    bool lessThan(const Field* other) const override {
        const StringField* otherString = dynamic_cast<const StringField*>(other);
        if (otherString == nullptr) {
            return false;
        }
        int i = 0;
        while (value_[i] != '\0' && otherString->value_[i] != '\0') {
            if (value_[i] < otherString->value_[i]) {
                return true;
            }
            if (value_[i] > otherString->value_[i]) {
                return false;
            }
            ++i;
        }
        if (value_[i] == '\0' && otherString->value_[i] != '\0') {
            return true;
        }
        return false;
    }
    
    void print() const override {
        printf("%s", value_);
    }
    
    Field* clone() const override {
        return new StringField(value_);
    }

    int serialize(char* buffer, int maxLen) const override {
        int len = 0;
        while (value_[len] != '\0') ++len;
        if (maxLen < 3 + len) return -1;
        buffer[0] = 0x03;
        buffer[1] = (len >> 8) & 0xFF;
        buffer[2] = len & 0xFF;
        for (int i = 0; i < len; ++i) {
            buffer[3 + i] = value_[i];
        }
        return 3 + len;
    }

    int deserialize(const char* buffer, int maxLen) override {
        if (maxLen < 3 || buffer[0] != 0x03) return -1;
        int len = ((unsigned char)buffer[1] << 8) | (unsigned char)buffer[2];
        if (maxLen < 3 + len) return -1;
        for (int i = 0; i < len && i < MAX_LENGTH - 1; ++i) {
            value_[i] = buffer[3 + i];
        }
        value_[len < MAX_LENGTH - 1 ? len : MAX_LENGTH - 1] = '\0';
        return 3 + len;
    }

    const char* getValue() const { return value_; }
    void setValue(const char* value) {
        int i = 0;
        while (value[i] != '\0' && i < MAX_LENGTH - 1) {
            value_[i] = value[i];
            ++i;
        }
        value_[i] = '\0';
    }

private:
    char value_[MAX_LENGTH];
};

struct Row {
    static const int MAX_COLUMNS = 16;
    
    Field* fields[MAX_COLUMNS];
    int columnCount;
    
    Row() : columnCount(0) {
        for (int i = 0; i < MAX_COLUMNS; ++i) {
            fields[i] = nullptr;
        }
    }
    
    ~Row() {
        for (int i = 0; i < columnCount; ++i) {
            if (fields[i] != nullptr) {
                delete fields[i];
                fields[i] = nullptr;
            }
        }
    }
    
    void clear() {
        for (int i = 0; i < columnCount; ++i) {
            if (fields[i] != nullptr) {
                delete fields[i];
                fields[i] = nullptr;
            }
        }
        columnCount = 0;
    }
    
    bool addField(Field* field) {
        if (columnCount >= MAX_COLUMNS || field == nullptr) {
            return false;
        }
        fields[columnCount] = field;
        ++columnCount;
        return true;
    }
    
    Field* getField(int index) {
        if (index < 0 || index >= columnCount) {
            return nullptr;
        }
        return fields[index];
    }
    
    const Field* getField(int index) const {
        if (index < 0 || index >= columnCount) {
            return nullptr;
        }
        return fields[index];
    }
    
    void print() const {
        printf("| ");
        for (int i = 0; i < columnCount; ++i) {
            if (fields[i] != nullptr) {
                fields[i]->print();
            }
            printf(" | ");
        }
        printf("\n");
    }
};

} // namespace NanoDB

#endif // NANODB_TYPES_H
