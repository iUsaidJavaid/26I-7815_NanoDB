#include "parser/ShuntingYard.h"

namespace NanoDB {

ShuntingYard::ShuntingYard() 
    : operator_stack_(nullptr), output_queue_(nullptr), 
      output_size_(0), output_capacity_(0) {
}

ShuntingYard::~ShuntingYard() {
}

Token* ShuntingYard::parse(Token* tokens, int num_tokens) {
    return nullptr;
}

int ShuntingYard::getPrecedence(const Token& token) {
    return 0;
}

bool ShuntingYard::isLeftAssociative(const Token& token) {
    return true;
}

void ShuntingYard::pushOperator(const Token& token) {
}

Token ShuntingYard::popOperator() {
    Token token;
    return token;
}

void ShuntingYard::pushOutput(const Token& token) {
}

} // namespace NanoDB
