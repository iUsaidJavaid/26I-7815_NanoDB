#ifndef NANODB_EXPRESSION_EVALUATOR_H
#define NANODB_EXPRESSION_EVALUATOR_H

#include "common/Types.h"
#include "parser/Tokenizer.h"
#include "parser/Stack.h"
#include "catalog/SystemCatalog.h"

namespace NanoDB {

class ExpressionEvaluator {
public:
    ExpressionEvaluator();
    ~ExpressionEvaluator();
    
    bool evaluate(Token* postfixTokens, int count, const Row& row, const TableSchema& schema);
    Field* evaluateArithmetic(Token* postfixTokens, int count, const Row& row, const TableSchema& schema);
    
private:
    Field* resolveIdentifier(const char* name, const Row& row, const TableSchema& schema);
    Field* coerceToFloat(Field* field);
    Field* coerceToInt(Field* field);
    bool isNumeric(Field* field);
};

} // namespace NanoDB

#endif // NANODB_EXPRESSION_EVALUATOR_H
