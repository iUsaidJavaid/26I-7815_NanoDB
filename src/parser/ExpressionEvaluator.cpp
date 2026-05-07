#include "parser/ExpressionEvaluator.h"

namespace NanoDB {

ExpressionEvaluator::ExpressionEvaluator() : stack_(nullptr) {
}

ExpressionEvaluator::~ExpressionEvaluator() {
}

Value ExpressionEvaluator::evaluate(Token* postfix, int num_tokens) {
    Value value;
    return value;
}

void ExpressionEvaluator::push(const Value& value) {
}

Value ExpressionEvaluator::pop() {
    Value value;
    return value;
}

bool ExpressionEvaluator::isEmpty() {
    return true;
}

} // namespace NanoDB
