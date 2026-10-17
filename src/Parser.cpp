/*
    NOTE: This is the given code solution for Parser.cpp
*/
#include "Parser.h"

#include <sstream>
#include <string>

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

const Token& Parser::peek(size_t offset) const {
    size_t index = current_ + offset;
    if (index >= tokens_.size()) return tokens_.back();
    return tokens_[index];
}

const Token& Parser::previous() const { return tokens_[current_ - 1]; }
bool Parser::atEnd() const { return peek().kind == TokenKind::End; }
bool Parser::check(TokenKind kind) const { return peek().kind == kind; }

bool Parser::match(TokenKind kind) {
    if (!check(kind)) return false;
    ++current_;
    return true;
}

const Token& Parser::consume(TokenKind kind, const std::string& message) {
    if (check(kind)) return tokens_[current_++];
    error(peek(), message);
}

[[noreturn]] void Parser::error(const Token& token, const std::string& message) const {
    std::ostringstream out;
    out << "Syntax error at line " << token.line << ", column " << token.column
        << ": " << message;
    throw ParseError(out.str());
}

Program Parser::parseProgram() {
    Program p{parseMainFunction()};
    consume(TokenKind::End, "unexpected tokens after main function");
    return p;
}

Function Parser::parseMainFunction() {
    // main_function -> "fn" "main" "(" ")" ":" "int" block
    consume(TokenKind::KwFn, "expected 'fn' at beginning of program");
    const Token& name = consume(TokenKind::Identifier, "expected function name after 'fn'");
    if (name.lexeme != "main")
        error(name, "TinyLang programs must define exactly one function named 'main'");

    consume(TokenKind::LParen, "expected '(' after 'main'");
    consume(TokenKind::RParen, "expected ')' after '('");
    consume(TokenKind::Colon, "expected ':' before return type");
    consume(TokenKind::KwInt, "TinyLang main() must return int");

    Function f;
    f.name = "main";
    f.returnType = Type::Int;
    f.body = parseBlock();
    return f;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    // block -> "{" statement* "}"
    consume(TokenKind::LBrace, "expected '{' to begin block");
    auto block = std::make_unique<BlockStmt>();

    while (!check(TokenKind::RBrace) && !atEnd())
        block->statements.push_back(parseStatement());

    consume(TokenKind::RBrace, "expected '}' to close block");
    return block;
}

StmtPtr Parser::parseStatement() {
    // FIRST sets make this dispatch straightforward.
    if (check(TokenKind::KwLet)) return parseVarDecl();
    if (check(TokenKind::KwIf)) return parseIf();
    if (check(TokenKind::KwReturn)) return parseReturn();
    if (check(TokenKind::Identifier)) return parseAssignment();
    error(peek(), "expected statement");
}

StmtPtr Parser::parseVarDecl() {
    // var_decl -> "let" Identifier ":" type ("=" expression)? ";"
    consume(TokenKind::KwLet, "expected 'let'");
    const Token& name = consume(TokenKind::Identifier, "expected variable name after 'let'");
    consume(TokenKind::Colon, "expected ':' after variable name");
    Type type = parseType();

    ExprPtr initializer = nullptr;
    if (match(TokenKind::Assign)) initializer = parseExpression();

    consume(TokenKind::Semicolon, "expected ';' after variable declaration");
    return std::make_unique<VarDeclStmt>(name.lexeme, type, std::move(initializer));
}

StmtPtr Parser::parseAssignment() {
    // assignment -> Identifier "=" expression ";"
    const Token& name = consume(TokenKind::Identifier, "expected variable name");
    consume(TokenKind::Assign, "expected '=' after variable name");
    ExprPtr value = parseExpression();
    consume(TokenKind::Semicolon, "expected ';' after assignment");
    return std::make_unique<AssignStmt>(name.lexeme, std::move(value));
}

StmtPtr Parser::parseIf() {
    // if_stmt -> "if" "(" expression ")" block ("else" block)?
    consume(TokenKind::KwIf, "expected 'if'");
    consume(TokenKind::LParen, "expected '(' after 'if'");
    ExprPtr condition = parseExpression();
    consume(TokenKind::RParen, "expected ')' after if condition");

    auto thenBlock = parseBlock();
    std::unique_ptr<BlockStmt> elseBlock = nullptr;
    if (match(TokenKind::KwElse)) elseBlock = parseBlock();

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

StmtPtr Parser::parseReturn() {
    // return_stmt -> "return" expression ";"
    consume(TokenKind::KwReturn, "expected 'return'");
    ExprPtr value = parseExpression();
    consume(TokenKind::Semicolon, "expected ';' after return expression");
    return std::make_unique<ReturnStmt>(std::move(value));
}

Type Parser::parseType() {
    if (match(TokenKind::KwInt)) return Type::Int;
    if (match(TokenKind::KwBool)) return Type::Bool;
    error(peek(), "expected type 'int' or 'bool'");
}

ExprPtr Parser::parseExpression() { return parseOr(); }

ExprPtr Parser::parseOr() {
    ExprPtr expr = parseAnd();
    while (match(TokenKind::OrOr)) {
        ExprPtr rhs = parseAnd();
        expr = std::make_unique<BinaryExpr>("||", std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseAnd() {
    ExprPtr expr = parseEquality();
    while (match(TokenKind::AndAnd)) {
        ExprPtr rhs = parseEquality();
        expr = std::make_unique<BinaryExpr>("&&", std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseEquality() {
    ExprPtr expr = parseComparison();
    while (match(TokenKind::EqualEqual)) {
        ExprPtr rhs = parseComparison();
        expr = std::make_unique<BinaryExpr>("==", std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseComparison() {
    ExprPtr expr = parseTerm();
    while (check(TokenKind::Less) || check(TokenKind::Greater)) {
        TokenKind op = peek().kind;
        ++current_;
        ExprPtr rhs = parseTerm();
        expr = std::make_unique<BinaryExpr>(op == TokenKind::Less ? "<" : ">",
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseTerm() {
    ExprPtr expr = parseFactor();
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
        TokenKind op = peek().kind;
        ++current_;
        ExprPtr rhs = parseFactor();
        expr = std::make_unique<BinaryExpr>(op == TokenKind::Plus ? "+" : "-",
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseFactor() {
    ExprPtr expr = parsePrimary();
    while (check(TokenKind::Star) || check(TokenKind::Slash)) {
        TokenKind op = peek().kind;
        ++current_;
        ExprPtr rhs = parsePrimary();
        expr = std::make_unique<BinaryExpr>(op == TokenKind::Star ? "*" : "/",
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenKind::Integer))
        return std::make_unique<IntegerExpr>(std::stoi(previous().lexeme));
    if (match(TokenKind::KwTrue)) return std::make_unique<BoolExpr>(true);
    if (match(TokenKind::KwFalse)) return std::make_unique<BoolExpr>(false);
    if (match(TokenKind::Identifier))
        return std::make_unique<VariableExpr>(previous().lexeme);

    if (match(TokenKind::LParen)) {
        ExprPtr expr = parseExpression();
        consume(TokenKind::RParen, "expected ')' after expression");
        return expr;
    }

    error(peek(), "expected expression");
}
