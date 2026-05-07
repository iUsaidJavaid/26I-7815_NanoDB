#ifndef NANODB_EXPRESSION_EVALUATOR_H
#define NANODB_EXPRESSION_EVALUATOR_H

#include "common/Types.h"
#include "parser/ShuntingYard.h"

namespace NanoDB {

class ExpressionEvaluator {
public:
    ExpressionEvaluator();
    ~ExpressionEvaluator();
    
    Value evaluate(Token* postfix, int num_tokens);
    
private:
    struct EvalStack {
        Value value;
        EvalStack* next;
    };
    
    EvalStack* stack_;
    
    void push(const Value& value);
    Value pop();
    bool isEmpty();
};

} // namespace NanoDB

#endif // NANODB_EXPRESSION_EVALUATOR_H
