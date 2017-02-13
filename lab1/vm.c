#include "vm.h"

//коды ошибок
//0 - всё ок
//-1 - выход из памяти
//-2 - невозможно открыть файл
//-3 - невозможно запись в файл
//-4 - невозможно прочитать файл
//-5 - неверное значение для регистра флагов
//-6 - неверное значение позиции регистра
//-7 - неверный операнд
//-8 - неверная команда

int sc_memoryInit()
{
    for (int i = 0; i < 100; i++)
        memory[i] = 0;
    return 0; //типа доне
}

int sc_memorySet(int address, int value)
{
    if (address >= 0 && address < 100) {
        memory[address] = value;
        return 0;
    } else {
        fprintf(stderr, "Out of memory\n");
        return -1;
    }
}

int sc_memoryGet(int address, int *value)
{
    if (address >= 0 && address < 100) {
        *value = memory[address];
        return 0;
    } else
        return -1;
}

int sc_memorySave(char *filename)
{
    int code = 0;
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Cannot open file\n");
        code = -2;
    }
    if (!fwrite(memory, sizeof(char), 100, file)) {
        fprintf(stderr, "Cannot write file\n");
        code = -3;
    }
    fclose(file);
    return code;
}

int sc_memoryLoad(char *filename)
{
    FILE *file = fopen(filename, "r");
    int code = 0;
    if (!file) {
        fprintf(stderr, "Cannot open file\n");
        code = -2;
    }
    sc_memoryInit();
    if (!fread(memory, sizeof(char), 100, file)) {
        fprintf(stderr, "Cannot read file\n");
        code = -4;
    }
    fclose(file);
    return code;
}

int sc_regInit()
{
    for (int i = 0; i < 5; i++)
        reg_flag[i] = 0;
    return 0;
}

int sc_regSet(int regist, int value)
{
    if (regist >= 0x01 && regist <= 0x10) {
        if (value == 0)
            reg_flag[regist - 1] &= value;
        if (value == 1)
            reg_flag[regist - 1] |= value;
        else {
            fprintf(stderr, "Wrong value for register\n");
            return -5;
        }
    } else {
        fprintf(stderr, "Wrong register selected\n");
        return -6;
    }
    return 0;
}

int sc_regGet(int regist, int *value)
{
    if (regist >= 0x01 && regist <= 0x10)
        *value = reg_flag[regist];
    else {
        fprintf(stderr, "Wrong register selected\n");
        return -6;
    }
    return 0;
}

int sc_commandEncode(int command, int operand, int *value)
{
    if (command >= 10 && command <= 76) {
        if (operand >= 0 && operand < 128) {
            *value = (command << 6) | operand;
        } else {
            fprintf(stderr, "Wrong operand\n");
            return -7;
        }
    } else {
        fprintf(stderr, "Wrong command\n");
        return -8;
    }
    return 0;
}

int sc_commandDecode(int value, int *command, int *operand)
{
    *command = (value >> 6);
    *operand = value & (~(*command << 6));
    if (*command >= 10 && *command <= 76) {
        if (*operand >= 0 && *operand < 128) {
            return 0;
        } else {
            fprintf(stderr, "Wrong operand\n");
            return -7;
        }
    } else {
        fprintf(stderr, "Wrong command\n");
        return -8;
    }
}
