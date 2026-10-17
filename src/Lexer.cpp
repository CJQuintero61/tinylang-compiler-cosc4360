/*
    NOTE: This is the given code solution for Lexer.cpp
*/
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

// whether we reach the end of file/input
bool Lexer::atEnd() const { return pos_ >= source_.size(); }

// consume current char, move pos_ forward
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

Token Lexer::makeToken(TokenKind kind, const std::string& lexeme,
                       int line, int column) const {
    return Token{kind, lexeme, line, column};
}

void Lexer::skipWhitespaceAndComments() {
    while (!atEnd()) {
        if (std::isspace(static_cast<unsigned char>(peek()))) {
            advance();
            continue;
        }

        // TinyLang supports // comments.  The newline itself will be
        // consumed on the next loop iteration as whitespace.
        if (peek() == '/' && peek(1) == '/') {
            while (!atEnd() && peek() != '\n') advance();
            continue;
        }
        break;
    }
}

Token Lexer::lexIdentifierOrKeyword() {
    const int startLine = line_;
    const int startColumn = column_;
    const size_t startPos = pos_;

    // Identifier grammar: [A-Za-z_][A-Za-z0-9_]*
    advance();
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
        advance();
    // std::isalnum https://icpc.student.cs.uwaterloo.ca/~acm00/cppreference/doc/html/en/cpp/string/byte/isalnum.html 
    //  Checks if the given character is an alphanumeric character as classified by the current C locale. In the default locale, the following characters are alphanumeric:
    // - digits (0123456789)
    // - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
    // - lowercase letters (abcdefghijklmnopqrstuvwxyz)

    const std::string text = source_.substr(startPos, pos_ - startPos);

    // Keywords have dedicated token kinds.  Everything else that follows
    // the identifier rule is a normal Identifier token.
    static const std::unordered_map<std::string, TokenKind> keywords = {
        {"fn", TokenKind::KwFn},       {"let", TokenKind::KwLet},
        {"int", TokenKind::KwInt},     {"bool", TokenKind::KwBool},
        {"if", TokenKind::KwIf},       {"else", TokenKind::KwElse},
        {"return", TokenKind::KwReturn},{"true", TokenKind::KwTrue},
        {"false", TokenKind::KwFalse},
    };

    auto it = keywords.find(text);
    if (it != keywords.end())
        return makeToken(it->second, text, startLine, startColumn);

    return makeToken(TokenKind::Identifier, text, startLine, startColumn);
}

Token Lexer::lexInteger() {
    const int startLine = line_;
    const int startColumn = column_;
    const size_t startPos = pos_;

    // Consume the whole decimal literal, e.g. "123" as one token.
    while (std::isdigit(static_cast<unsigned char>(peek()))) advance();

    return makeToken(TokenKind::Integer,
                     source_.substr(startPos, pos_ - startPos),
                     startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!atEnd()) {
        skipWhitespaceAndComments();
        if (atEnd()) break;

        const int startLine = line_;
        const int startColumn = column_;
        const char c = peek();

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(lexInteger());
            continue;
        }

        // Punctuation and operators.  Multi-character operators are checked
        // before falling back to their one-character form.
        switch (c) {
            case '(':
                advance(); tokens.push_back(makeToken(TokenKind::LParen, "(", startLine, startColumn)); break;
            case ')':
                advance(); tokens.push_back(makeToken(TokenKind::RParen, ")", startLine, startColumn)); break;
            case '{':
                advance(); tokens.push_back(makeToken(TokenKind::LBrace, "{", startLine, startColumn)); break;
            case '}':
                advance(); tokens.push_back(makeToken(TokenKind::RBrace, "}", startLine, startColumn)); break;
            case ':':
                advance(); tokens.push_back(makeToken(TokenKind::Colon, ":", startLine, startColumn)); break;
            case ';':
                advance(); tokens.push_back(makeToken(TokenKind::Semicolon, ";", startLine, startColumn)); break;
            case ',':
                advance(); tokens.push_back(makeToken(TokenKind::Comma, ",", startLine, startColumn)); break;
            case '+':
                advance(); tokens.push_back(makeToken(TokenKind::Plus, "+", startLine, startColumn)); break;
            case '-':
                advance(); tokens.push_back(makeToken(TokenKind::Minus, "-", startLine, startColumn)); break;
            case '*':
                advance(); tokens.push_back(makeToken(TokenKind::Star, "*", startLine, startColumn)); break;
            case '/':
                // // comments have already been handled above.
                advance(); tokens.push_back(makeToken(TokenKind::Slash, "/", startLine, startColumn)); break;
            case '<':
                advance(); tokens.push_back(makeToken(TokenKind::Less, "<", startLine, startColumn)); break;
            case '>':
                advance(); tokens.push_back(makeToken(TokenKind::Greater, ">", startLine, startColumn)); break;
            case '=':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenKind::EqualEqual, "==", startLine, startColumn));
                } else {
                    tokens.push_back(makeToken(TokenKind::Assign, "=", startLine, startColumn));
                }
                break;
            case '&':
                advance();
                if (peek() != '&')
                    throw std::runtime_error("Lexical error at line " + std::to_string(startLine) +
                                             ", column " + std::to_string(startColumn) +
                                             ": expected '&' after '&'");
                advance();
                tokens.push_back(makeToken(TokenKind::AndAnd, "&&", startLine, startColumn));
                break;
            case '|':
                advance();
                if (peek() != '|')
                    throw std::runtime_error("Lexical error at line " + std::to_string(startLine) +
                                             ", column " + std::to_string(startColumn) +
                                             ": expected '|' after '|'");
                advance();
                tokens.push_back(makeToken(TokenKind::OrOr, "||", startLine, startColumn));
                break;
            default:
                throw std::runtime_error("Lexical error at line " + std::to_string(startLine) +
                                         ", column " + std::to_string(startColumn) +
                                         ": unexpected character '" + std::string(1, c) + "'");
        }
    }

    // End is a sentinel consumed by parseProgram().
    tokens.push_back(makeToken(TokenKind::End, "", line_, column_));
    return tokens;
}
