#ifndef NANODB_TOKENIZER_H
#define NANODB_TOKENIZER_H

namespace NanoDB {

enum TokenType {
    SELECT,
    INSERT,
    UPDATE,
    DELETE,
    WHERE,
    FROM,
    JOIN,
    ON,
    AND,
    OR,
    NOT,
    LPAREN,
    RPAREN,
    EQUALS,
    NOT_EQUALS,
    LESS,
    GREATER,
    LESS_EQ,
    GREATER_EQ,
    PLUS,
    MINUS,
    MULTIPLY,
    MODULO,
    NUMBER_INT,
    NUMBER_FLOAT,
    STRING_LITERAL,
    IDENTIFIER,
    END_OF_INPUT
};

struct Token {
    TokenType type;
    char value[128];
    int precedence;
    
    Token() : type(IDENTIFIER), precedence(0) {
        for (int i = 0; i < 128; ++i) {
            value[i] = '\0';
        }
    }
};

class Tokenizer {
public:
    Tokenizer(const char* input);
    ~Tokenizer();
    
    Token nextToken();
    Token* tokenizeAll(int& count);
    bool hasMore() const;
    
private:
    const char* input_;
    int position_;
    int length_;
    
    void skipWhitespace();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token readOperator();
    void setPrecedence(Token& token);
};

} // namespace NanoDB

#endif // NANODB_TOKENIZER_H
