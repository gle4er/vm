#include "mt.h"
#include <stdio.h>

int main()
{
    mt_clrscr();
    int rows, cols;
    mt_getscreensize(&rows, &cols);
    cols /= 2;
    cols -= 10;
    mt_gotoXY(cols, 0);
    printf("Valerie Funtikoffa\n");
    return 0;
}
