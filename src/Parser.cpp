#include "Parser.h"
#include <sstream>

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {}

const Token& Parser::peek(size_t offset) const {
    /*
        look at the current token pointed to by current_

        Args:
            offset (size_t): the number of tokens to look ahead. If 0,
            returns the current token.
        
        Returns:
            Token: the token pointed to
    */
    size_t index = current_ + offset;
    if (index >= tokens_.size()) return tokens_.back();
    return tokens_[index];
}

const Token& Parser::previous() const {
    /*
        similar to peek(), it gets the previous token
    */
    return tokens_[current_ - 1];
}

bool Parser::atEnd() const {
    /*
        checks if current_ is at the end of the tokens list
    */
    return peek().kind == TokenKind::End;
}

bool Parser::check(TokenKind kind) const {
    /*
        checks if the current token equals a specific kind
    */
    return peek().kind == kind;
}

bool Parser::match(TokenKind kind) {
    /*
        checks if a token matches a specific kind.
        If the kinds do match, the current_ pointer is advanced
    */
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
    /*
        parses the main function for a program.
        Since tinylang has only 1 function and no parameters,
        all programs must exactly match so we can just
        hard code and check everything directly
    */
    // TODO(A1): Parse exactly:
    //
    // fn main() : int { ... }
    //
    // You may require the function name to literally be "main".
    // TinyLang has no parameters and no additional functions.


    consume(TokenKind::KwFn, "Expected function declaration keyword 'fn'");

    if (check(TokenKind::Identifier) && peek().lexeme == "main") {
        consume(TokenKind::Identifier, "Expected function name 'main'");
    }
    else {
        error(peek(), "Expected Identifier token 'main'");
    }

    consume(TokenKind::LParen, "Expected '(' for main function parameter list");
    consume(TokenKind::RParen, "Expected ')' for main function parameter list");
    consume(TokenKind::Colon, "Expected ':' after main function parameter list");
    consume(TokenKind::KwInt, "Expected return type 'int' after main function declaration");

    /*
        struct Function {
            std::string name;
            Type returnType;
            std::unique_ptr<BlockStmt> body;
        };
    */

    return Function{"main", Type::Int, parseBlock()};
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    /*
        Parses a block of statements enclosed in braces.
        
        Returns:
            unique_ptr: A pointer to the parsed block of statements.
    */
    // TODO(A1):
    // block := "{" statement* "}"
    
    consume(TokenKind::LBrace, "Expected '{' for block of statements");
    
    // continue to parse until the closing brace
    // and collect all StmtPtrs into a block
    std::vector<StmtPtr> statements;
    while (!check(TokenKind::RBrace) && !check(TokenKind::End)) {
        statements.push_back(parseStatement());
    }

    // when the loop ends, the type of the current
    // token is an } so we can consume without checking it again
    consume(TokenKind::RBrace, "Expected '}' to close block of statements");

    // build a unique pointer to the block of statements and return it
    std::unique_ptr<BlockStmt> block = std::make_unique<BlockStmt>();
    block->statements = std::move(statements);
    return block;
}

StmtPtr Parser::parseStatement() {
    /*
        this is the dispatcher for specific statement
        types, therefore, it doesn't consume any tokens.
    */
    // TODO(A1): Select among:
    //
    // let declaration
    // if statement
    // return statement
    // assignment (starts with Identifier)

    if (check(TokenKind::KwLet)) {
        return parseVarDecl();
    }
    else if (check(TokenKind::KwIf)) {
        return parseIf();
    }
    else if (check(TokenKind::KwReturn)) {
        return parseReturn();
    }
    else if (check(TokenKind::Identifier)) {
        return parseAssignment();
    }
    else {
        error(peek(), "Unexpected token kind in parseStatement. Token kind expected to be KwLet, KwIf, KwReturn, or Identifier");
    }
}

StmtPtr Parser::parseVarDecl() {
    /*
        Parses a variable declaration statement

        The variable declaration can either be an initialization with no value
        or an initialization with a value.

        Returns:
            StmtPtr: A unique pointer to a Stmt object
    */
    // TODO(A1):
    //
    // var_decl :=
    //   "let" Identifier ":" type
    //   ("=" expression)? ";"

    std::string name;
    Type type;
    ExprPtr expr;

    consume(TokenKind::KwLet, "Expected 'let' for variable declaration");
    consume(TokenKind::Identifier, "Expected identifier for variable declaration");
    name = previous().lexeme;

    consume(TokenKind::Colon, "Expected ':' after variable name in variable declaration");
    type = parseType();

    // optionally parse the initializer expression if '=' is present
    if (check(TokenKind::Assign)) {
        consume(TokenKind::Assign, "Expected '=' for variable initialization in variable declaration");
        expr = parseExpression();
    }

    consume(TokenKind::Semicolon, "Expected ';' at the end of variable declaration");

    // make a unique pointer using the variable name, type, and initializer expression
    return std::make_unique<VarDeclStmt>(std::move(name), type, std::move(expr));
}

