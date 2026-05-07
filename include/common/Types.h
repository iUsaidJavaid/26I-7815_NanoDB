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
    virtual ~Field() = default;
    
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
