#include "CodeGen.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>

#include <typeinfo>

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

    if (typeid(stmt) == typeid(VarDeclStmt)) {
        const VarDeclStmt& vds = static_cast<const VarDeclStmt&>(stmt);

        // allocate the var space
        auto* slot = builder_->CreateAlloca(
            llvm::Type::getInt32Ty(*context_),
            nullptr,
            vds.name
        );

        // save the slot to variables_
        variables_[vds.name] = slot;

        // if there is an initializer then store the value into that slot
        if (vds.initializer) {
            // get the initializer value
            llvm::Value* init = emitExpr(*vds.initializer);

            if (init == nullptr) { return false; }

            // store the init value into the slot
            builder_->CreateStore(init, slot);
        }
        return true;
    }

    else if (typeid(stmt) == typeid(AssignStmt)) {
        const AssignStmt& as = static_cast<const AssignStmt&>(stmt);

        // find the variables stack slot
        auto it = variables_.find(as.name);
        if (it == variables_.end()) {
            llvm::errs() << "Could not find '" + as.name + "' in stack\n";
            return false;
        }

        // get the value of the expression
        llvm::Value* value = emitExpr(*as.value);
        if (value == nullptr) { return false; }

        // store the result back
        builder_->CreateStore(value, it->second);
        return true;
    }

    else if (typeid(stmt) == typeid(ReturnStmt)) {
        const ReturnStmt& rs = static_cast<const ReturnStmt&>(stmt);

        // get the value being returned
        llvm::Value* value = emitExpr(*rs.value);
        if (value == nullptr) { return false; }

        // create the return statement
        builder_->CreateRet(value);
        return true;
    }

    else if (typeid(stmt) == typeid(IfStmt)) {
        const IfStmt& is = static_cast<const IfStmt&>(stmt);

        // emit the condition expression
        llvm::Value* cond = emitExpr(*is.condition);
        if (cond == nullptr) { return false; }

        // check it is a boolean value
        if (!cond->getType()->isIntegerTy(1)) {
            llvm::errs() << "Condition expression is not a boolean\n";
            return false;
        }

        // create the then block
        llvm::Function* function = builder_->GetInsertBlock()->getParent();

        // create an entrypoint for the then block
        // this is the block that will be executed if the condition is true
        auto* then = llvm::BasicBlock::Create(*context_, "then", function);

        // create an entrypoint for the else block
        // this is the block that will be executed if the condition is false
        auto* elseBlock = llvm::BasicBlock::Create(*context_, "else", function);

        // create an entrypoint for the merge block
        // this is the block that will be executed after the if statement
        auto* merge = llvm::BasicBlock::Create(*context_, "merge", function);

        // create the conditional branch
        builder_->CreateCondBr(cond, then, elseBlock);

        // emit the then block
        builder_->SetInsertPoint(then);
        if (!emitBlock(*is.thenBlock)) { return false; }

        // if there is no terminator then branch to the merge block
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(merge);
        }

        // set the insert point to the else block
        builder_->SetInsertPoint(elseBlock);
        if (is.elseBlock) {
            if (!emitBlock(*is.elseBlock)) { return false; }
        }

        // if there is no terminator then branch to the merge block
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(merge);
        }

        // set the insert point to the merge block
        builder_->SetInsertPoint(merge);

        return true;
    }

    llvm::errs() << "Unknown statement type is not a var declaration, assignment, or return statement\n";
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
    
    // integer expressions return an integer constant after resolving them
    if (typeid(expr) == typeid(IntegerExpr)) {
        const IntegerExpr& ie = static_cast<const IntegerExpr&>(expr);

        return llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context_),
            ie.value,
            true
        );
    }

    // load the variable first then perform the expression
    else if (typeid(expr) == typeid(VariableExpr)) {
        const VariableExpr& ve = static_cast<const VariableExpr&>(expr);

        // find the slot for this variable
        auto it = variables_.find(ve.name);
        if (it == variables_.end()) {
            llvm::errs() << "Unknown variable '" << ve.name << "'\n";
            return nullptr;
        }
        
        // return the loaded value
        return builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), it->second, ve.name);
    }

    else if (typeid(expr) == typeid(BinaryExpr)) {
        const BinaryExpr& be = static_cast<const BinaryExpr&>(expr);
        std::string op = be.op;

        // recurse the left and right
        llvm::Value* lhs = emitExpr(*be.lhs);
        llvm::Value* rhs = emitExpr(*be.rhs);

        if (lhs == nullptr || rhs == nullptr) {
            return nullptr;
        }

        // perform the specific action based on operator
        if (op == "+") { return builder_->CreateAdd(lhs, rhs); }
        else if (op == "-")  { return builder_->CreateSub(lhs, rhs); }
        else if (op == "*")  { return builder_->CreateMul(lhs, rhs); }
        else if (op == "/")  { return builder_->CreateSDiv(lhs, rhs); }
        else if (op == "<")  { return builder_->CreateICmpSLT(lhs, rhs); }
        else if (op == ">")  { return builder_->CreateICmpSGT(lhs, rhs); }
        else if (op == "==") { return builder_->CreateICmpEQ(lhs, rhs); }
        else {
            llvm::errs() << "Unknown operator '" + op + "'\n";
            return nullptr;
        }
    }

    llvm::errs() << "Unknown expression is not an integer, variable, or binary expression\n";
    return nullptr;
}

void CodeGenerator::printIR() const {
    module_->print(llvm::outs(), nullptr);
}
