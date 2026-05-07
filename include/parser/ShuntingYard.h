#ifndef NANODB_SHUNTING_YARD_H
#define NANODB_SHUNTING_YARD_H

#include "parser/Tokenizer.h"

namespace NanoDB {

class ShuntingYard {
public:
    struct StackNode {
        Token token;
        StackNode* next;
    };
    
    ShuntingYard();
    ~ShuntingYard();
    
    Token* parse(Token* tokens, int num_tokens);
    
private:
    StackNode* operator_stack_;
    Token* output_queue_;
    int output_size_;
    int output_capacity_;
    
    int getPrecedence(const Token& token);
    bool isLeftAssociative(const Token& token);
    void pushOperator(const Token& token);
    Token popOperator();
    void pushOutput(const Token& token);
};

} // namespace NanoDB

#endif // NANODB_SHUNTING_YARD_H
