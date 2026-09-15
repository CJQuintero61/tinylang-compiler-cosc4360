cmake_minimum_required(VERSION 3.20)
project(TinyLangCompiler LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(tinylang
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/AST.cpp
    src/Semantic.cpp
    src/CodeGen.cpp
)

llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
)

target_link_libraries(tinylang PRIVATE ${LLVM_LIBS})
