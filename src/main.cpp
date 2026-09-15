#include "AST.h"
#include "CodeGen.h"
#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static std::string readFile(const std::string& path) {
    std::ifstream input(path);
    if (!input)
        throw std::runtime_error("cannot open file: " + path);

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

static void usage(const char* argv0) {
    std::cerr
        << "Usage:\n"
        << "  " << argv0 << " file.tl --tokens\n"
        << "  " << argv0 << " file.tl --ast\n"
        << "  " << argv0 << " file.tl --check\n"
        << "  " << argv0 << " file.tl --emit-llvm\n";
}

int main(int argc, char** argv) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    try {
        std::string source = readFile(argv[1]);
        std::string mode = argv[2];

        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        if (mode == "--tokens") {
            for (const auto& token : tokens) {
                std::cout
                    << token.line << ":"
                    << token.column << "  "
                    << tokenKindName(token.kind)
                    << "  "
                    << token.lexeme << "\n";
            }
            return 0;
        }

        Parser parser(std::move(tokens));
        Program program = parser.parseProgram();

        if (mode == "--ast") {
            printAST(program);
            return 0;
        }

        SemanticAnalyzer semantic;
        bool valid = semantic.analyze(program);

        if (!valid) {
            for (const auto& error : semantic.errors())
                std::cerr << "Semantic error: "
                          << error << "\n";
            return 2;
        }

        if (mode == "--check") {
            std::cout << "Semantic analysis successful.\n";
            return 0;
        }

        if (mode == "--emit-llvm") {
            CodeGenerator codegen;
            if (!codegen.generate(program))
                return 3;

            codegen.printIR();
            return 0;
        }

        usage(argv[0]);
        return 1;

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
