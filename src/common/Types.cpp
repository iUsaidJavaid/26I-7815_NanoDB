#include "common/Types.h"

namespace NanoDB {

Value::Value() : type_(ValueType::NULL_VALUE), string_value_(nullptr) {
}

Value::~Value() {
    if (string_value_ != nullptr) {
        delete[] string_value_;
    }
}

ValueType Value::getType() const {
    return type_;
}

void Value::setType(ValueType type) {
    type_ = type;
}

int Value::getInt() const {
    return int_value_;
}

void Value::setInt(int value) {
    type_ = ValueType::INTEGER;
    int_value_ = value;
}

float Value::getFloat() const {
    return float_value_;
}

void Value::setFloat(float value) {
    type_ = ValueType::FLOAT;
    float_value_ = value;
}

const char* Value::getString() const {
    return string_value_;
}

void Value::setString(const char* str) {
    type_ = ValueType::STRING;
    if (string_value_ != nullptr) {
        delete[] string_value_;
    }
    int len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    string_value_ = new char[len + 1];
    for (int i = 0; i < len; ++i) {
        string_value_[i] = str[i];
    }
    string_value_[len] = '\0';
}

bool Value::isNull() const {
    return type_ == ValueType::NULL_VALUE;
}

void Value::setNull() {
    type_ = ValueType::NULL_VALUE;
}

} // namespace NanoDB
