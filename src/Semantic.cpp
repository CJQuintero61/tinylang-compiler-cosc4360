#include "Semantic.h"

#include <unordered_map>
#include <iostream>

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
    /*
        this function checks that a variable is not decalared twice in the
        current scope in std::vector<std::unordered_map<std::string, Type>> scopes_;

        where the vector is a list of scopes that are implemented as dictionaries/maps
        and map a variable name to a type.

        conceptually, to figure out if a variable is already declared,
        we need to look it up in the current scope. If not found, declare the var,
        else return false due to duplicate declaration.

        Args:
            name (str) - the variable name to search for or declare
            type (Type) - the variable type to declare
        
        Returns:
            bool - true if the variable can be declared, false if the var is already declared
    */

    // TODO(A2):
    // Reject duplicate declarations in the current scope only.
    //
    // Return true on success, false on duplicate.

    // iterator to search the scopes map
    std::unordered_map<std::string, Type>::iterator it;

    // since the scopes is a stack, the current scope is at the top (back) of the stack
    it = scopes_.back().find(name);
    if (it != scopes_.back().end()) {
        // if the var is already declared, return false since it's a duplicate declaration
        return false;
    }

    // add the var to the current scope
    scopes_.back().insert({name, type});
    return true;
}

Type SemanticAnalyzer::lookup(const std::string& name) const {
    /*
        search scopes from current (innermost) to global (outermost)
        for a variable declaration. If found return the
        type of the var, else return an error since
        a variable that wasn't declared is trying to
        be used for something

        Args:
            name (str) - the name of the var to lookup

        Return:
            Type - the var type if found, else returns an error
    */

    // TODO(A2):
    // Search scopes from innermost to outermost.
    // Return Type::Error if not found.

    int i = scopes_.size() - 1; // start from the innermost scope
    std::unordered_map<std::string, Type>::iterator it;
    std::unordered_map<std::string, Type> currentScope;

    /*
        from the inner most scope (size - 1), search for the variable.
        If not found, move to the parent scope by decrementing i.
        Keep moving to the next outer scope until the global scope at
        [0] and stop at -1. If still not found by now, then
        the var hasn't been declared in any scope
    */
    while (i > -1) {
        // get the current scope
        currentScope = scopes_[i];

        it = currentScope.find(name);
        if (it != currentScope.end()) {
            // found in the current scope

            // return the variables type
            return it->second;
        }

        // move to the parent scope
        i--;
    }

    // not found in any scope
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
