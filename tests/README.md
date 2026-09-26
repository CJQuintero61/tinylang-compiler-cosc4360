# Test Cases for Assignment 1

Christian Quintero

This folder has a description for each test case for assignment 1.

I am organizing this by first going through the given tests in the instructions
then by listing my own created tests.

## Begin Given Tests

### Test 1

- expected result - pass
- given/created - given

This test was given in the instructions and tests binary operator chaining.

### Test 2

- expected result - pass
- given/created - given

This test was given and tests operator chaining with parenthesis

### Test 3

- expected result - pass
- given/created - given

Tests an expression inside of an if clause with no else block.

### Test 4

- expected result - pass
- given/created - given

Tests ==, ||, and && can be parsed correctly. These are the special operators that are 2 characters long

---

## Begin Created Tests

this section has all the test cases I made that were not given in the instructions.

### Test 5

- expected result - pass
- given/created - created

Tests the parser can parse a variable initialization without an assignment.

### Test 6

- expected result - fail
- given/created - created

Tests that an else clause with no if clause fails to parse. This should fail because
the parseStatement function only recognizes 'let' 'if' 'return' or identifiers.

### Test 7

- expected result - fail
- given/created - created

Tests that the parser throws an error when you try to return something invalid. In this case,
the bool type is returned and crashes since primaryExpression only recognizes
int literals, bool literals, identifiers, or expressions

### Test 8

- expected result - fail
- given/created - created

I used codegen.tl as a template. This should fail due to a missing semicolon in the return statement

### Test 9

- expected result - fail
- given/created - created

should fail due to trying to use a binary operator with only 1 operand. This case tests addition
with only the left side

### Test 10

- expected result - fail
- given/created - created

should fail since division requires 2 operands and only 1 is given

### Test 11

- expected result - pass
- given/created - created

tests that a division operation is parsed to completion. This test tries to divide by 0
but the parser should not understand meaning, so this should pass.

### Test 12

- expected result - pass
- given/created - created

tests that comments are ignored

### Test 13

- expected result - fail
- given/created - created

tests that a walrus operator is not recognized. This was in a lot of the TODO
comments and I wanted to make sure this operator/syntax isn't supported.


### Test 14

- expected result - fail
- given/created - created

tests that a variable declaration without an = fails

### Test 15

- expected result - fail
- given/created - created

test that only 'main' is a recognized function name. other functions names
shouldn't be supported

### Test 16

- expected result - fail
- given/created - created

test that a function with a missing closing brace fails to compile

### Test 17

- expected result - fail
- given/created - created

tests that a statement with a missing closing parenthesis fails

### Test 18

- expected result - fail
- given/created - created

tests an empty file fails since there is no function declaration

### Test 19

- expected result - fail
- given/created - created

tests that tokens after the main function crash since tinylang only allows 1 function
and nothing else after it.

### Test 20

- expected result - pass
- given/created - created

tests a function with no return passes the parser. There is no syntax errors by having
no return statement. That is a semantic thing.