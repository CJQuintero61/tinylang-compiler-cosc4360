#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

const char* tokenKindName(TokenKind kind) {
    switch (kind) {
        case TokenKind::End: return "End";
        case TokenKind::Identifier: return "Identifier";
        case TokenKind::Integer: return "Integer";
        case TokenKind::KwFn: return "fn";
        case TokenKind::KwLet: return "let";
        case TokenKind::KwInt: return "int";
        case TokenKind::KwBool: return "bool";
        case TokenKind::KwIf: return "if";
        case TokenKind::KwElse: return "else";
        case TokenKind::KwReturn: return "return";
        case TokenKind::KwTrue: return "true";
        case TokenKind::KwFalse: return "false";
        case TokenKind::LParen: return "(";
        case TokenKind::RParen: return ")";
        case TokenKind::LBrace: return "{";
        case TokenKind::RBrace: return "}";
        case TokenKind::Colon: return ":";
        case TokenKind::Semicolon: return ";";
        case TokenKind::Comma: return ",";
        case TokenKind::Assign: return "=";
        case TokenKind::Plus: return "+";
        case TokenKind::Minus: return "-";
        case TokenKind::Star: return "*";
        case TokenKind::Slash: return "/";
        case TokenKind::Less: return "<";
        case TokenKind::Greater: return ">";
        case TokenKind::EqualEqual: return "==";
        case TokenKind::AndAnd: return "&&";
        case TokenKind::OrOr: return "||";
    }
    return "?";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek(size_t offset) const {
    if (pos_ + offset >= source_.size()) return '\0';
    return source_[pos_ + offset];
}

bool Lexer::atEnd() const {
    return pos_ >= source_.size();
}

char Lexer::advance() {
    char c = source_[pos_++];
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return c;
}

Token Lexer::makeToken(TokenKind kind,
                       const std::string& lexeme,
                       int line,
                       int column) const {
    return Token{kind, lexeme, line, column};
}

void Lexer::skipWhitespaceAndComments() {
    while (!atEnd()) {
        if (std::isspace(static_cast<unsigned char>(peek()))) {
            advance();
            continue;
        }

        if (peek() == '/' && peek(1) == '/') {
            while (!atEnd() && peek() != '\n')
                advance();
            continue;
        }

        break;
    }
}

Token Lexer::lexIdentifierOrKeyword() {
    // TODO(A1): Read [A-Za-z_][A-Za-z0-9_]*.
    //
    // Then recognize these keywords:
    // fn let int bool if else return true false
    //
    // Otherwise return Identifier.

    int startLine = line_;
    int startColumn = column_;

    // Starter placeholder: consume one character so the compiler
    // can build before students implement this routine.
    std::string text(1, advance());
    return makeToken(TokenKind::Identifier, text,
                     startLine, startColumn);
}

Token Lexer::lexInteger() {
    // TODO(A1): Consume a complete sequence of decimal digits.

    int startLine = line_;
    int startColumn = column_;

    std::string text(1, advance());
    return makeToken(TokenKind::Integer, text,
                     startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!atEnd()) {
        skipWhitespaceAndComments();
        if (atEnd()) break;

        int startLine = line_;
        int startColumn = column_;
        char c = peek();

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(lexInteger());
            continue;
        }

        // TODO(A1): Recognize punctuation and operators:
        // ( ) { } : ; ,
        // = + - * / < >
        // == && ||
        //
        // Remember that ==, && and || are two-character tokens.

        throw std::runtime_error(
            "Lexical error at line " + std::to_string(startLine) +
            ", column " + std::to_string(startColumn) +
            ": unexpected character '" + std::string(1, c) + "'"
        );
    }

    tokens.push_back(
        makeToken(TokenKind::End, "", line_, column_)
    );
    return tokens;
}