StmtPtr Parser::parseAssignment() {
    /*
        this function parses an assignment statement for
        an already declared variable.

        Returns:
            StmtPtr: A unique pointer to a Stmt object
    */
    // TODO(A1):
    //
    // assignment :=
    //   Identifier "=" expression ";"

    std::string name;
    ExprPtr expr;

    consume(TokenKind::Identifier, "Expected identifier for assignment");
    name = previous().lexeme;

    consume(TokenKind::Assign, "Expected '=' for assignment");
    expr = parseExpression();
    consume(TokenKind::Semicolon, "Expected ';' at the end of assignment");

    return std::make_unique<AssignStmt>(std::move(name), std::move(expr));
}

StmtPtr Parser::parseIf() {
    /*
        Parses an if statement.

        Returns:
            StmtPtr: A unique pointer to a Stmt object
    */
    // TODO(A1):
    //
    // if_stmt :=
    //   "if" "(" expression ")" block
    //   ("else" block)?

    ExprPtr condition;
    std::unique_ptr<BlockStmt> thenBlock;
    std::unique_ptr<BlockStmt> elseBlock;

    // parse the if block
    consume(TokenKind::KwIf, "Expected 'if' for if statement");
    consume(TokenKind::LParen, "Expected '(' for if condition");
    condition = parseExpression();
    consume(TokenKind::RParen, "Expected ')' after if condition");
    thenBlock = parseBlock();

    // there can optionally be an else block
    if (check(TokenKind::KwElse)) {
        consume(TokenKind::KwElse, "Expected 'else' for if statement");
        elseBlock = parseBlock();
    }

    // build and return an IfStmt pointer
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

StmtPtr Parser::parseReturn() {
    /*
        Parses a return statement.

        Returns:
            StmtPtr: A unique pointer to a Stmt object
    */
    // TODO(A1):
    //
    // return_stmt :=
    //   "return" expression ";"

    ExprPtr expr;

    consume(TokenKind::KwReturn, "Expected 'return' for return statement");
    expr = parseExpression();
    consume(TokenKind::Semicolon, "Expected ';' at the end of return statement");

    return std::make_unique<ReturnStmt>(std::move(expr));
}

Type Parser::parseType() {
    // TODO(A1): Recognize int and bool.
    
    if(check(TokenKind::KwInt)) {
        consume(TokenKind::KwInt, "Expected 'int' for type");
        return Type::Int;
    }
    else if(check(TokenKind::KwBool)) {
        consume(TokenKind::KwBool, "Expected 'bool' for type");
        return Type::Bool;
    }
    else {
        error(peek(), "Expected 'int' or 'bool' for type");
        return Type::Error;
    }
}


ExprPtr Parser::parseExpression() {
    return parseOr();
}

ExprPtr Parser::parseOr() {
    /*
        Parses an OR expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the OR expression.
    */
    // TODO(A1):
    // or := and ("||" and)*

    ExprPtr left = parseAnd();

    // we need to loop through 0 or more OR operators
    // and build the binary expression object
    while (check(TokenKind::OrOr)) {
        consume(TokenKind::OrOr, "Expected '||' for OR expression");
        ExprPtr right = parseAnd();
        left = std::make_unique<BinaryExpr>("||", std::move(left), std::move(right));
    }

    return left;
}

ExprPtr Parser::parseAnd() {
    /*
        Parses an AND expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the AND expression.
    */
    // TODO(A1):
    // and := equality ("&&" equality)*
    
    ExprPtr left = parseEquality();

    // we need to loop through 0 or more AND operators
    // and build the binary expression object
    while (check(TokenKind::AndAnd)) {
        consume(TokenKind::AndAnd, "Expected '&&' for AND expression");
        ExprPtr right = parseEquality();
        left = std::make_unique<BinaryExpr>("&&", std::move(left), std::move(right));
    }

    return left;
}

ExprPtr Parser::parseEquality() {
    /*
        Parses an equality expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the equality expression.
    */
    // TODO(A1):
    // equality := comparison ("==" comparison)*

    ExprPtr left = parseComparison();

    // we need to loop through 0 or more equality operators
    // and build the binary expression object
    while (check(TokenKind::EqualEqual)) {
        consume(TokenKind::EqualEqual, "Expected '==' for equality expression");
        ExprPtr right = parseComparison();
        left = std::make_unique<BinaryExpr>("==", std::move(left), std::move(right));
    }

    return left;
}

ExprPtr Parser::parseComparison() {
    /*
        Parses a comparison expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the comparison expression.
    */
    // TODO(A1):
    // comparison := term (("<" | ">") term)*

    ExprPtr left = parseTerm();

    // we need to loop through 0 or more comparison operators
    // and build the binary expression object
    while (check(TokenKind::Less) || check(TokenKind::Greater)) {
        // check <
        if (check(TokenKind::Less)) {
            consume(TokenKind::Less, "Expected '<' for comparison expression");
            ExprPtr right = parseTerm();
            left = std::make_unique<BinaryExpr>("<", std::move(left), std::move(right));
        }
        // check > 
        else {
            consume(TokenKind::Greater, "Expected '>' for comparison expression");
            ExprPtr right = parseTerm();
            left = std::make_unique<BinaryExpr>(">", std::move(left), std::move(right));
        }
    }

    return left;
}

ExprPtr Parser::parseTerm() {
    /*
        Parses a addition and subtraction expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the term expression.
    */
    // TODO(A1):
    // term := factor (("+" | "-") factor)*

    ExprPtr left = parseFactor();

    // we need to loop through 0 or more addition and subtraction operators
    // and build the binary expression object
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
        // handle +
        if (check(TokenKind::Plus)) {
            consume(TokenKind::Plus, "Expected '+' for addition expression");
            ExprPtr right = parseFactor();
            left = std::make_unique<BinaryExpr>("+", std::move(left), std::move(right));
        }
        // handle -
        else {
            consume(TokenKind::Minus, "Expected '-' for subtraction expression");
            ExprPtr right = parseFactor();
            left = std::make_unique<BinaryExpr>("-", std::move(left), std::move(right));
        }
    }

    return left;
}

