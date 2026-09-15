#pragma once
#include "AST.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
class LLVMContext;
class Module;
class IRBuilderBase;
class Value;
class AllocaInst;
class Function;
}

class CodeGenerator {
public:
    CodeGenerator();
    ~CodeGenerator();

    bool generate(const Program& program);
    void printIR() const;

private:
    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

    std::unordered_map<std::string, llvm::AllocaInst*> variables_;

    llvm::Value* emitExpr(const Expr& expr);
    bool emitStmt(const Stmt& stmt);
    bool emitBlock(const BlockStmt& block);
};
