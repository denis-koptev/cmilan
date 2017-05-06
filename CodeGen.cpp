//
// Created by Koptev Denis
// Peter the Great St.Petersburg Polytechnic University
// Saint Petersburg, 2017
// Based on existing CMilan compiler code
//

#include "CodeGen.hpp"

Command::Command(Instruction instruction_)
        : instruction(instruction_), arg(0)
{}

Command::Command(Instruction instruction_, int arg_)
        : instruction(instruction_), arg(arg_)
{}

void Command::print(int address, std::ostream & output)
{
    output << address << ":\t";

    switch (instruction) {
        case NOP: {
            output << "NOP";
            break;
        }

        case STOP: {
            output << "STOP";
            break;
        }

        case LOAD: {
            output << "LOAD\t" << arg;
            break;
        }

        case STORE: {
            output << "STORE\t" << arg;
            break;
        }

        case BLOAD: {
            output << "BLOAD\t" << arg;
            break;
        }

        case BSTORE: {
            output << "BSTORE\t" << arg;
            break;
        }

        case PUSH: {
            output << "PUSH\t" << arg;
            break;
        }

        case POP: {
            output << "POP";
            break;
        }

        case DUP: {
            output << "DUP";
            break;
        }

        case ADD: {
            output << "ADD";
            break;
        }

        case SUB: {
            output << "SUB";
            break;
        }

        case MULT: {
            output << "MULT";
            break;
        }

        case DIV: {
            output << "DIV";
            break;
        }

        case INVERT: {
            output << "INVERT";
            break;
        }

        case COMPARE: {
            output << "COMPARE\t" << arg;
            break;
        }

        case JUMP: {
            output << "JUMP\t" << arg;
            break;
        }

        case JUMP_YES: {
            output << "JUMP_YES\t" << arg;
            break;
        }

        case JUMP_NO: {
            output << "JUMP_NO\t" << arg;
            break;
        }

        case INPUT: {
            output << "INPUT";
            break;
        }

        case PRINT: {
            output << "PRINT";
            break;
        }
    }

    output << std::endl;
}

CodeGen::CodeGen(std::ostream & output_)
        : output(output_)
{}

void CodeGen::emit(Instruction instruction)
{
    commandBuffer.push_back(Command(instruction));
}

void CodeGen::emit(Instruction instruction, int arg)
{
    commandBuffer.push_back(Command(instruction, arg));
}

void CodeGen::emitAt(int address, Instruction instruction)
{
    commandBuffer.at((unsigned int) address) = Command(instruction);
}

void CodeGen::emitAt(int address, Instruction instruction, int arg)
{
    commandBuffer.at((unsigned int) address) = Command(instruction, arg);
}

int CodeGen::getCurrentAddress()
{
    return commandBuffer.size();
}

int CodeGen::reserve()
{
    emit(NOP);
    return commandBuffer.size() - 1;
}

void CodeGen::flush()
{
    int count = commandBuffer.size();
    for (int address = 0; address < count; ++address) {
        commandBuffer.at((unsigned int) address).print(address, output);
    }
    output.flush();
}
