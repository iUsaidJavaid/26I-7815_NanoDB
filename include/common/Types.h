#ifndef NANODB_TYPES_H
#define NANODB_TYPES_H

namespace NanoDB {

enum class ValueType {
    INTEGER,
    FLOAT,
    STRING,
    NULL_VALUE
};

class Value {
public:
    Value();
    ~Value();
    
    ValueType getType() const;
    void setType(ValueType type);
    
    int getInt() const;
    void setInt(int value);
    
    float getFloat() const;
    void setFloat(float value);
    
    const char* getString() const;
    void setString(const char* str);
    
    bool isNull() const;
    void setNull();

private:
    ValueType type_;
    union {
        int int_value_;
        float float_value_;
    };
    char* string_value_;
};

} // namespace NanoDB

#endif // NANODB_TYPES_H
