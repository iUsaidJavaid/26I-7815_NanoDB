#include "parser/ShuntingYard.h"
#include <cstdio>

namespace NanoDB {

ShuntingYard::ShuntingYard() {
}

ShuntingYard::~ShuntingYard() {
}

Token* ShuntingYard::convert(Token* infixTokens, int tokenCount, int& outCount) {
    Stack<Token> operatorStack(256);
    Queue<Token> outputQueue(512);
    
    for (int i = 0; i < tokenCount; ++i) {
        Token token = infixTokens[i];
        
        if (isOperand(token.type)) {
            outputQueue.enqueue(token);
        } else if (isOperator(token.type)) {
            while (!operatorStack.isEmpty()) {
                Token top = operatorStack.peek();
                if (top.type != LPAREN && top.precedence >= token.precedence) {
                    outputQueue.enqueue(operatorStack.pop());
                } else {
                    break;
                }
            }
            operatorStack.push(token);
        } else if (token.type == LPAREN) {
            operatorStack.push(token);
        } else if (token.type == RPAREN) {
            while (!operatorStack.isEmpty()) {
                Token top = operatorStack.pop();
                if (top.type == LPAREN) {
                    break;
                }
                outputQueue.enqueue(top);
            }
        }
    }
    
    while (!operatorStack.isEmpty()) {
        outputQueue.enqueue(operatorStack.pop());
    }
    
    Token* postfix = new Token[512];
    outCount = 0;
    
    while (!outputQueue.isEmpty()) {
        postfix[outCount] = outputQueue.dequeue();
        ++outCount;
    }
    
    char infixStr[1024] = {0};
    int pos = 0;
    for (int i = 0; i < tokenCount && pos < 1000; ++i) {
        int j = 0;
        while (infixTokens[i].value[j] != '\0' && pos < 1000) {
            infixStr[pos++] = infixTokens[i].value[j++];
        }
        if (i < tokenCount - 1 && pos < 1000) {
            infixStr[pos++] = ' ';
        }
    }
    infixStr[pos] = '\0';
    
    char postfixStr[1024] = {0};
    pos = 0;
    for (int i = 0; i < outCount && pos < 1000; ++i) {
        int j = 0;
        while (postfix[i].value[j] != '\0' && pos < 1000) {
            postfixStr[pos++] = postfix[i].value[j++];
        }
        if (i < outCount - 1 && pos < 1000) {
            postfixStr[pos++] = ' ';
        }
    }
    postfixStr[pos] = '\0';
    
    char logMessage[2048];
    int logPos = 0;
    const char* prefix = "[LOG] Infix \"";
    int prefixIdx = 0;
    while (prefix[prefixIdx] != '\0') {
        logMessage[logPos++] = prefix[prefixIdx++];
    }
    
    int infixIdx = 0;
    while (infixStr[infixIdx] != '\0' && logPos < 2040) {
        logMessage[logPos++] = infixStr[infixIdx++];
    }
    
    const char* middle = "\" converted to Postfix \"";
    int midIdx = 0;
    while (middle[midIdx] != '\0' && logPos < 2040) {
        logMessage[logPos++] = middle[midIdx++];
    }
    
    int postfixIdx = 0;
    while (postfixStr[postfixIdx] != '\0' && logPos < 2040) {
        logMessage[logPos++] = postfixStr[postfixIdx++];
    }
    
    logMessage[logPos++] = '"';
    logMessage[logPos++] = '\n';
    logMessage[logPos] = '\0';
    
    Logger::getInstance()->logLog(logMessage);
    
    return postfix;
}

void ShuntingYard::printPostfix(Token* postfix, int count) const {
    for (int i = 0; i < count; ++i) {
        printf("%s ", postfix[i].value);
    }
    printf("\n");
}

bool ShuntingYard::isOperator(TokenType type) const {
    return type == PLUS || type == MINUS || type == MULTIPLY || type == MODULO ||
           type == EQUALS || type == NOT_EQUALS || type == LESS || type == GREATER ||
           type == LESS_EQ || type == GREATER_EQ || type == AND || type == OR;
}

bool ShuntingYard::isOperand(TokenType type) const {
    return type == NUMBER_INT || type == NUMBER_FLOAT || type == STRING_LITERAL || type == IDENTIFIER;
}

#ifdef RUN_SHUNTING_YARD_TEST
void testShuntingYard() {
    const char* query = "( c_acctbal > 5000 AND c_mktsegment == \"BUILDING\" ) OR c_nationkey == 15";
    Tokenizer tokenizer(query);
    
    int tokenCount = 0;
    Token* infixTokens = tokenizer.tokenizeAll(tokenCount);
    
    ShuntingYard shuntingYard;
    int postfixCount = 0;
    Token* postfixTokens = shuntingYard.convert(infixTokens, tokenCount, postfixCount);
    
    printf("Postfix: ");
    shuntingYard.printPostfix(postfixTokens, postfixCount);
    
    delete[] infixTokens;
    delete[] postfixTokens;
}
#endif

} // namespace NanoDB
