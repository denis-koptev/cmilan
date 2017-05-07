//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#ifndef MILANCOMPILER_PARSER_HPP
#define MILANCOMPILER_PARSER_HPP


#include "Scanner.hpp"
#include "CodeGen.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

class Parser
{
    public:

        Parser(std::istream & input_, std::ostream & output_);

        void parse();

    private:

        typedef std::map<std::string, int> VarTable;

        /* PARSING METHODS */

        /* Parses 'BEGIN ... END' section */
        void program();
        /* Parses list of statements */
        void statementList();
        /* Parses statement */
        void statement();
        /* Parses arithmetical expression */
        void expression();
        /* Parses term (in add and sub operations) */
        void term();
        /* Parses factor (in mult and div operations) */
        void factor();
        /* Parses logical condition */
        void relation();
        /* Parses values inside enum */
        void enumeration();

        /* CHECK AND RECOVERY METHODS */

        /* Compares lexeme with sample */
        bool checkLexeme(Token t);
        /* Compares lexem with sample and removes it from stream if equals */
        bool matchLexeme(Token t);
        /* Transition to next lexeme */
        void nextLexeme();
        /* The same as match, but with error message generation and recovery*/
        void matchLexemeSafe(Token t);
        /* Generates error message */
        void reportError(const std::string & message);
        /* Recovery after error */
        void recover(Token t);

        /*  METHODS FOR VARIABLES */

        /* Adds variable to list if needed and returns its number */
        int getVariableIdx(const std::string & name);
        /* Adds variable to act as enum (returns address) */
        int addVariable(const std::string & name);

        /* FIELDS */

        Scanner scanner;
        CodeGen codegen;

        std::ostream & output;

        bool error;
        bool recovered;

        VarTable variables;
        int lastVar;

};


#endif //MILANCOMPILER_PARSER_HPP
