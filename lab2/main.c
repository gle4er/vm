#include "term.h"

int main()
{
    mt_clrscr();
    mt_gotoXY(20, 20);
    int rows, cols;
    mt_getscreensize(&rows, &cols);
    mt_setfgcolor(red);
    mt_setbgcolor(cyan);
    printf("%dx%d\n", rows, cols);
    mt_setfgcolor(white);
    mt_setbgcolor(black);
    return 0;
}
