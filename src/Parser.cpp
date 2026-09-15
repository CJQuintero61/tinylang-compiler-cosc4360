#include "Parser.h"
#include <sstream>

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {}

const Token& Parser::peek(size_t offset) const {
    size_t index = current_ + offset;
    if (index >= tokens_.size()) return tokens_.back();
    return tokens_[index];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

bool Parser::atEnd() const {
    return peek().kind == TokenKind::End;
}

bool Parser::check(TokenKind kind) const {
    return peek().kind == kind;
}

bool Parser::match(TokenKind kind) {
    if (!check(kind)) return false;
    ++current_;
    return true;
}

const Token& Parser::consume(TokenKind kind,
                             const std::string& message) {
    if (check(kind)) return tokens_[current_++];
    error(peek(), message);
}

[[noreturn]] void Parser::error(const Token& token,
                                const std::string& message) const {
    std::ostringstream out;
    out << "Syntax error at line "
        << token.line << ", column " << token.column
        << ": " << message;
    throw ParseError(out.str());
}

Program Parser::parseProgram() {
    // TinyLang deliberately contains exactly one main function.
    Program p{parseMainFunction()};

    consume(TokenKind::End,
            "unexpected tokens after main function");
    return p;
}

Function Parser::parseMainFunction() {
    // TODO(A1): Parse exactly:
    //
    // fn main() : int { ... }
    //
    // You may require the function name to literally be "main".
    // TinyLang has no parameters and no additional functions.

    error(peek(), "TODO(A1): parseMainFunction not implemented");
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    // TODO(A1):
    // block := "{" statement* "}"
    error(peek(), "TODO(A1): parseBlock not implemented");
}

StmtPtr Parser::parseStatement() {
    // TODO(A1): Select among:
    //
    // let declaration
    // if statement
    // return statement
    // assignment (starts with Identifier)

    error(peek(), "TODO(A1): parseStatement not implemented");
}

StmtPtr Parser::parseVarDecl() {
    // TODO(A1):
    //
    // var_decl :=
    //   "let" Identifier ":" type
    //   ("=" expression)? ";"

    error(peek(), "TODO(A1): parseVarDecl not implemented");
}

StmtPtr Parser::parseAssignment() {
    // TODO(A1):
    //
    // assignment :=
    //   Identifier "=" expression ";"

    error(peek(), "TODO(A1): parseAssignment not implemented");
}

StmtPtr Parser::parseIf() {
    // TODO(A1):
    //
    // if_stmt :=
    //   "if" "(" expression ")" block
    //   ("else" block)?

    error(peek(), "TODO(A1): parseIf not implemented");
}

StmtPtr Parser::parseReturn() {
    // TODO(A1):
    //
    // return_stmt :=
    //   "return" expression ";"

    error(peek(), "TODO(A1): parseReturn not implemented");
}

Type Parser::parseType() {
    // TODO(A1): Recognize int and bool.
    error(peek(), "TODO(A1): parseType not implemented");
}

ExprPtr Parser::parseExpression() {
    return parseOr();
}

ExprPtr Parser::parseOr() {
    // TODO(A1):
    // or := and ("||" and)*
    return parseAnd();
}

ExprPtr Parser::parseAnd() {
    // TODO(A1):
    // and := equality ("&&" equality)*
    return parseEquality();
}

ExprPtr Parser::parseEquality() {
    // TODO(A1):
    // equality := comparison ("==" comparison)*
    return parseComparison();
}

ExprPtr Parser::parseComparison() {
    // TODO(A1):
    // comparison := term (("<" | ">") term)*
    return parseTerm();
}

ExprPtr Parser::parseTerm() {
    // TODO(A1):
    // term := factor (("+" | "-") factor)*
    return parseFactor();
}

ExprPtr Parser::parseFactor() {
    // TODO(A1):
    // factor := primary (("*" | "/") primary)*
    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    // TODO(A1): Handle:
    //
    // Integer
    // true
    // false
    // Identifier
    // "(" expression ")"

    error(peek(), "TODO(A1): parsePrimary not implemented");
}
