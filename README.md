# TinyLang Compiler Project: Starter Package

TinyLang is a deliberately small programming language used in **TAMUCC COSC 4360** to study the major stages of a compiler without requiring students to build a production-sized compiler.

Across two coding assignments, you will gradually build a compiler front end and a small LLVM-based back end:

```text
TinyLang source code
        |
        v
+------------------+
|      Lexer       |   Coding Assignment 1
+------------------+
        |
        | tokens
        v
+------------------+
|      Parser      |   Coding Assignment 1
+------------------+
        |
        | AST
        v
+------------------+
| Semantic Analyzer|   Coding Assignment 2
+------------------+
        |
        | semantically valid AST
        v
+------------------+
| LLVM IR CodeGen  |   Coding Assignment 2
+------------------+
        |
        | LLVM IR
        v
     executable
```


# 1. Starter package structure
The starter is organized around supplied class interfaces.
Unless the assignment explicitly instructs otherwise, avoid redesigning the public interfaces in the header files just to make an implementation easier.
The goal is to fill in the missing compiler logic while preserving the pipeline and data structures used by the rest of the project.
The project uses one shared codebase for both assignments. You do **not** create a second compiler for Assignment 2. Assignment 2 continues from the AST produced in Assignment 1.

```text
tinylang-starter/
|
|-- README.md
|-- CMakeLists.txt
|
|-- include/
|   |-- Token.h
|   |-- Lexer.h
|   |-- AST.h
|   |-- Parser.h
|   |-- Semantic.h
|   `-- CodeGen.h
|
|-- src/
|   |-- main.cpp
|   |-- Lexer.cpp
|   |-- Parser.cpp
|   |-- AST.cpp
|   |-- Semantic.cpp
|   `-- CodeGen.cpp
|
`-- examples/
    |-- valid.tl
    |-- syntax_error.tl
    |-- semantic_error.tl
    `-- codegen.tl
```


---

# 2. Which files belong to each assignment?

## Coding Assignment 1

