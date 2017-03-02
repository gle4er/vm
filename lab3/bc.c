#include "bc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int bc_printA(char *str)
{
    write(1, "\E(0", 3);
    write(1, str, sizeof(char) * strlen(str));
    write(1, "\E(B", 3);
    return 0;
}

int bc_box(int x1, int y1, int x2, int y2)
{
    for (int i = y1; i < y2; i++)
        for (int j = x1; j < x2; j++) {
            if (i == y1 && j == x1)
                bc_printA("l");
            else if (i == y1 && j == x2 - 1)
                bc_printA("k\n");
            else if (i == y2 - 1 && j == x1)
                bc_printA("m");
            else if (i == y2 - 1 && j == x2 - 1)
                bc_printA("j");
            else if ((i == y1 || i == y2 - 1) && j > x1 && j < x2 - 1)
                bc_printA("q");
            else if (i > y1 && i < y2 - 1 && j == x1)
                bc_printA("x");
            else if (i > y1 && i < y2 - 1 && j == x2 - 1)
                bc_printA("x\n");
            else
                write(1, " ", sizeof(char));
        }
}

int bc_printbigchar(int *big, int x, int y, enum colors fgcolor, enum colors bgcolor)
{
    mt_gotoXY(x, y);
    mt_setfgcolor(fgcolor);
    mt_setbgcolor(bgcolor);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int value;
            bc_getbigcharpos(big, j, i, &value);
            if (!value)
                write(1, " ", 1);
            else
                bc_printA("a");
        }
        write(1, "\n", 1);
    }
    mt_setbgcolor(black);
    mt_setfgcolor(white);
    return 0;
}

int bc_setbigcharpos(int *big, int x, int y, int value)
{
    if (!(8 > x > 0) || !(8 > y > 0) || value != 0 || value != 1)
        return -1;
    int pos = (y > 3) ? 1 : 0;
    y %= 4;
    if (!value)
        big[pos] &= ~(1 << 32 - (y * 8 + x));
    if (value)
        big[pos] |= 1 << 32 - (y * 8 + x);
    return 0;
}

int bc_getbigcharpos(int *big, int x, int y, int *value)
{
    if (!(8 > x > 0) || !(8 > y > 0))
        return -1;
    int pos = (y > 3) ? 1 : 0;
    y %= 4;
    if (big[pos] & (1 << 32 - (y * 8 + x)))
        *value = 1;
    else
        *value = 0;
    return 0;
}

int bc_bigcharwrite(int fd, int *big, int count)
{
    for (int i = 0; i < count * 2; i++)
        if (write(fd, &big[i], sizeof(int) * 2) == -1)
            return -1;
    return 0;
}

int bc_bigcharread(int fd, int *big, int need_count, int *count)
{
    for (*count = 0; (*count <= need_count * 2); *count += 1) 
        if (read(fd, &big[*count], sizeof(int) * 2) == -1)
            return -1;
    return 0;
}

