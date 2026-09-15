#pragma once
#include <memory>
#include <string>
#include <vector>

enum class Type {
    Int,
    Bool,
    Error
};

std::string typeName(Type type);

struct Expr {
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct IntegerExpr : Expr {
    int value;
    explicit IntegerExpr(int v) : value(v) {}
};

struct BoolExpr : Expr {
    bool value;
    explicit BoolExpr(bool v) : value(v) {}
};

struct VariableExpr : Expr {
    std::string name;
    explicit VariableExpr(std::string n) : name(std::move(n)) {}
};

struct BinaryExpr : Expr {
    std::string op;
    ExprPtr lhs;
    ExprPtr rhs;

    BinaryExpr(std::string op, ExprPtr lhs, ExprPtr rhs)
        : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

struct Stmt {
    virtual ~Stmt() = default;
};

using StmtPtr = std::unique_ptr<Stmt>;

struct VarDeclStmt : Stmt {
    std::string name;
    Type type;
    ExprPtr initializer;

    VarDeclStmt(std::string name, Type type, ExprPtr initializer)
        : name(std::move(name)),
          type(type),
          initializer(std::move(initializer)) {}
};

struct AssignStmt : Stmt {
    std::string name;
    ExprPtr value;

    AssignStmt(std::string name, ExprPtr value)
        : name(std::move(name)), value(std::move(value)) {}
};

struct ReturnStmt : Stmt {
    ExprPtr value;
    explicit ReturnStmt(ExprPtr value) : value(std::move(value)) {}
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
};

struct IfStmt : Stmt {
    ExprPtr condition;
    std::unique_ptr<BlockStmt> thenBlock;
    std::unique_ptr<BlockStmt> elseBlock;

    IfStmt(ExprPtr condition,
           std::unique_ptr<BlockStmt> thenBlock,
           std::unique_ptr<BlockStmt> elseBlock)
        : condition(std::move(condition)),
          thenBlock(std::move(thenBlock)),
          elseBlock(std::move(elseBlock)) {}
};

struct Function {
    std::string name;
    Type returnType;
    std::unique_ptr<BlockStmt> body;
};

struct Program {
    Function mainFunction;
};

void printAST(const Program& program);