The detailed instruction is at [Coding Assignment 1](https://github.com/apace-lab/COSC4360/blob/main/coding-1/CODING1.md). 
You primarily edit `src/Lexer.cpp` and `src/Parser.cpp` and 
look for:
`TODO(A1)` to fill in your code. 
The token definitions, AST representation, AST printer, driver, and class interfaces are provided.
Assignment 1 produces:
```text
source code -> tokens -> AST
```

---

## Coding Assignment 2

The detailed instruction is at [Coding Assignment 2](https://github.com/apace-lab/COSC4360/blob/main/coding-2/CODING2.md). 
You primarily edit `src/Semantic.cpp` and `src/CodeGen.cpp` and 
look for:
`TODO(A2)` to fill in your code. 
Assignment 2 continues the pipeline from Assignment 1:
```text
AST -> semantic checking -> LLVM IR
```

---

# 3. Building the project

## Requirements

You will need:
- a C++17-compatible compiler
- CMake 
    - recommended: CMake 3.20+
    - command to install CMake in Ubuntu and MacOS: `sudo apt install cmake` or `brew install cmake`
- LLVM development libraries
    - LLVM 19 is recommended for this starter
    - command to install LLVM in Ubuntu:
    ```bash
    sudo apt update
    sudo apt install llvm clang libllvm-dev llvm-dev
    ```
    If you specifically want LLVM 19: 
    ```bash
    sudo apt update
    sudo apt install llvm-19 clang-19 llvm-19-dev
    ```

Check your environment with to verify:

```bash
clang++ --version
cmake --version
llvm-config --version
```

## Build with CMake

From the repository root:

```bash
mkdir build
cd build
cmake .. 
cmake --build .
```

If CMake cannot locate LLVM:

```bash
llvm-config --cmakedir
```

Then:

```bash
cmake -DLLVM_DIR="$(llvm-config --cmakedir)" .. 
cmake --build .
```

If you have multiple LLVM installed, use: 
```bash
cmake -DLLVM_DIR="$(llvm-config-19 --cmakedir)" ..
```

The executable is normally:

```text
build/tinylang
```

From inside `build/`:

```bash
./tinylang ../examples/valid.tl --tokens
```

---

# 4. Running TinyLang

The driver in `src/main.cpp` provides four modes for you to debug and generate output for assignment submission.
The following are the commands to print the output of tokens, AST, semantic analysis and LLVM IR. 

## Print tokens (Coding Assignment 1)
Use this while developing the lexer.

```bash
./tinylang program.tl --tokens
```

Expected output:

```text
1:1  fn          fn
1:4  Identifier  main
1:8  (           (
1:9  )           )
...
```


## Print the AST (Coding Assignment 1)
Use this while developing the parser.

```bash
./tinylang program.tl --ast
```

For the following TinyLang program:

```text
fn main(): int {
    let x: int = 10;
    return x + 1;
}
```

the output should conceptually resemble:

```text
Function main -> int
  VarDecl x : int
    Integer(10)
  Return
    Binary(+)
      Variable(x)
      Integer(1)
```


## Run semantic analysis (Coding Assignment 2)
A syntactically valid but semantically invalid program should report semantic errors.

```bash
./tinylang program.tl --check
```

A valid program prints:

```text
Semantic analysis successful.
```

## Generate LLVM IR (Coding Assignment 2)

```bash
./tinylang program.tl --emit-llvm
```

A simple program may produce LLVM IR conceptually similar to:

```llvm
define i32 @main() {
entry:
  %x = alloca i32
  store i32 10, ptr %x
  %1 = load i32, ptr %x
  ret i32 %1
}
```

Temporary names (e.g., `%1`) may differ; that is normal.

---

# 5. Example input files

The `examples/` directory provides several useful tests, which you can use to test the correctness of your code.

- `examples/valid.tl`: 
Syntactically and semantically valid. Useful with the following commands:

```bash
--tokens
--ast
--check
```

- `examples/syntax_error.tl`:
Contains an intentional syntax error and is useful for testing Assignment 1 error handling.

- `examples/semantic_error.tl`:
Contains syntactically valid constructs that violate TinyLang semantic rules.

- `examples/codegen.tl`:
Uses the smaller subset intended for LLVM IR generation.

These are **limited** examples. You are encouraged to create your test `.tl` files and test your code. 


---

# 6. Recommended development workflow

For Coding Assignment 1:

1. identifiers and keywords
2. integer literals
3. punctuation and operators
4. verify `--tokens`
5. parse `main()`
6. parse blocks and statements
7. parse primary expressions
8. implement expression precedence
9. verify `--ast`
10. test syntax errors

For Coding Assignment 2:

1. symbol-table scopes
2. declaration and lookup
3. literal and variable types
4. binary-expression type checking
5. statement checking
6. verify `--check`
7. integer LLVM constants
8. variables: `alloca`/`load`/`store`
9. arithmetic expressions
10. return
11. required comparison/extended features

Compile and test frequently.
When something fails, identify the compiler stage first:

```text
Wrong token?
    -> Lexer.cpp

Correct tokens but wrong/rejected AST?
    -> Parser.cpp

Correct AST but semantic result is wrong?
    -> Semantic.cpp

Semantic analysis succeeds but LLVM IR is wrong?
    -> CodeGen.cpp
```

---

# 7. Lexical, syntax, and semantic errors

These are different error categories.

## Lexical error

A character cannot be converted into a valid TinyLang token:

```text
let x: int = 10 @ 2;
```

## Syntax error

The tokens are valid but do not match the grammar:

```text
let x: int = ;
```

## Semantic error

The program matches the grammar but violates language rules:

```text
let x: int = 10;
x = true;
```

Understanding these distinctions is one of the central goals of the project.

