//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#ifndef MILANCOMPILER_SCANNER_HPP
#define MILANCOMPILER_SCANNER_HPP

#include <fstream>
#include <map>
#include <string>
#include <iostream>

enum Token
{
    T_EOF,              // End of input stream
    T_ILLEGAL,          // Illegal symbol case
    T_IDENTIFIER,       // Identifier
    T_NUMBER,           // Integer literal
    T_BEGIN,            // Keyword "begin"
    T_END,              // Keyword "end"
    T_IF,               // Keyword "if"
    T_THEN,             // Keyword "then"
    T_ELSE,             // Keyword "else"
    T_FI,               // Keyword "fi"
    T_WHILE,            // Keyword "while"
    T_DO,               // Keyword "do"
    T_OD,               // Keyword "od"
    T_WRITE,            // Keyword "write"
    T_READ,             // Keyword "read"
    T_ASSIGN,           // Assignment operator
    T_ADDOP,            // Addition-type operation
    T_MULOP,            // Multiplication-type operation
    T_CMP,              // Relation-type operator
    T_LPAREN,           // Left parenthesis
    T_RPAREN,           // Right parenthesis
    T_SEMICOLON         // Semicolon (end of statement)
};

enum Cmp
{
    C_EQ,       // "="
    C_NE,       // "!="
    C_LT,       // "<"
    C_LE,       // "<="
    C_GT,       // ">"
    C_GE        // ">="
};

enum Arithmetic
{
    A_PLUS,             // "+"
    A_MINUS,            // "-"
    A_MULTIPLY,         // "*"
    A_DIVIDE            // "/"
};

class Scanner
{

    public:

        Scanner(std::istream & input_);

        /* Getters for fields */

        int getLineNumber() const;
        Token getToken() const;
        int getIntValue() const;
        std::string getStringValue() const;
        Cmp getCmpValue() const;
        Arithmetic getArithmeticValue() const;

        /* Transition to next lexeme */
        void nextToken();

    private:

        /* Skips spaces and changes line if '\n' found */
        void skipSpaces();
        /* Moves to next symbol */
        void nextChar();

        /* Identifier start check (must be alpha) */
        bool isIdentifierStart(char c);
        /* Identifier body check (must be alphanumeric) */
        bool isIdentifierBody(char c);

        /* FIELDS */

        /* Current code-string number */
        int lineNumber;

        /* Current lexeme */
        Token token;

        /* Current integer value */
        int intValue;

        /* Variable name */
        std::string stringValue;

        /* Current symbol */
        char currChar;

        /* Current cmp-operator value */
        Cmp cmpValue;

        /* Current arithmetic-operator value */
        Arithmetic arithmeticValue;

        /* Lexeme name as index, enum id as value */
        std::map<std::string, Token> keywords;

        /* Input stream */
        std::istream & input;

};


#endif //MILANCOMPILER_SCANNER_HPP
