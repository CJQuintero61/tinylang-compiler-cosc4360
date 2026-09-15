#pragma once
#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;

    char peek(size_t offset = 0) const;
    bool atEnd() const;
    char advance();

    void skipWhitespaceAndComments();
    Token lexIdentifierOrKeyword();
    Token lexInteger();

    Token makeToken(TokenKind kind, const std::string& lexeme,
                    int line, int column) const;
};
