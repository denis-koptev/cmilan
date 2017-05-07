//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#include <algorithm>
#include "Scanner.hpp"

Scanner::Scanner(std::istream & input_)
        : lineNumber(1), input(input_)
{
    keywords["begin"] = T_BEGIN;
    keywords["end"] = T_END;
    keywords["if"] = T_IF;
    keywords["then"] = T_THEN;
    keywords["else"] = T_ELSE;
    keywords["fi"] = T_FI;
    keywords["while"] = T_WHILE;
    keywords["do"] = T_DO;
    keywords["od"] = T_OD;
    keywords["write"] = T_WRITE;
    keywords["read"] = T_READ;
    keywords["enum"] = T_ENUM;
    keywords["{"] = T_LBRACE;
    keywords["}"] = T_RBRACE;
    keywords[","] = T_COMMA;

    nextChar();
}

int Scanner::getLineNumber() const
{
    return lineNumber;
}

Token Scanner::getToken() const
{
    return token;
}

int Scanner::getIntValue() const
{
    return intValue;
}

std::string Scanner::getStringValue() const
{
    return stringValue;
}

Cmp Scanner::getCmpValue() const
{
    return cmpValue;
}

Arithmetic Scanner::getArithmeticValue() const
{
    return arithmeticValue;
}

void Scanner::nextToken()
{
    skipSpaces();

    /* Comments processing */
    while (currChar == '/') {
        nextChar();
        if (currChar == '*') {
            nextChar();
            bool insideComment = true;
            while (insideComment) {
                while (currChar != '*' && !input.eof()) {
                    nextChar();
                }
                if (input.eof()) {
                    token = T_EOF;
                    return;
                }
                nextChar();
                if (currChar == '/') {
                    insideComment = false;
                    nextChar();
                }
            }
        } else {    // it is division
            token = T_MULOP;
            arithmeticValue = A_DIVIDE;
            return;
        }

        skipSpaces();
    }

    /* EOF processing */
    if (input.eof()) {
        token = T_EOF;
        return;
    }

    /* Integer processing */
    if (isdigit(currChar)) {
        int val = 0;
        while (isdigit(currChar)) {
            val = val * 10 + (currChar - '0');
            nextChar();
        }
        token = T_NUMBER;
        intValue = val;
    } else if (isIdentifierStart(currChar)) { // Variable processing
        std::string buff;
        while (isIdentifierBody(currChar)) {
            buff += currChar;
            nextChar();
        }

        std::transform(buff.begin(), buff.end(), buff.begin(), (int (*)(int)) std::tolower);
        // Keywords processing
        std::map<std::string, Token>::iterator keywordsIter = keywords.find(buff);

        if (keywordsIter == keywords.end()) {
            token = T_IDENTIFIER;
            stringValue = buff;
        } else {
            token = keywordsIter->second;
        }
    } else {  // Other variants (not digit, alpha or EOF)
        switch (currChar) {
            case '(': {
                token = T_LPAREN;
                nextChar();
                break;
            }
            case ')': {
                token = T_RPAREN;
                nextChar();
                break;
            }
            case ';': {
                token = T_SEMICOLON;
                nextChar();
                break;
            }
            case ':': {
                nextChar();
                if (currChar == '=') {
                    token = T_ASSIGN;
                    nextChar();
                } else {
                    token = T_ILLEGAL;
                }
                break;
            }
            case '<': {
                token = T_CMP;
                nextChar();
                if (currChar == '=') {
                    cmpValue = C_LE;
                    nextChar();
                } else {
                    cmpValue = C_LT;
                }
                break;
            }
            case '>': {
                token = T_CMP;
                nextChar();
                if (currChar == '=') {
                    cmpValue = C_GE;
                    nextChar();
                } else {
                    cmpValue = C_GT;
                }
                break;
            }
            case '!': {
                nextChar();
                if (currChar == '=') {
                    nextChar();
                    token = T_CMP;
                    cmpValue = C_NE;
                } else {
                    token = T_ILLEGAL;
                }
                break;
            }
            case '=': {
                token = T_CMP;
                cmpValue = C_EQ;
                nextChar();
                break;
            }
            case '+': {
                token = T_ADDOP;
                arithmeticValue = A_PLUS;
                nextChar();
                break;
            }
            case '-': {
                token = T_ADDOP;
                arithmeticValue = A_MINUS;
                nextChar();
                break;
            }
            case '*': {
                token = T_MULOP;
                arithmeticValue = A_MULTIPLY;
                nextChar();
                break;
            }
            case '{': {
                token = T_LBRACE;
                nextChar();
                break;
            }
            case ',': {
                token = T_COMMA;
                nextChar();
                break;
            }
            case '}': {
                token = T_RBRACE;
                nextChar();
                break;
            }

                // Division is processed earlier

            default: {
                token = T_ILLEGAL;
                nextChar();
                break;
            }
        }
    }
}

void Scanner::skipSpaces()
{
    while (isspace(currChar)) {
        if (currChar == '\n') {
            ++lineNumber;
        }
        nextChar();
    }
}

void Scanner::nextChar()
{
    currChar = (char) input.get();
}

bool Scanner::isIdentifierStart(char c)
{
    return isalpha(c);
}

bool Scanner::isIdentifierBody(char c)
{
    // c == ':' is for enums (which act like variables)
    return isalnum(c) || c == ':';
}