ExprPtr Parser::parseFactor() {
    /*
        Parses multiplication and division expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the factor expression.
    */
    // TODO(A1):
    // factor := primary (("*" | "/") primary)*

    ExprPtr left = parsePrimary();

    // we need to loop through 0 or more multiplication and division operators
    // and build the binary expression object
    while (check(TokenKind::Star) || check(TokenKind::Slash)) {
        // handle *
        if (check(TokenKind::Star)) {
            consume(TokenKind::Star, "Expected '*' for multiplication expression");
            ExprPtr right = parsePrimary();
            left = std::make_unique<BinaryExpr>("*", std::move(left), std::move(right));
        }
        // handle /
        else {
            consume(TokenKind::Slash, "Expected '/' for division expression");
            ExprPtr right = parsePrimary();
            left = std::make_unique<BinaryExpr>("/", std::move(left), std::move(right));
        }
    }

    return left;
}

ExprPtr Parser::parsePrimary() {
    /*
        Parses a primary expression.

        Returns:
            ExprPtr: A unique pointer to an Expr object representing the primary expression.
    */

    // TODO(A1): Handle:
    //
    // Integer
    // true
    // false
    // Identifier
    // "(" expression ")"

    // handle integers
    if (check(TokenKind::Integer)) {
        const Token& token = consume(TokenKind::Integer, "Expected integer literal");
        // this converts the token lexeme string into an integer literal
        return std::make_unique<IntegerExpr>(std::stoi(token.lexeme));
    }

    // handle True
    if (check(TokenKind::KwTrue)) {
        consume(TokenKind::KwTrue, "Expected 'true' for boolean literal");
        return std::make_unique<BoolExpr>(true);
    }

    // handle False
    if (check(TokenKind::KwFalse)) {
        consume(TokenKind::KwFalse, "Expected 'false' for boolean literal");
        return std::make_unique<BoolExpr>(false);
    }

    // handle identifiers
    if (check(TokenKind::Identifier)) {
        const Token& token = consume(TokenKind::Identifier, "Expected identifier for variable expression");
        // use the lexeme (variable name) to create a variable expression
        return std::make_unique<VariableExpr>(token.lexeme);
    }

    // handle expressions in parenthesis
    if (check(TokenKind::LParen)) { 
        consume(TokenKind::LParen, "Expected '(' for parenthesized expression");
        ExprPtr expr = parseExpression();
        consume(TokenKind::RParen, "Expected ')' for parenthesized expression");
        return expr;
    }

    // if none of the above patterns matched, throw an error
    std::string msg = "Unexpected token in primary expression. Expected Integer literal, boolean literal, identifier, or expression.";
    error(peek(), msg);
    return nullptr;
}
