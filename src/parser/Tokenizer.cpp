#include "parser/Tokenizer.h"
#include <cstdio>

namespace NanoDB {

Tokenizer::Tokenizer(const char* input) 
    : input_(input), position_(0), length_(0) {
    if (input_ != nullptr) {
        while (input_[length_] != '\0') {
            ++length_;
        }
    }
}

Tokenizer::~Tokenizer() {
}

Token Tokenizer::nextToken() {
    skipWhitespace();
    
    if (position_ >= length_) {
        Token token;
        token.type = END_OF_INPUT;
        return token;
    }
    
    char c = input_[position_];
    
    if (c == '(') {
        Token token;
        token.type = LPAREN;
        token.value[0] = '(';
        token.value[1] = '\0';
        ++position_;
        return token;
    }
    
    if (c == ')') {
        Token token;
        token.type = RPAREN;
        token.value[0] = ')';
        token.value[1] = '\0';
        ++position_;
        return token;
    }
    
    if (c == '"') {
        return readString();
    }
    
    if (isAlpha(c) || c == '_') {
        return readIdentifierOrKeyword();
    }
    
    if (isDigit(c)) {
        return readNumber();
    }
    
    if (c == '+' || c == '-' || c == '*' || c == '%' || c == '<' || c == '>' || c == '=' || c == '!') {
        return readOperator();
    }
    
    Token token;
    token.type = IDENTIFIER;
    token.value[0] = c;
    token.value[1] = '\0';
    ++position_;
    return token;
}

Token* Tokenizer::tokenizeAll(int& count) {
    int originalPosition = position_;
    position_ = 0;
    
    Token* tokens = new Token[1024];
    count = 0;
    
    while (true) {
        Token token = nextToken();
        if (token.type == END_OF_INPUT) {
            break;
        }
        tokens[count] = token;
        ++count;
    }
    
    position_ = originalPosition;
    return tokens;
}

bool Tokenizer::hasMore() const {
    return position_ < length_;
}

void Tokenizer::skipWhitespace() {
    while (position_ < length_) {
        char c = input_[position_];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            ++position_;
        } else {
            break;
        }
    }
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

Token Tokenizer::readIdentifierOrKeyword() {
    Token token;
    int pos = 0;
    
    while (position_ < length_ && (isAlphaNumeric(input_[position_]) || input_[position_] == '_' || input_[position_] == '.')) {
        if (pos < 127) {
            token.value[pos++] = input_[position_];
        }
        ++position_;
    }
    token.value[pos] = '\0';
    
    token.type = IDENTIFIER;
    
    if (token.value[0] == 'S' || token.value[0] == 's') {
        int i = 0;
        const char* select = "SELECT";
        bool match = true;
        while (select[i] != '\0' && token.value[i] != '\0') {
            char c1 = select[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && select[i] == '\0' && token.value[i] == '\0') {
            token.type = SELECT;
        }
    }
    
    if (token.value[0] == 'I' || token.value[0] == 'i') {
        int i = 0;
        const char* insert = "INSERT";
        bool match = true;
        while (insert[i] != '\0' && token.value[i] != '\0') {
            char c1 = insert[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && insert[i] == '\0' && token.value[i] == '\0') {
            token.type = INSERT;
        }
    }
    
    if (token.value[0] == 'U' || token.value[0] == 'u') {
        int i = 0;
        const char* update = "UPDATE";
        bool match = true;
        while (update[i] != '\0' && token.value[i] != '\0') {
            char c1 = update[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && update[i] == '\0' && token.value[i] == '\0') {
            token.type = UPDATE;
        }
    }
    
    if (token.value[0] == 'D' || token.value[0] == 'd') {
        int i = 0;
        const char* del = "DELETE";
        bool match = true;
        while (del[i] != '\0' && token.value[i] != '\0') {
            char c1 = del[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && del[i] == '\0' && token.value[i] == '\0') {
            token.type = DELETE;
        }
    }
    
    if (token.value[0] == 'W' || token.value[0] == 'w') {
        int i = 0;
        const char* where = "WHERE";
        bool match = true;
        while (where[i] != '\0' && token.value[i] != '\0') {
            char c1 = where[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && where[i] == '\0' && token.value[i] == '\0') {
            token.type = WHERE;
        }
    }
    
    if (token.value[0] == 'F' || token.value[0] == 'f') {
        int i = 0;
        const char* from = "FROM";
        bool match = true;
        while (from[i] != '\0' && token.value[i] != '\0') {
            char c1 = from[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && from[i] == '\0' && token.value[i] == '\0') {
            token.type = FROM;
        }
    }
    
    if (token.value[0] == 'J' || token.value[0] == 'j') {
        int i = 0;
        const char* join = "JOIN";
        bool match = true;
        while (join[i] != '\0' && token.value[i] != '\0') {
            char c1 = join[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && join[i] == '\0' && token.value[i] == '\0') {
            token.type = JOIN;
        }
    }
    
    if (token.value[0] == 'O' || token.value[0] == 'o') {
        int i = 0;
        const char* on = "ON";
        bool match = true;
        while (on[i] != '\0' && token.value[i] != '\0') {
            char c1 = on[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && on[i] == '\0' && token.value[i] == '\0') {
            token.type = ON;
        }
    }
    
    if (token.value[0] == 'A' || token.value[0] == 'a') {
        int i = 0;
        const char* and_str = "AND";
        bool match = true;
        while (and_str[i] != '\0' && token.value[i] != '\0') {
            char c1 = and_str[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && and_str[i] == '\0' && token.value[i] == '\0') {
            token.type = AND;
        }
    }
    
    if (token.value[0] == 'O' || token.value[0] == 'o') {
        int i = 0;
        const char* or_str = "OR";
        bool match = true;
        while (or_str[i] != '\0' && token.value[i] != '\0') {
            char c1 = or_str[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && or_str[i] == '\0' && token.value[i] == '\0') {
            token.type = OR;
        }
    }
    
    if (token.value[0] == 'N' || token.value[0] == 'n') {
        int i = 0;
        const char* not_str = "NOT";
        bool match = true;
        while (not_str[i] != '\0' && token.value[i] != '\0') {
            char c1 = not_str[i];
            char c2 = token.value[i];
            if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
            if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
            if (c1 != c2) {
                match = false;
                break;
            }
            ++i;
        }
        if (match && not_str[i] == '\0' && token.value[i] == '\0') {
            token.type = NOT;
        }
    }
    
    setPrecedence(token);
    return token;
}

Token Tokenizer::readNumber() {
    Token token;
    int pos = 0;
    bool isFloat = false;
    
    while (position_ < length_ && (isDigit(input_[position_]) || input_[position_] == '.')) {
        if (input_[position_] == '.') {
            isFloat = true;
        }
        if (pos < 127) {
            token.value[pos++] = input_[position_];
        }
        ++position_;
    }
    token.value[pos] = '\0';
    
    token.type = isFloat ? NUMBER_FLOAT : NUMBER_INT;
    return token;
}

Token Tokenizer::readString() {
    Token token;
    ++position_;
    
    int pos = 0;
    while (position_ < length_ && input_[position_] != '"') {
        if (pos < 127) {
            token.value[pos++] = input_[position_];
        }
        ++position_;
    }
    token.value[pos] = '\0';
    
    if (position_ < length_ && input_[position_] == '"') {
        ++position_;
    }
    
    token.type = STRING_LITERAL;
    return token;
}

Token Tokenizer::readOperator() {
    Token token;
    char c = input_[position_];
    
    if (c == '=') {
        if (position_ + 1 < length_ && input_[position_ + 1] == '=') {
            token.type = EQUALS;
            token.value[0] = '=';
            token.value[1] = '=';
            token.value[2] = '\0';
            position_ += 2;
        } else {
            token.type = EQUALS;
            token.value[0] = '=';
            token.value[1] = '\0';
            ++position_;
        }
    } else if (c == '!') {
        if (position_ + 1 < length_ && input_[position_ + 1] == '=') {
            token.type = NOT_EQUALS;
            token.value[0] = '!';
            token.value[1] = '=';
            token.value[2] = '\0';
            position_ += 2;
        } else {
            token.type = NOT;
            token.value[0] = '!';
            token.value[1] = '\0';
            ++position_;
        }
    } else if (c == '<') {
        if (position_ + 1 < length_ && input_[position_ + 1] == '=') {
            token.type = LESS_EQ;
            token.value[0] = '<';
            token.value[1] = '=';
            token.value[2] = '\0';
            position_ += 2;
        } else {
            token.type = LESS;
            token.value[0] = '<';
            token.value[1] = '\0';
            ++position_;
        }
    } else if (c == '>') {
        if (position_ + 1 < length_ && input_[position_ + 1] == '=') {
            token.type = GREATER_EQ;
            token.value[0] = '>';
            token.value[1] = '=';
            token.value[2] = '\0';
            position_ += 2;
        } else {
            token.type = GREATER;
            token.value[0] = '>';
            token.value[1] = '\0';
            ++position_;
        }
    } else if (c == '+') {
        token.type = PLUS;
        token.value[0] = '+';
        token.value[1] = '\0';
        ++position_;
    } else if (c == '-') {
        token.type = MINUS;
        token.value[0] = '-';
        token.value[1] = '\0';
        ++position_;
    } else if (c == '*') {
        token.type = MULTIPLY;
        token.value[0] = '*';
        token.value[1] = '\0';
        ++position_;
    } else if (c == '%') {
        token.type = MODULO;
        token.value[0] = '%';
        token.value[1] = '\0';
        ++position_;
    } else {
        token.type = IDENTIFIER;
        token.value[0] = c;
        token.value[1] = '\0';
        ++position_;
    }
    
    setPrecedence(token);
    return token;
}

void Tokenizer::setPrecedence(Token& token) {
    switch (token.type) {
        case MULTIPLY:
        case MODULO:
            token.precedence = 5;
            break;
        case PLUS:
        case MINUS:
            token.precedence = 4;
            break;
        case LESS:
        case GREATER:
        case LESS_EQ:
        case GREATER_EQ:
            token.precedence = 3;
            break;
        case EQUALS:
        case NOT_EQUALS:
            token.precedence = 2;
            break;
        case AND:
            token.precedence = 1;
            break;
        case OR:
            token.precedence = 0;
            break;
        default:
            token.precedence = 0;
            break;
    }
}

#ifdef RUN_TOKENIZER_TEST
void testTokenizer() {
    const char* query = "( c_acctbal > 5000 AND c_mktsegment == \"BUILDING\" ) OR c_nationkey == 15";
    Tokenizer tokenizer(query);
    
    int count = 0;
    Token* tokens = tokenizer.tokenizeAll(count);
    
    const char* typeNames[] = {
        "SELECT", "INSERT", "UPDATE", "DELETE", "WHERE", "FROM", "JOIN", "ON",
        "AND", "OR", "NOT", "LPAREN", "RPAREN", "EQUALS", "NOT_EQUALS",
        "LESS", "GREATER", "LESS_EQ", "GREATER_EQ", "PLUS", "MINUS",
        "MULTIPLY", "MODULO", "NUMBER_INT", "NUMBER_FLOAT", "STRING_LITERAL",
        "IDENTIFIER", "END_OF_INPUT"
    };
    
    for (int i = 0; i < count; ++i) {
        printf("Token: %s, Type: %s, Precedence: %d\n", 
               tokens[i].value, typeNames[tokens[i].type], tokens[i].precedence);
    }
    
    delete[] tokens;
}
#endif

} // namespace NanoDB
