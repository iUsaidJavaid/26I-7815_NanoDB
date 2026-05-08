#include "parser/ExpressionEvaluator.h"
#include <cstdio>

namespace NanoDB {

ExpressionEvaluator::ExpressionEvaluator() {
}

ExpressionEvaluator::~ExpressionEvaluator() {
}

bool ExpressionEvaluator::evaluate(Token* postfixTokens, int count, const Row& row, const TableSchema& schema) {
    Stack<Field*> evalStack(256);
    
    for (int i = 0; i < count; ++i) {
        Token token = postfixTokens[i];
        
        if (token.type == IDENTIFIER) {
            Field* field = resolveIdentifier(token.value, row, schema);
            if (field != nullptr) {
                evalStack.push(field->clone());
            }
        } else if (token.type == NUMBER_INT) {
            int val = 0;
            int pos = 0;
            int sign = 1;
            if (token.value[pos] == '-') {
                sign = -1;
                ++pos;
            }
            while (token.value[pos] != '\0') {
                val = val * 10 + (token.value[pos] - '0');
                ++pos;
            }
            evalStack.push(new IntField(val * sign));
        } else if (token.type == NUMBER_FLOAT) {
            float val = 0.0f;
            float decimal = 0.1f;
            int pos = 0;
            int sign = 1;
            if (token.value[pos] == '-') {
                sign = -1;
                ++pos;
            }
            bool afterDecimal = false;
            while (token.value[pos] != '\0') {
                if (token.value[pos] == '.') {
                    afterDecimal = true;
                } else if (afterDecimal) {
                    val += (token.value[pos] - '0') * decimal;
                    decimal *= 0.1f;
                } else {
                    val = val * 10.0f + (token.value[pos] - '0');
                }
                ++pos;
            }
            evalStack.push(new FloatField(val * sign));
        } else if (token.type == STRING_LITERAL) {
            evalStack.push(new StringField(token.value));
        } else if (token.type == EQUALS) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = (left != nullptr && right != nullptr && left->equals(right));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == NOT_EQUALS) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = !(left != nullptr && right != nullptr && left->equals(right));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == LESS) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = (left != nullptr && right != nullptr && left->lessThan(right));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == GREATER) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = (left != nullptr && right != nullptr && right->lessThan(left));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == LESS_EQ) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = !(left != nullptr && right != nullptr && right->lessThan(left));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == GREATER_EQ) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool result = !(left != nullptr && right != nullptr && left->lessThan(right));
            delete left;
            delete right;
            evalStack.push(new IntField(result ? 1 : 0));
        } else if (token.type == AND) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool leftVal = false;
            bool rightVal = false;
            if (left != nullptr && left->getType() == DataType::INT) {
                leftVal = ((IntField*)left)->getValue() != 0;
            }
            if (right != nullptr && right->getType() == DataType::INT) {
                rightVal = ((IntField*)right)->getValue() != 0;
            }
            delete left;
            delete right;
            evalStack.push(new IntField((leftVal && rightVal) ? 1 : 0));
        } else if (token.type == OR) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            bool leftVal = false;
            bool rightVal = false;
            if (left != nullptr && left->getType() == DataType::INT) {
                leftVal = ((IntField*)left)->getValue() != 0;
            }
            if (right != nullptr && right->getType() == DataType::INT) {
                rightVal = ((IntField*)right)->getValue() != 0;
            }
            delete left;
            delete right;
            evalStack.push(new IntField((leftVal || rightVal) ? 1 : 0));
        } else if (token.type == PLUS || token.type == MINUS || token.type == MULTIPLY || token.type == MODULO) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            
            if (left == nullptr || right == nullptr) {
                delete left;
                delete right;
                evalStack.push(new IntField(0));
                continue;
            }
            
            Field* result = nullptr;
            
            if (isNumeric(left) && isNumeric(right)) {
                if (left->getType() == DataType::FLOAT || right->getType() == DataType::FLOAT) {
                    Field* leftFloat = coerceToFloat(left);
                    Field* rightFloat = coerceToFloat(right);
                    float lval = ((FloatField*)leftFloat)->getValue();
                    float rval = ((FloatField*)rightFloat)->getValue();
                    float res = 0.0f;
                    if (token.type == PLUS) {
                        res = lval + rval;
                    } else if (token.type == MINUS) {
                        res = lval - rval;
                    } else if (token.type == MULTIPLY) {
                        res = lval * rval;
                    } else if (token.type == MODULO) {
                        res = (int)lval % (int)rval;
                    }
                    result = new FloatField(res);
                    delete leftFloat;
                    delete rightFloat;
                } else {
                    int lval = ((IntField*)left)->getValue();
                    int rval = ((IntField*)right)->getValue();
                    int res = 0;
                    if (token.type == PLUS) {
                        res = lval + rval;
                    } else if (token.type == MINUS) {
                        res = lval - rval;
                    } else if (token.type == MULTIPLY) {
                        res = lval * rval;
                    } else if (token.type == MODULO) {
                        res = lval % rval;
                    }
                    result = new IntField(res);
                }
            }
            
            delete left;
            delete right;
            
            if (result != nullptr) {
                evalStack.push(result);
            } else {
                evalStack.push(new IntField(0));
            }
        }
    }
    
    Field* finalResult = evalStack.pop();
    bool result = false;
    if (finalResult != nullptr && finalResult->getType() == DataType::INT) {
        result = ((IntField*)finalResult)->getValue() != 0;
    }
    delete finalResult;
    
    while (!evalStack.isEmpty()) {
        delete evalStack.pop();
    }
    
    return result;
}

