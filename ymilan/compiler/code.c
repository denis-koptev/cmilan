#include "code.h"
#include "milan.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct opcode_info {
        char *name;
        int need_arg;
} opcode_info;

opcode_info opcodes_table[] = {
        {"STOP",     0},
        {"LOAD",     1},
        {"STORE",    1},
        {"BLOAD",    1},
        {"BSTORE",   1},
        {"PUSH",     1},
        {"POP",      0},
        {"DUP",      0},
        {"INVERT",   0},
        {"ADD",      0},
        {"SUB",      0},
        {"MULT",     0},
        {"DIV",      0},
        {"COMPARE",  1},
        {"JUMP",     1},
        {"JUMP_YES", 1},
        {"JUMP_NO",  1},
        {"INPUT",    0},
        {"PRINT",    0}
};

/* Edges for cycle for break and continue */

bool in_cycle = false;
int start_of_cycle = -1;
int end_of_cycle = -1;

int opcode_count = sizeof(opcodes_table) / sizeof(opcode_info);

unsigned int generate_command(FILE* stream, unsigned int address, opcode op, int arg)
{
        if(op < opcode_count) {
                if(opcodes_table[op].need_arg) {
                        fprintf(stream, "%d:\t%s\t\t\t%d\n", address,
                                        opcodes_table[op].name, arg);
                }
                else {
                        fprintf(stream, "%d:\t%s\n", address, opcodes_table[op].name);
                }
        }
        else {
                milan_error("Illegal opcode");
        }

        return address + 1;
}

int get_var_address(ast_node* ast)
{
        return find_name(ast->data.string_value);
}

unsigned int generate_code(FILE* stream, unsigned int address, ast_node* ast)
{
        ast_node* ptr;
        opcode op;
        unsigned int tmpaddr1, tmpaddr2;
        unsigned int new_address;
        
        if(ast) {
                switch(ast->type) {
                        case Node_Block:
                                ptr = ast->sub[0];
                                new_address = address;
                                while(ptr) {
                                        new_address = generate_code(stream, new_address, ptr);
                                        ptr = ptr->next;
                                }
                                break;

                        case Node_Const:
                                new_address = generate_command(stream, address, PUSH, ast->data.integer_value);
                                break;

                        case Node_Var:
                                new_address = generate_command(stream, address, LOAD, get_var_address(ast));
                                break;

                        case Node_Read:
                                new_address = generate_command(stream, address, INPUT, 0);
                                break;
                                
                        case Node_Expr:
                                new_address = generate_code(stream, address, ast->sub[0]);
                                
                                if(ast->data.integer_value != OP_NEG) {
                                        new_address = generate_code(stream, new_address, ast->sub[1]);
                                }
                                        
                                switch(ast->data.integer_value) {
                                        case OP_ADD: op = ADD; break;
                                        case OP_SUB: op = SUB; break;
                                        case OP_MUL: op = MULT; break;
                                        case OP_DIV: op = DIV; break;
                                        case OP_NEG: op = INVERT; break;
                                        default:
                                                     milan_error("Unknown arithmetical operator");
                                                     break;
                                }

                                new_address = generate_command(stream, new_address, op, 0);
                                break;
                                
                        case Node_Assign:
                                new_address = generate_code(stream, address, ast->sub[0]);
                                new_address = generate_command(stream, new_address, STORE, get_var_address(ast));
                                break;
                                
                        case Node_Write:
                                new_address = generate_code(stream, address, ast->sub[0]);
                                new_address = generate_command(stream, new_address, PRINT, 0);
                                break;
                                
                        case Node_Cond:
                                new_address = generate_code(stream, address, ast->sub[0]);
                                new_address = generate_code(stream, new_address, ast->sub[1]);
                                new_address = generate_command(stream, new_address, COMPARE, ast->data.integer_value);
                                break;
                                
                        case Node_If:
                                new_address = generate_code(stream, address, ast->data.ast_value);
                                tmpaddr1 = new_address++;
                                new_address = generate_code(stream, new_address, ast->sub[0]);

                                if(ast->sub[1]) {
                                        generate_command(stream, tmpaddr1, JUMP_NO, new_address + 1);
                                        tmpaddr1 = new_address++;
                                        new_address = generate_code(stream, new_address, ast->sub[1]);
                                        generate_command(stream, tmpaddr1, JUMP, new_address);
                                }
                                else {
                                        generate_command(stream, tmpaddr1, JUMP_NO, new_address);
                                }
                                break;

                        /*
                        * While cycle will be formed as described:
                        * 1: condition (start_of_cycle)
                        * 2: if true -> jump 4
                        * 3: else jump to the end (will be formed after all)
                        * 4: body
                        * 5: end of cycle
                        */
                                
                        case Node_While:

                                // start of cycle
                                in_cycle = true;
                                tmpaddr1 = address;
                                start_of_cycle = tmpaddr1;

                                // condition check
                                new_address = generate_code(stream, address, ast->data.ast_value);

                                // if true
                                new_address = generate_command(stream, new_address, JUMP_YES, new_address + 2);

                                // if false
                                tmpaddr2 = new_address++;
                                end_of_cycle = tmpaddr2;

                                // cycle code. here can be break and continue
                                new_address = generate_code(stream, new_address, ast->sub[0]);

                                // back to check condition
                                new_address = generate_command(stream, new_address, JUMP, tmpaddr1);

                                // for cycle-exit
                                in_cycle = false;
                                generate_command(stream, tmpaddr2, JUMP, new_address);

                                break;



                        case Node_Repeat:

                                in_cycle = true;

                                // jump straight to the body of cycle
                                new_address = generate_command(stream, address, JUMP, address + 3);
                                
                                // 2 cells are reserved for start and end of cycle
                                start_of_cycle = address + 1; // jumps to condition
                                end_of_cycle = address + 2;  // jumps after condition

                                new_address += 2;

                                // now we know body address
                                tmpaddr1 = new_address; // body address

                                // here is body
                                new_address = generate_code(stream, new_address, ast->sub[0]);

                                // now we know condition address
                                tmpaddr2 = new_address;

                                // here is condition
                                new_address = generate_code(stream, new_address, ast->data.ast_value);

                                // if it is true - go to the body
                                new_address = generate_command(stream, new_address, JUMP_YES, tmpaddr1);
                                // else - nothing

                                // let's generate start and end jumps
                                generate_command(stream, start_of_cycle, JUMP, tmpaddr2);
                                // last new_address - is the end of cycle
                                generate_command(stream, end_of_cycle, JUMP, new_address);

                                in_cycle = false;

                                break;

                        case Node_Break:
                                if (in_cycle) {
                                    new_address = generate_command(stream, address, JUMP, end_of_cycle);
                                } else {
                                    milan_error("BREAK is not in cycle");
                                }
                                break;
                                
                        case Node_Continue:
                                if (in_cycle) {
                                    new_address = generate_command(stream, address, JUMP, start_of_cycle);
                                } else {
                                    milan_error("CONTINUE is not in cycle");
                                }
                                break;

                        default:
                                milan_error("Unknown node type");
                                break;
                }
        
                return new_address;
        }
        else {
                return address;
        }
}

void generate_program(FILE* stream, ast_node* ast)
{
        unsigned int address;
        
        address = generate_code(stream, 0, ast);
        generate_command(stream, address, STOP, 0);
}

