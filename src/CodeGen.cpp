#include "CodeGen.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>(
          "tinylang", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

CodeGenerator::~CodeGenerator() = default;

bool CodeGenerator::generate(const Program& program) {
    variables_.clear();

    // TinyLang Assignment 2 requires main(): int only.
    auto* functionType =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(*context_),
            false);

    auto* function =
        llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            "main",
            module_.get());

    auto* entry =
        llvm::BasicBlock::Create(
            *context_, "entry", function);

    builder_->SetInsertPoint(entry);

    return emitBlock(*program.mainFunction.body);
}

bool CodeGenerator::emitBlock(const BlockStmt& block) {
    for (const auto& stmt : block.statements) {
        if (!emitStmt(*stmt))
            return false;
    }
    return true;
}

bool CodeGenerator::emitStmt(const Stmt& stmt) {
    // TODO(A2):
    //
    // Required:
    //   VarDeclStmt for int
    //   AssignStmt for int
    //   ReturnStmt for int
    //
    // Suggested VarDecl implementation:
    //
    //   auto* slot = builder_->CreateAlloca(
    //       llvm::Type::getInt32Ty(*context_),
    //       nullptr,
    //       variableName);
    //
    // Save slot in variables_.
    //
    // IfStmt and bool LLVM generation may be extra credit.

    llvm::errs() << "TODO(A2): emitStmt not implemented\n";
    return false;
}

llvm::Value* CodeGenerator::emitExpr(const Expr& expr) {
    // TODO(A2):
    //
    // Required:
    //   IntegerExpr
    //   VariableExpr
    //   BinaryExpr: + - * / < > ==
    //
    // Integer constant:
    //
    //   llvm::ConstantInt::get(
    //       llvm::Type::getInt32Ty(*context_),
    //       value,
    //       true);
    //
    // Variable load:
    //
    //   builder_->CreateLoad(
    //       llvm::Type::getInt32Ty(*context_),
    //       slot,
    //       name);
    //
    // Useful IRBuilder operations:
    //
    //   CreateAdd
    //   CreateSub
    //   CreateMul
    //   CreateSDiv
    //   CreateICmpSLT
    //   CreateICmpSGT
    //   CreateICmpEQ

    llvm::errs() << "TODO(A2): emitExpr not implemented\n";
    return nullptr;
}

void CodeGenerator::printIR() const {
    module_->print(llvm::outs(), nullptr);
}
