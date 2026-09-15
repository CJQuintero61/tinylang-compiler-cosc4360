#pragma once
#include "AST.h"
#include <string>
#include <unordered_map>
#include <vector>

class SemanticAnalyzer {
public:
    bool analyze(const Program& program);
    const std::vector<std::string>& errors() const { return errors_; }

private:
    std::vector<std::unordered_map<std::string, Type>> scopes_;
    std::vector<std::string> errors_;
    Type currentReturnType_ = Type::Error;

    void enterScope();
    void leaveScope();

    bool declare(const std::string& name, Type type);
    Type lookup(const std::string& name) const;

    void checkBlock(const BlockStmt& block);
    void checkStmt(const Stmt& stmt);
    Type checkExpr(const Expr& expr);

    void report(const std::string& message);
};
