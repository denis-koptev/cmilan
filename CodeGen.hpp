//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#ifndef MILANCOMPILER_CODEGEN_HPP
#define MILANCOMPILER_CODEGEN_HPP


#include <vector>
#include <iostream>
#include <fstream>

enum Instruction
{
    NOP,            // no operation
    STOP,           // program ending
    LOAD,           // LOAD addr - load data in stack from memory at address 'addr'
    STORE,          // STORE addr - store data from stack head to memory at address 'addr'
    BLOAD,          // BLOAD addr - load at address: addr + value at stack head
    BSTORE,         // BSTORE addr - store at address: addr + value at stack head
    PUSH,           // PUSH n - load constant to stack
    POP,            // delete data from stack head
    DUP,            // copy data from stack head
    ADD,            // addition of data at stack head and result insertion instead of it
    SUB,            // subtraction of data at stack head and result insertion instead of it
    MULT,           // multiplication ...
    DIV,            // division ...
    INVERT,         // invertion of data sign at stack head
    COMPARE,        // COMPARE cmp - comparison between data at stack head with cmp operator
    JUMP,           // JUMP addr - jump to address addr
    JUMP_YES,       // JUMP_YES addr - jump if 1 at stack head
    JUMP_NO,        // JUMP_NO addr - jump if 0 at stack head
    INPUT,          // reading integer from stream and loading it to stack
    PRINT           // print to stream from stack head
};

class Command
{

    public:

        Command(Instruction instruction_);
        /* Instruction with argument */
        Command(Instruction instruction_, int arg_);

        void print(int address, std::ostream & output);

    private:

        /* Instruction code */
        Instruction instruction;

        /* Argument for instruction */
        int arg;
};

class CodeGen
{

    public:

        CodeGen(std::ostream & output);

        /* Instruction without argument addition to the end of program */
        void emit(Instruction instruction);

        /* Instruction with argument addition to the end of program */
        void emit(Instruction instruction, int arg);

        /* Instruction without argument addition to concrete address */
        void emitAt(int address, Instruction instruction);

        /* Instruction with argument addition to concrete address */
        void emitAt(int address, Instruction instruction, int arg);

        /* Get address next to the last instruction */
        int getCurrentAddress();

        /* Generation of empty instruction (NOP);
           @return: address */
        int reserve();

        /* Output of instructions sequence */
        void flush();

    private:

        std::ostream & output;
        std::vector<Command> commandBuffer;

};

#endif //MILANCOMPILER_CODEGEN_HPP
