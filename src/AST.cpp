#include "AST.h"
#include <iostream>

std::string typeName(Type type) {
    switch (type) {
        case Type::Int: return "int";
        case Type::Bool: return "bool";
        default: return "<error>";
    }
}

namespace {

void indent(int depth) {
    for (int i = 0; i < depth; ++i) std::cout << "  ";
}

void printExpr(const Expr& expr, int depth) {
    indent(depth);

    if (auto* e = dynamic_cast<const IntegerExpr*>(&expr)) {
        std::cout << "Integer(" << e->value << ")\n";
    } else if (auto* e = dynamic_cast<const BoolExpr*>(&expr)) {
        std::cout << "Bool(" << (e->value ? "true" : "false") << ")\n";
    } else if (auto* e = dynamic_cast<const VariableExpr*>(&expr)) {
        std::cout << "Variable(" << e->name << ")\n";
    } else if (auto* e = dynamic_cast<const BinaryExpr*>(&expr)) {
        std::cout << "Binary(" << e->op << ")\n";
        printExpr(*e->lhs, depth + 1);
        printExpr(*e->rhs, depth + 1);
    }
}

void printStmt(const Stmt& stmt, int depth) {
    if (auto* s = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        indent(depth);
        std::cout << "VarDecl " << s->name
                  << " : " << typeName(s->type) << "\n";
        if (s->initializer) printExpr(*s->initializer, depth + 1);
    } else if (auto* s = dynamic_cast<const AssignStmt*>(&stmt)) {
        indent(depth);
        std::cout << "Assign " << s->name << "\n";
        printExpr(*s->value, depth + 1);
    } else if (auto* s = dynamic_cast<const ReturnStmt*>(&stmt)) {
        indent(depth);
        std::cout << "Return\n";
        printExpr(*s->value, depth + 1);
    } else if (auto* s = dynamic_cast<const IfStmt*>(&stmt)) {
        indent(depth);
        std::cout << "If\n";
        indent(depth + 1);
        std::cout << "Condition\n";
        printExpr(*s->condition, depth + 2);

        indent(depth + 1);
        std::cout << "Then\n";
        for (const auto& child : s->thenBlock->statements)
            printStmt(*child, depth + 2);

        if (s->elseBlock) {
            indent(depth + 1);
            std::cout << "Else\n";
            for (const auto& child : s->elseBlock->statements)
                printStmt(*child, depth + 2);
        }
    }
}

}

void printAST(const Program& program) {
    std::cout << "Function "
              << program.mainFunction.name
              << " -> "
              << typeName(program.mainFunction.returnType)
              << "\n";

    for (const auto& stmt : program.mainFunction.body->statements)
        printStmt(*stmt, 1);
}
