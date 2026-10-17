#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

const char* tokenKindName(TokenKind kind) {
    /*
        This function returns a string representation of the given TokenKind by mapping
        each TokenKind to its corresponding keyword or operator string. If an unknown token
        is found, it returns a ? as a placeholder for an unrecognized token kind.

        Args:
            kind (TokenKind): The token kind to be converted to a string
        
        Returns:
            const char*: A string representation of the token kind
    */
    switch (kind) {
        case TokenKind::End: return "End";
        case TokenKind::Identifier: return "Identifier";
        case TokenKind::Integer: return "Integer";
        case TokenKind::KwFn: return "fn";
        case TokenKind::KwLet: return "let";
        case TokenKind::KwInt: return "int";
        case TokenKind::KwBool: return "bool";
        case TokenKind::KwIf: return "if";
        case TokenKind::KwElse: return "else";
        case TokenKind::KwReturn: return "return";
        case TokenKind::KwTrue: return "true";
        case TokenKind::KwFalse: return "false";
        case TokenKind::LParen: return "(";
        case TokenKind::RParen: return ")";
        case TokenKind::LBrace: return "{";
        case TokenKind::RBrace: return "}";
        case TokenKind::Colon: return ":";
        case TokenKind::Semicolon: return ";";
        case TokenKind::Comma: return ",";
        case TokenKind::Assign: return "=";
        case TokenKind::Plus: return "+";
        case TokenKind::Minus: return "-";
        case TokenKind::Star: return "*";
        case TokenKind::Slash: return "/";
        case TokenKind::Less: return "<";
        case TokenKind::Greater: return ">";
        case TokenKind::EqualEqual: return "==";
        case TokenKind::AndAnd: return "&&";
        case TokenKind::OrOr: return "||";
    }
    return "?";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek(size_t offset) const {
    /*
        this function allows you to look ahead in the input line without consuming characters.
        It has safe bounds checking and will return a null terminator if the position you are trying
        to peek at is beyond the end of the input string.

        Args:
            offset (size_t): The number of characters to look ahead from the current position.
                If 0, then you can look at the current character that pos_ is pointing to.
        
        Returns:
            char: The character at the specified offset from the current position, or '\0' if out of bounds.
    */
   
    // bounds check
    if (pos_ + offset >= source_.size()) return '\0';

    // return the character at the specified offset from the current position
    return source_[pos_ + offset];
}

bool Lexer::atEnd() const {
    /*
        This function is a check to see if the current position in the input string has
        reached the end of the string.

        Returns:
            bool: True if the current position is at or beyond the end of the input string, False otherwise.
    */
    return pos_ >= source_.size();
}

char Lexer::advance() {
    /*
        This function consumes the current character in the input string and advances the position
        to the next character. If the new current character is the newline character, it moves
        to the next line and resets the column count to 1. Otherwise, it simply increments the column count
        and parses like a regular for loop.

        NOTE: "Consume" here means read the current character, save its value, and move the position pointer forward - permanently.

        Returns:
            char: The character that was consumed from the input string.
    */

    // Consume the current character and advance the position
    char c = source_[pos_++];

    // if the character consumed was a newline, increment the line count and reset the column count to 1
    if (c == '\n') {
        ++line_;
        column_ = 1;
    }
    // else just increment the column count by 1 
    else {
        ++column_;
    }
    return c;
}

Token Lexer::makeToken(TokenKind kind,
                       const std::string& lexeme,
                       int line,
                       int column) const {
    return Token{kind, lexeme, line, column};
}

void Lexer::skipWhitespaceAndComments() {
    /*
        peeks at the current character that pos_ is pointing to and checks if it is a whitespace character or a /.
        If true, it will continue to consume and advance the pos_ pointer until it reaches a non-whitespace character or a character that is not a /
    */

    // loop skips continuous whitespace characters and comments
    while (!atEnd()) {
        // skip whitespace characters
        if (std::isspace(static_cast<unsigned char>(peek()))) {
            advance();
            continue;
        }

        // if current pos_ character is a / and next character is also a /, then a comment was found
        if (peek() == '/' && peek(1) == '/') {
            // skip the comment by advancing until the newline character
            while (!atEnd() && peek() != '\n')
                advance();
            continue;
        }

        break;
    }
}

Token Lexer::lexIdentifierOrKeyword() {
    /*
        this function is responsible for determining if the current sequence of characters
        in the input string is either an identifier or a keyword. 

        Logical Thinking:
        1. we are given the input line and we know pos_ is pointing to the first character of the input string.
        2. we will check the current character (peek()) to see if this char is a letter, number, or underscore, aka [A-Za-z_0-9]
        3. if valid, we continue to loop over the input string and consume characters until we reach a non valid character.
            when a valid character is found, concatenate it to the text string and advance the pos_ pointer to the next character.
        4. after the loop, we will have a token created
        5. Check if this token is a keyword to be recognized (fn let int bool if else return true false)
        6. if recognized keyword, return corresponding keyword type, else return identifier type
        
    */

    // TODO(A1): Read [A-Za-z_][A-Za-z0-9_]*.
    //
    // Then recognize these keywords:
    // fn let int bool if else return true false
    //
    // Otherwise return Identifier.

    int startLine = line_;
    int startColumn = column_;

    // the current token being created
    std::string input_token = "";

    // build a dictionary mapping input token strings to recognized keyword types
    std::unordered_map<std::string, TokenKind> recognized_keywords = {
        {"fn", TokenKind::KwFn},
        {"let", TokenKind::KwLet},
        {"int", TokenKind::KwInt},
        {"bool", TokenKind::KwBool},
        {"if", TokenKind::KwIf},
        {"else", TokenKind::KwElse},
        {"return", TokenKind::KwReturn},
        {"true", TokenKind::KwTrue},
        {"false", TokenKind::KwFalse},
    };

    // if the current character is a letter, number, or underscore, then consume characters
    // until a character that is not a letter, number, or underscore is found
    while (!atEnd() && (isalnum(peek()) || peek() == '_')) {
        // concatenate the character to build the current token
        input_token += advance();
    }

    // lookup the token
    auto iterator = recognized_keywords.find(input_token);
    if (iterator != recognized_keywords.end()) {
        // if found, build a token with the recognized keyword type and return it
        return makeToken(recognized_keywords[input_token], input_token,
                         startLine, startColumn);
    }
    
    // if not recognized, the type defaults to an identifier
    return makeToken(TokenKind::Identifier, input_token,
                     startLine, startColumn);
}

Token Lexer::lexInteger() {
    // TODO(A1): Consume a complete sequence of decimal digits.

    /*
        Logical Thinking:
        1. parse the input line till a digit is found.
        2. when found, concatenate the digit to the token string and advance
        3. keep looping until a non-digit character is found
        4. return the token with the type Integer
    */

    int startLine = line_;
    int startColumn = column_;

    std::string input_token = "";

    // loop until a non-digit character is found
    while (!atEnd() && std::isdigit(peek())) {
        // concatenate the digit to the token string and advance the pos_ pointer
        input_token += advance();
    }

    // build an integer token with the lexeme and return it
    return makeToken(TokenKind::Integer, input_token,
                     startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!atEnd()) {
        skipWhitespaceAndComments();
        if (atEnd()) break;

        int startLine = line_;
        int startColumn = column_;
        char c = peek();

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(lexInteger());
            continue;
        }

        /*
            logical thinking:
            1. we loop until punctuation is found
            2. when found we do a lookup to get the punctuation type
            3. if the punctuation is a special case, we also look at the next character, then do the lookup

        */

        // build a mapping of punctuation and operator strings to their corresponding TokenKind
        std::unordered_map<char, TokenKind> punctuation_and_operators = {
            {'(', TokenKind::LParen},
            {')', TokenKind::RParen},
            {'{', TokenKind::LBrace},
            {'}', TokenKind::RBrace},
            {':', TokenKind::Colon},
            {';', TokenKind::Semicolon},
            {',', TokenKind::Comma},
            {'=', TokenKind::Assign},
            {'+', TokenKind::Plus},
            {'-', TokenKind::Minus},
            {'*', TokenKind::Star},
            {'/', TokenKind::Slash},
            {'<', TokenKind::Less},
            {'>', TokenKind::Greater}
        };

        // handle special cases
        if (c == '=' && peek(1) == '=') {
            tokens.push_back(makeToken(TokenKind::EqualEqual, "==", line_, column_));
            advance();
            advance(); // move past the second '=' character
            continue;
        }

        if (c == '&' && peek(1) == '&') {
            tokens.push_back(makeToken(TokenKind::AndAnd, "&&", line_, column_));
            advance();
            advance(); // move past the second '&' character
            continue;
        }

        if (c == '|' && peek(1) == '|') {
            tokens.push_back(makeToken(TokenKind::OrOr, "||", line_, column_));
            advance();
            advance(); // move past the second '|' character
            continue;
        }

        // TODO(A1): Recognize punctuation and operators:
        // ( ) { } : ; ,
        // = + - * / < >
        // == && ||
        //
        // Remember that ==, && and || are two-character tokens.

        // if found in the dictionary, add the token to the vector by looking up its type and string representation
        auto iterator = punctuation_and_operators.find(c);
        if (iterator != punctuation_and_operators.end()) {
            tokens.push_back(
                makeToken(punctuation_and_operators[c], tokenKindName(punctuation_and_operators[c]), startLine, startColumn)
            );
            advance();
            continue;
        }
        
        // since all if checks call advance() and continue,
        // if the loop reaches this then something went wrong
        throw std::runtime_error(
            "Lexical error at line " + std::to_string(startLine) +
            ", column " + std::to_string(startColumn) +
            ": unexpected character '" + std::string(1, c) + "'"
        );
    }

    tokens.push_back(
        makeToken(TokenKind::End, "", line_, column_)
    );
    return tokens;
}
