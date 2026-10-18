# Assignment 2 Tests

Christian Quintero

These are the test cases for A2

To run:
    `cd build`
    `cmake --build .`
    `./tinylang ../A2_tests/test1.tl --check`

## Given Semantic Tests

Tests for `Semantic.cpp` that were given. All tests 1-6 match
the expected outcome.

### Test 1

Undeclared Var

- given
- expected - fail
- result - fail

### Test 2

duplicate declaration

- given
- expected - fail
- result - fail

### Test 3

assignment mismatch

- given
- expected - fail
- result - fail

### Test 4

operator mismatch

- given
- expected - fail
- result - fail

### Test 5

incorrect if condition

- given
- expected - fail
- result - fail

### Test 6

incorrect return type

- given
- expected - fail
- result - fail

## Created Semantic Tests

these are the tests i made for testing semantics using the `--check` flag.
You can run these like `./tinylang ../A2_tests/test15.tl --check`

### Test 7

integers as conditions

- created
- expected - fail
- result - fail

Should treat 0 and 1 as ints and not as truth values

### Test 8

mismatched return type in a block

- created
- expected - fail
- result - fail

should fail since main's return type is int

### Test 9

multiple errors thrown

- created
- expected - fail
- result - fail

the test throws 2 errors for conditions must be bool

### Test 10

var defined in parent scope

- created
- expected - pass
- result - pass

semantic analysis successful

### Test 11

var defined in inner scope

- created
- expected - pass
- result - pass

semantic analysis successful

### Test 12

var defined in then block but not else block. Should throw an error

- created
- expected - fail
- result - fail

Semantic error: Use of undeclared variable: 'y'

### Test 13

var defined in parent scope and used in else block

- created
- expected - pass
- result - pass

Semantic analysis successful.

### Test 14

test variable shadowing doesn't throw a duplicate declaration error

- created
- expected - pass
- result - pass

### Test 15

test adding a variable in an inner scope with one defined in outer scope

- created
- expected - pass
- result - pass

### Test 26

I went back and wrote more tests for semantics after writing the code gen ones

This test checks that an innerscope type mismatch with an outer scope type fails

- expected - fail
- result - fail

Semantic error: Invalid operands for operator '+': int and bool

### Test 27

similar to 26 but swaps the types and operator

Semantic error: Invalid operands for operator '/': bool and int

### Test 28

undeclared variable used in deeply nested scope

- expected - fail
- result - fail

### Test 29

deeply nested variables that are declared pass successfully

- expected - pass
- result - pass

## Created CodeGen Tests

tests I made for testing code gen with the `--emit-llvm` flag

### Test 16

if blocks should fail since I didn't do the extra credit. This may change when I do corrections.

- created
- expected - fail
- result - fail

Unknown statement type is not a var declaration, assignment, or return statement

### Test 17

else blocks also fail

- expected - fail
- result - fail

### Test 18

logic OR fails to generate

- expected - fail
- result - fail

### Test 19

logic AND fails to generate

- expected - fail
- result - fail

### Test 20

code with no if/else generates correctly

- expected - pass
- result - pass

### Test 21

code generation calls sub correctly in LLVM IR

- expected - pass
- result - pass

sub operation is called successfully `%0 = sub i32 %x1, %y2`

### Test 22

code gen calls multiply correctly in LLVM IR

- expected - pass
- result - pass

`%0 = mul i32 %x1, %y2`

### Test 23

code gen calls divide properly

- expected - pass
- result - pass

` %0 = sdiv i32 %x1, %y2`

### Test 24

code gen fails to generate bool statements since i didn't do extra credit

- expected - fail
- result - fail

Unknown expression is not an integer, variable, or binary expression

### Test 25

code gen works when only returning

- expected - pass
- result - pass

```text
; ModuleID = 'tinylang'
source_filename = "tinylang"

define i32 @main() {
entry:
  ret i32 24
}
```

### Test 30

comments aren't a part of the code gen

- expected - pass
- result - pass

```text
; ModuleID = 'tinylang'
source_filename = "tinylang"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 29, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  ret i32 %x1
}
```