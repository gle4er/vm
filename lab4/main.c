#include "bc.h"
#include "mt.h"
#include "rk.h"
#include "vm.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int *alph = NULL;

void print_mem(int *x, int *y)
{
    bc_box(1, 1, 61, 12);
    mt_gotoXY(30, 0);
    char tmp[] = " Memory ";
    write(1, tmp, sizeof(tmp));
    for (int i = 0; i < 100; i++) {
        mt_gotoXY(2 + 6 * (i / 10), 2 + (i % 10));
        if (*x > 9)
            *x = 0;
        if (*x < 0)
            *x = 9;
        if (*y > 9)
            *y = 0;
        if (*y < 0)
            *y = 9;
        if (i / 10 == *y && i % 10 == *x)
            mt_setbgcolor(red);
        int command, operand, value;
        sc_memoryGet(i, &value);
        sc_commandDecode(value, &command, &operand);
        char tmp1[10]; 
        if (command < 16 && operand < 16)
            sprintf(tmp1, "0%x:0%x", command, operand);
        else if (command < 16 && operand >= 16)
            sprintf(tmp1, "0%x:%x", command, operand);
        else if (command >= 16 && operand < 16)
            sprintf(tmp1, "%x:0%x", command, operand);
        else
            sprintf(tmp1, "%x:%x", command, operand);
        write(1, tmp1, strlen(tmp1));
        mt_setbgcolor(deflt);
    }
}

void print_accum()
{
    bc_box(62, 1, 22, 3);
    mt_gotoXY(66, 1);
    char tmp[] = " accumulator ";
    write(1, tmp, sizeof(tmp));
    mt_gotoXY(71, 2);
    char tmp1[] = "+9999";
    write(1, tmp1, sizeof(tmp1));
}

void print_instcnt()
{
    bc_box(62, 4, 22, 3);
    mt_gotoXY(63, 4);
    char tmp[] = " instructionCounter ";
    write(1, tmp, sizeof(tmp));
    mt_gotoXY(71, 5);
    char tmp1[] = "+0000";
    write(1, tmp1, sizeof(tmp1));
}

void print_operation()
{
    bc_box(62, 7, 22, 3);
    mt_gotoXY(68, 7);
    char tmp[] = " Operation ";
    write(1, tmp, sizeof(tmp));
    char tmp1[] = "+00 : 00";
    mt_gotoXY(69, 8);
    write(1, tmp1, sizeof(tmp1));
}

void print_flg()
{
    bc_box(62, 10, 22, 3);
    mt_gotoXY(69, 10);
    char tmp[] = " Flags ";
    write(1, tmp, strlen(tmp));
    char tmp1[] = "OZMFC";
    mt_gotoXY(67, 11);
    for (int i = 0; i < 5; i++) {
        int value;
        sc_regGet(1 << i, &value);
        if (value)
            mt_setfgcolor(red);
        char tmp2[3];
        sprintf(tmp2, "%c ", tmp1[i]);
        write(1, tmp2, strlen(tmp2));
        mt_setfgcolor(deflt);
    }
}

void print_membc(int x, int y)
{
    bc_box(1, 13, 50, 10);
    int value, command, operand;
    sc_memoryGet(10 * y + x, &value);
    sc_commandDecode(value, &command, &operand);
    int big[] = {alph[command / 16 * 2], alph[command / 16 * 2 + 1]};
    bc_printbigchar(big, 2 + 10 * 0, 14, deflt, deflt);
    int big1[] = {alph[command % 16 * 2], alph[command % 16 * 2 + 1]};
    bc_printbigchar(big1, 2 + 10 * 1, 14, deflt, deflt);
    int big2[] = {alph[16 * 2], alph[16 * 2 + 1]};
    bc_printbigchar(big2, 2 + 10 * 2, 14, deflt, deflt);
    int big3[] = {alph[operand / 16 * 2], alph[operand / 16 * 2 + 1]};
    bc_printbigchar(big3, 2 + 10 * 3, 14, deflt, deflt);
    int big4[] = {alph[operand % 16 * 2], alph[operand % 16 * 2 + 1]};
    bc_printbigchar(big4, 2 + 10 * 4, 14, deflt, deflt);
}

void print_keys()
{
    bc_box(51, 13, 33, 10);
    mt_gotoXY(52, 13);
    char tmp[] = " Keys: ";
    write(1, tmp, strlen(tmp));
    char *tmp1[] = {"l - load", "s - save", "r - run", "t - step", 
        "i - reset", "F5 - accumulator", "F6 - instructionCounter"};
    for (int i = 0; i < 7; i++) {
        mt_gotoXY(52, 14 + i);
        write(1, tmp1[i], strlen(tmp1[i]));
    }
}

void interface()
{
    enum keys key;
    int exit = 0;
    int x = 0, y = 0;
    while (!exit) {
        print_mem(&x, &y);
        print_accum();
        print_instcnt();
        print_operation();
        print_flg();
        print_membc(x, y);
        print_keys();
        rk_readkey(&key);
        if (key == q)
            exit++;
        if (key == l)
            sc_memoryLoad("mem");
        if (key == s)
            sc_memorySave("mem");
        if (key == up)
            x--;
        if (key == down)
            x++;
        if (key == left)
            y--;
        if (key == right)
            y++;
    }
}

int main()
{
    int fd = open("font", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int cnt = 0;
    alph = (int*) malloc(sizeof(int) * 2 * 17);
    if (bc_bigcharread(fd, alph, 17, &cnt) == -1)
        return -1;
    sc_memoryInit();
    mt_clrscr();
    int value;
    interface();
    write(1, "\e[0m", strlen("\e[0m"));
    rk_mytermrestore();
    mt_clrscr();
    return 0;
}
