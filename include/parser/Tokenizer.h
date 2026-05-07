#ifndef NANODB_TOKENIZER_H
#define NANODB_TOKENIZER_H

namespace NanoDB {

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    PUNCTUATION,
    EOF_TOKEN,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    char value[64];
    int line;
    int column;
};

class Tokenizer {
public:
    Tokenizer(const char* input);
    ~Tokenizer();
    
    Token getNextToken();
    void reset();
    
private:
    const char* input_;
    int position_;
    int line_;
    int column_;
    
    bool isWhitespace(char c);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};

} // namespace NanoDB

#endif // NANODB_TOKENIZER_H
