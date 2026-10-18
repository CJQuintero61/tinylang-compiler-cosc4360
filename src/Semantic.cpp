#include "Semantic.h"

#include <unordered_map>
#include <iostream>
#include <typeinfo>

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
    /*
        this function determines the static type of an expression.

        Args:
            expr (Expr) - the expression to evaluate
        
        Returns:
            Type - the type of the expressions or an error if cannot be determined
    */

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

    // simple int expression
    if (typeid(expr) == typeid(IntegerExpr)) {
        return Type::Int;
    }
    
    // simple bool expression
    else if (typeid(expr) == typeid(BoolExpr)) {
        return Type::Bool;
    }

    // find the type of the variable
    else if (typeid(expr) == typeid(VariableExpr)) {

        // since the C++ compiler can't tell this is a variable expression at compile time,
        // this has to be cased explicitly to a VariableExpr type to then use
        // the name attribute to perform the lookup
        VariableExpr var = static_cast<const VariableExpr&>(expr);

        // then do the lookup since now we can access name from the struct
        Type type = lookup(var.name);

        // not found, log it
        if (type == Type::Error) {
            report("Use of undeclared variable: '" + var.name + "'");
        }

        return type;
    }

    // ensure the LHS and RHS agree in type
    else if (typeid(expr) == typeid(BinaryExpr)) {
        // same thing for binary expressions, to get the specific struct
        // we need to cast them otherwise this won't compile
        // binary expressions in AST.h have ExprPtr which are unique pointers
        // so this needs to be a reference instead of a regular copy
        // like what was done in the variable expression block
        const BinaryExpr& binaryExpr = static_cast<const BinaryExpr&>(expr);

        // recursively figure out the LHS and RHS types
        Type lhs = checkExpr(*binaryExpr.lhs);
        Type rhs = checkExpr(*binaryExpr.rhs);
        std::string op = binaryExpr.op;

        // if either side is an error type, then return error
        if (lhs == Type::Error || rhs == Type::Error) {
            return Type::Error;
        }

        // for arithmetic both must be ints
        if (op == "+" || op == "-" || op == "*" || op == "/") {
            // ensure both are Integers
            if (lhs == Type::Int && rhs == Type::Int) { return Type::Int; }
        }

        // for inequalities both must be ints
        else if (op == "<" || op == ">") {
            // ensure both are ints
            if (lhs == Type::Int && rhs == Type::Int) { return Type::Bool; }
        }

        // for equality both must be the same type
        else if (op == "==") {
            if (lhs == rhs) { return Type::Bool; }
        }

        // for logic both must be bools
        else if (op == "&&" || op == "||") {
            if (lhs == Type::Bool && rhs == Type::Bool) { return Type::Bool; }
        }

        report("Invalid operands for operator '" + op + "': " + typeName(lhs) + " and " + typeName(rhs));
        return Type::Error;
    }

    report("Semantic: checkExpr(): The expression passed is not an integer, boolean, or binary expression.");
    return Type::Error;
}
