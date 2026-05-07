#include "parser/Tokenizer.h"

namespace NanoDB {

Tokenizer::Tokenizer(const char* input) 
    : input_(input), position_(0), line_(1), column_(1) {
}

Tokenizer::~Tokenizer() {
}

Token Tokenizer::getNextToken() {
    Token token;
    token.type = TokenType::UNKNOWN;
    return token;
}

void Tokenizer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

bool Tokenizer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Tokenizer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Tokenizer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Tokenizer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

} // namespace NanoDB
