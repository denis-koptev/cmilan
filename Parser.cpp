//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#include "Parser.hpp"

#include <sstream>

Parser::Parser(std::istream & input_, std::ostream & output_)
        : output(output_), error(false), recovered(true), lastVar(0),
          codegen(output_), scanner(input_)
{
    nextLexeme();
}

void Parser::parse()
{
    program();
    if (!error) {
        codegen.flush(); // write to output
    }
}

void Parser::program()
{
    matchLexemeSafe(T_BEGIN);
    statementList();
    matchLexemeSafe(T_END);
    codegen.emit(STOP);
}

void Parser::statementList()
{
    if (!checkLexeme(T_END) && !checkLexeme(T_ELSE) &&
        !checkLexeme(T_OD) && !checkLexeme(T_FI)) {
        do {
            statement();
        } while (matchLexeme(T_SEMICOLON));
    }
}

void Parser::statement()
{
    if (checkLexeme(T_IDENTIFIER)) {
        int varAddress = getVariableIdx(scanner.getStringValue());
        nextLexeme();
        matchLexemeSafe(T_ASSIGN);
        expression();
        codegen.emit(STORE, varAddress);
    } else if (matchLexeme(T_IF)) {
        relation();
        int jumpNoAddress = codegen.reserve();
        matchLexemeSafe(T_THEN);
        statementList();

        if (matchLexeme(T_ELSE)) {
            int jumpAddress = codegen.reserve();
            // Start of else block (jump if false condition)
            codegen.emitAt(jumpNoAddress, JUMP_NO, codegen.getCurrentAddress());
            statementList();
            // End of else block (jump if true condition)
            codegen.emitAt(jumpAddress, JUMP, codegen.getCurrentAddress());
        } else {
            codegen.emitAt(jumpNoAddress, JUMP_NO, codegen.getCurrentAddress());
        }

        matchLexemeSafe(T_FI);

    } else if (matchLexeme(T_WHILE)) {
        int conditionAddress = codegen.getCurrentAddress();
        relation();
        int jumpNoAddress = codegen.reserve();
        matchLexemeSafe(T_DO);
        statementList();
        matchLexemeSafe(T_OD);
        codegen.emit(JUMP, conditionAddress); // check condition
        codegen.emitAt(jumpNoAddress, JUMP_NO, codegen.getCurrentAddress()); // if false
    } else if (matchLexeme(T_WRITE)) {
        matchLexemeSafe(T_LPAREN);
        expression();
        matchLexemeSafe(T_RPAREN);
        codegen.emit(PRINT);
    } else {
        reportError("Statement expected");
    }
}

// <expression> -> <term> | <term> + <term> | <term> - <term>
void Parser::expression()
{
    term();
    while (checkLexeme(T_ADDOP)) {
        Arithmetic op = scanner.getArithmeticValue();
        nextLexeme();
        term();
        codegen.emit(op == A_PLUS ? ADD : SUB);
    }
}

// <expression> -> <factor> | <factor> + <factor> | <factor> - <factor>
void Parser::term()
{
    factor();
    while (checkLexeme(T_MULOP)) {
        Arithmetic op = scanner.getArithmeticValue();
        nextLexeme();
        factor();
        codegen.emit(op == A_MULTIPLY ? MULT : DIV);
    }
}

// <factor> -> number | identifier | -<factor> | (<expression>) | READ
void Parser::factor()
{
    if (checkLexeme(T_NUMBER)) {
        int val = scanner.getIntValue();
        nextLexeme();
        codegen.emit(PUSH, val);
    } else if (checkLexeme(T_IDENTIFIER)) {
        int varAddress = getVariableIdx(scanner.getStringValue());
        nextLexeme();
        codegen.emit(LOAD, varAddress);
    } else if (checkLexeme(T_ADDOP) && scanner.getArithmeticValue() == A_MINUS) {
        nextLexeme();
        factor();
        codegen.emit(INVERT); // Negative value
    } else if (matchLexeme(T_LPAREN)) {
        expression();
        matchLexemeSafe(T_RPAREN);
    } else if (matchLexeme(T_READ)) {
        codegen.emit(INPUT);
    } else {
        reportError("Expression expected");
    }
}

void Parser::relation()
{
    expression();
    if (checkLexeme(T_CMP)) {
        Cmp cmp = scanner.getCmpValue();
        nextLexeme();
        expression();
        switch (cmp) {
            case C_EQ: {
                codegen.emit(COMPARE, 0); // 0 - just argument for operator
                break;
            }
            case C_NE: {
                codegen.emit(COMPARE, 1);
                break;
            }
            case C_LT: {
                codegen.emit(COMPARE, 2);
                break;
            }
            case C_GT: {
                codegen.emit(COMPARE, 3);
                break;
            }
            case C_LE: {
                codegen.emit(COMPARE, 4);
                break;
            }
            case C_GE: {
                codegen.emit(COMPARE, 5);
                break;
            }
        }
    } else {
        reportError("Comparison operator expected");
    }
}

bool Parser::checkLexeme(Token t)
{
    return scanner.getToken() == t;
}

bool Parser::matchLexeme(Token t)
{
    if (scanner.getToken() == t) {
        scanner.nextToken();
        return true;
    }
    return false;
}

void Parser::nextLexeme()
{
    scanner.nextToken();
}

// function
// implementation - later
const std::string tokenToString(Token t);

void Parser::matchLexemeSafe(Token t)
{
    if (!matchLexeme(t)) {
        error = true;

        std::ostringstream msg;
        msg << tokenToString(scanner.getToken())
            << " found while " << tokenToString(t)
            << " expected.";
        reportError(msg.str());

        recover(t);
    }
}

void Parser::reportError(const std::string & message)
{
    std::cerr << "Error at line "
              << scanner.getLineNumber()
              << ": " << message << std::endl;
    error = true;
}

int Parser::getVariableIdx(const std::string & name)
{
    VarTable::iterator varTableIter = variables.find(name);
    if (varTableIter == variables.end()) {
        variables[name] = lastVar;
        return lastVar++;
    } else {
        return varTableIter->second;
    }
}

void Parser::recover(Token t)
{
    while (!checkLexeme(t) && !checkLexeme(T_EOF)) {
        nextLexeme();
    }

    if (checkLexeme(t)) {
        nextLexeme();
    }
}

/* TOKEN TO STRING */

static const std::string tokenNames[] = {
        "end of file",
        "illegal token",
        "identifier",
        "number",
        "'BEGIN'",
        "'END'",
        "'IF'",
        "'THEN'",
        "'ELSE'",
        "'FI'",
        "'WHILE'",
        "'DO'",
        "'OD'",
        "'WRITE'",
        "'READ'",
        "':='",
        "'+' or '-'",
        "'*' or '/'",
        "comparison operator",
        "'('",
        "')'",
        "';'",
};

const std::string tokenToString(Token t)
{
    return tokenNames[t];
}
