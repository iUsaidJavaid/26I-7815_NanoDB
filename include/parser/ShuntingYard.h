#ifndef NANODB_SHUNTING_YARD_H
#define NANODB_SHUNTING_YARD_H

#include "parser/Tokenizer.h"
#include "parser/Stack.h"
#include "parser/Queue.h"
#include "common/Logger.h"

namespace NanoDB {

class ShuntingYard {
public:
    ShuntingYard();
    ~ShuntingYard();
    
    Token* convert(Token* infixTokens, int tokenCount, int& outCount);
    void printPostfix(Token* postfix, int count) const;
    
private:
    bool isOperator(TokenType type) const;
    bool isOperand(TokenType type) const;
};

} // namespace NanoDB

#endif // NANODB_SHUNTING_YARD_H
