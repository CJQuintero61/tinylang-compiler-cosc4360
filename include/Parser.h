#pragma once
#include "AST.h"
#include "Token.h"
#include <stdexcept>
#include <vector>

class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    Program parseProgram();

private:
    std::vector<Token> tokens_;
    size_t current_ = 0;

    const Token& peek(size_t offset = 0) const;
    const Token& previous() const;
    bool atEnd() const;

    bool check(TokenKind kind) const;
    bool match(TokenKind kind);
    const Token& consume(TokenKind kind, const std::string& message);

    Function parseMainFunction();
    std::unique_ptr<BlockStmt> parseBlock();
    StmtPtr parseStatement();
    StmtPtr parseVarDecl();
    StmtPtr parseAssignment();
    StmtPtr parseIf();
    StmtPtr parseReturn();

    Type parseType();

    ExprPtr parseExpression();
    ExprPtr parseOr();
    ExprPtr parseAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parsePrimary();

    [[noreturn]] void error(const Token& token,
                            const std::string& message) const;
};