Field* ExpressionEvaluator::evaluateArithmetic(Token* postfixTokens, int count, const Row& row, const TableSchema& schema) {
    Stack<Field*> evalStack(256);
    
    for (int i = 0; i < count; ++i) {
        Token token = postfixTokens[i];
        
        if (token.type == IDENTIFIER) {
            Field* field = resolveIdentifier(token.value, row, schema);
            if (field != nullptr) {
                evalStack.push(field->clone());
            }
        } else if (token.type == NUMBER_INT) {
            int val = 0;
            int pos = 0;
            int sign = 1;
            if (token.value[pos] == '-') {
                sign = -1;
                ++pos;
            }
            while (token.value[pos] != '\0') {
                val = val * 10 + (token.value[pos] - '0');
                ++pos;
            }
            evalStack.push(new IntField(val * sign));
        } else if (token.type == NUMBER_FLOAT) {
            float val = 0.0f;
            float decimal = 0.1f;
            int pos = 0;
            int sign = 1;
            if (token.value[pos] == '-') {
                sign = -1;
                ++pos;
            }
            bool afterDecimal = false;
            while (token.value[pos] != '\0') {
                if (token.value[pos] == '.') {
                    afterDecimal = true;
                } else if (afterDecimal) {
                    val += (token.value[pos] - '0') * decimal;
                    decimal *= 0.1f;
                } else {
                    val = val * 10.0f + (token.value[pos] - '0');
                }
                ++pos;
            }
            evalStack.push(new FloatField(val * sign));
        } else if (token.type == STRING_LITERAL) {
            evalStack.push(new StringField(token.value));
        } else if (token.type == PLUS || token.type == MINUS || token.type == MULTIPLY || token.type == MODULO) {
            Field* right = evalStack.pop();
            Field* left = evalStack.pop();
            
            if (left == nullptr || right == nullptr) {
                delete left;
                delete right;
                evalStack.push(new IntField(0));
                continue;
            }
            
            Field* result = nullptr;
            
            if (isNumeric(left) && isNumeric(right)) {
                if (left->getType() == DataType::FLOAT || right->getType() == DataType::FLOAT) {
                    Field* leftFloat = coerceToFloat(left);
                    Field* rightFloat = coerceToFloat(right);
                    float lval = ((FloatField*)leftFloat)->getValue();
                    float rval = ((FloatField*)rightFloat)->getValue();
                    float res = 0.0f;
                    if (token.type == PLUS) {
                        res = lval + rval;
                    } else if (token.type == MINUS) {
                        res = lval - rval;
                    } else if (token.type == MULTIPLY) {
                        res = lval * rval;
                    } else if (token.type == MODULO) {
                        res = (int)lval % (int)rval;
                    }
                    result = new FloatField(res);
                    delete leftFloat;
                    delete rightFloat;
                } else {
                    int lval = ((IntField*)left)->getValue();
                    int rval = ((IntField*)right)->getValue();
                    int res = 0;
                    if (token.type == PLUS) {
                        res = lval + rval;
                    } else if (token.type == MINUS) {
                        res = lval - rval;
                    } else if (token.type == MULTIPLY) {
                        res = lval * rval;
                    } else if (token.type == MODULO) {
                        res = lval % rval;
                    }
                    result = new IntField(res);
                }
            }
            
            delete left;
            delete right;
            
            if (result != nullptr) {
                evalStack.push(result);
            } else {
                evalStack.push(new IntField(0));
            }
        }
    }
    
    Field* finalResult = evalStack.pop();
    
    while (!evalStack.isEmpty()) {
        delete evalStack.pop();
    }
    
    return finalResult;
}

Field* ExpressionEvaluator::resolveIdentifier(const char* name, const Row& row, const TableSchema& schema) {
    int colIndex = schema.getColumnIndex(name);
    if (colIndex < 0 || colIndex >= row.columnCount) {
        return nullptr;
    }
    return row.fields[colIndex];
}

Field* ExpressionEvaluator::coerceToFloat(Field* field) {
    if (field == nullptr) {
        return new FloatField(0.0f);
    }
    if (field->getType() == DataType::FLOAT) {
        return field->clone();
    }
    if (field->getType() == DataType::INT) {
        return new FloatField((float)((IntField*)field)->getValue());
    }
    return new FloatField(0.0f);
}

Field* ExpressionEvaluator::coerceToInt(Field* field) {
    if (field == nullptr) {
        return new IntField(0);
    }
    if (field->getType() == DataType::INT) {
        return field->clone();
    }
    if (field->getType() == DataType::FLOAT) {
        return new IntField((int)((FloatField*)field)->getValue());
    }
    return new IntField(0);
}

bool ExpressionEvaluator::isNumeric(Field* field) {
    if (field == nullptr) {
        return false;
    }
    return field->getType() == DataType::INT || field->getType() == DataType::FLOAT;
}

} // namespace NanoDB
