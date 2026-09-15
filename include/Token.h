#pragma once
#include <string>

enum class TokenKind {
    End,
    Identifier,
    Integer,

    KwFn, // keyword fn
    KwLet, // keyword let
    KwInt,  // keyword int
    KwBool, // keyword bool
    KwIf, // keyword if
    KwElse, // keyword else
    KwReturn, // keyword return
    KwTrue, // keyword true
    KwFalse, // keyword false

    LParen, // ( left parenthesis 
    RParen, // ) right parenthesis
    LBrace, // { left brace 
    RBrace, // } right brace
    Colon,  
    Semicolon,
    Comma,

    Assign, // =
    Plus, // +
    Minus, // -
    Star, // * multiplication
    Slash, // / division

    Less, // < 
    Greater, // >
    EqualEqual, // ==
    AndAnd, // &&
    OrOr // ||
};

struct Token {
    TokenKind kind;
    std::string lexeme;
    int line = 1;
    int column = 1;
};

const char* tokenKindName(TokenKind kind);
