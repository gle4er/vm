#ifndef VM_H
#define VM_H

#include <stdio.h>

#define OVERFLOW 0x1
#define ZERO_ERR 0x2
#define OUT_OF_MEMORY 0x4 
#define FREQ_ERR 0x8 
#define COMMAND_ERR 0x10

char memory[100];
int reg_flag;

int sc_memoryInit();
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int *value);
int sc_memorySave(char *filename);
int sc_memoryLoad(char *filename);
int sc_regInit();
int sc_regSet(int regist, int value);
int sc_regGet(int regist, int *value);
int sc_commandEncode(int command, int operand, int *value);
int sc_commandDecode(int value, int *command, int *operand);

#endif
