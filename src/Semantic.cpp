#include "Semantic.h"

void SemanticAnalyzer::enterScope() {
    scopes_.push_back({});
}

void SemanticAnalyzer::leaveScope() {
    scopes_.pop_back();
}

void SemanticAnalyzer::report(const std::string& message) {
    errors_.push_back(message);
}

bool SemanticAnalyzer::declare(const std::string& name, Type type) {
    // TODO(A2):
    // Reject duplicate declarations in the current scope only.
    //
    // Return true on success, false on duplicate.
    return false;
}

Type SemanticAnalyzer::lookup(const std::string& name) const {
    // TODO(A2):
    // Search scopes from innermost to outermost.
    // Return Type::Error if not found.
    return Type::Error;
}

bool SemanticAnalyzer::analyze(const Program& program) {
    errors_.clear();
    scopes_.clear();

    currentReturnType_ = program.mainFunction.returnType;

    enterScope();
    checkBlock(*program.mainFunction.body);
    leaveScope();

    return errors_.empty();
}

void SemanticAnalyzer::checkBlock(const BlockStmt& block) {
    // Each block receives its own lexical scope.
    enterScope();

    for (const auto& stmt : block.statements)
        checkStmt(*stmt);

    leaveScope();
}

void SemanticAnalyzer::checkStmt(const Stmt& stmt) {
    // TODO(A2):
    //
    // VarDecl:
    //   - reject duplicate declaration
    //   - if initializer exists, require same type
    //
    // Assign:
    //   - variable must exist
    //   - assigned expression must have same type
    //
    // Return:
    //   - expression type must match currentReturnType_
    //
    // If:
    //   - condition must be bool
    //   - analyze then/else blocks

    report("TODO(A2): checkStmt not implemented");
}

Type SemanticAnalyzer::checkExpr(const Expr& expr) {
    // TODO(A2):
    //
    // Literals:
    //   IntegerExpr -> int
    //   BoolExpr    -> bool
    //
    // VariableExpr:
    //   look up variable; report undeclared name
    //
    // BinaryExpr:
    //
    //   + - * /
    //      int x int -> int
    //
    //   < >
    //      int x int -> bool
    //
    //   ==
    //      operands must have same type -> bool
    //
    //   && ||
    //      bool x bool -> bool

    report("TODO(A2): checkExpr not implemented");
    return Type::Error;
}
