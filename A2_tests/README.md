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