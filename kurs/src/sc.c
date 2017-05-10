#include "../include/sc.h"
#include "../include/gui.h"
#include "../include/alu.h"

int sc_memoryInit()
{
    for (int i = 0; i < 100; i++)
        memory[i] = 0;
    sc_regSet(OUT_OF_MEMORY, 0);
    return OK;
}

int sc_memorySet(int address, int value)
{
    if (address >= 0 && address < 100) {
        memory[address] = value;
        return OK;
    } else {
        sc_regSet(OUT_OF_MEMORY, 1);
        return OOM;
    }
}

int sc_memoryGet(int address, int *value)
{
    if (address >= 0 && address < 100) {
        *value = memory[address];
        return OK;
    } else {
        sc_regSet(OUT_OF_MEMORY, 1);
        return OOM;
    }
}

int sc_memorySave(char *filename)
{
    int code = OK;
    FILE *file = fopen(filename, "w");
    if (!file) 
        code = OPEN_ERR;
    if (!fwrite(memory, 4, 100, file)) 
        code = WRITE_ERR;
    fclose(file);
    return code;
}

int sc_memoryLoad(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return OPEN_ERR;
    sc_memoryInit();
    if (!fread(memory, 4, 100, file)) 
        return READ_ERR;
    fclose(file);
    return OK;
}

int sc_regInit()
{
    reg_flag = 0;
    return OK;
}

int sc_regSet(int regist, int value)
{
    if (regist >= 0x01 && regist <= 0x10) {
        if (value == 0)
            reg_flag &= ~regist;
        else if (value == 1)
            reg_flag |= regist;
        else 
            return WRONG_VALUE;
    } else 
        return WRONG_REGISTER;
    return OK;
}

int sc_regGet(int regist, int *value)
{
    if (regist >= 0x01 && regist <= 0x10)
        *value = (reg_flag & regist) > 0 ? 1 : 0;
    else 
        return WRONG_REGISTER;
    return OK;
}

int sc_commandEncode(int command, int operand, int *value)
{
    if ((command >= 10 && command <= 76) || command == 0) {
        if (operand >= 0 && operand < 128)
            *value = (command << 7) | operand;
        else if (operand > -128 && operand < 0) {
            operand *= -1;
            *value = (command << 7) | operand;
            *value |= 1 << 14;
        } else {
            return WRONG_OPERAND;
        }
    } else {
        return WRONG_COMMAND;
    }
    return OK;
}

int sc_commandDecode(int value, int *command, int *operand)
{
    int flg = 0;
    if (value & (1 << 14)) {
        flg++;
        value = value & ~(1 << 14);
    }
    *command = (value >> 7);
    *operand = value & (~(*command << 7));
    if (flg)
        *operand *= -1;
    if ((*command >= 10 && *command <= 76) || *command == 0) {
        if (*operand > -128 && *operand < 128) {
            return OK;
        } else {
            *operand = 0;
            sc_regSet(OVERFLOW, 1);
            return WRONG_OPERAND;
        }
    } else {
        *command = 0;
        return WRONG_COMMAND;
    }
}

int sc_instGet(int *value)
{
    *value = inst_cnt;
    return OK;
}

int sc_instSet(int value)
{
    if (value >= 100)
        return WRONG_VALUE;
    inst_cnt = value;
    return OK;
}

int sc_accumGet(int *value)
{
    *value = accum;
    return OK;
}

int sc_accumSet(int value)
{
    accum = value;
    return OK;
}

void CU()
{
    int freq_flg = 0;
    sc_regGet(FREQ_ERR, &freq_flg);
    if (freq_flg)
        return;
    int inst_curr = 0;
    sc_instGet(&inst_curr);
    int memory_curr;
    sc_memoryGet(inst_curr, &memory_curr);
    if (!memory_curr)
        return;
    int command = 0,
        operand = 0,
        flg = 0;
    sc_commandDecode(memory_curr, &command, &operand);
    sc_regGet(OVERFLOW, &flg);
    if (flg) {
        sc_regSet(FREQ_ERR, 1);
        return;
    }
    ALU(command, operand);
    sc_regGet(COMMAND_ERR, &flg);
    if (flg) {
        sc_regSet(FREQ_ERR, 1);
        return;
    }
    sc_regGet(FREQ_ERR, &flg);
    if (!inst_curr || !flg)
        sc_instSet(++inst_curr);
    interface();
}
