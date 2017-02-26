#include "term.h"

int main()
{
    mt_clrscr();
    mt_gotoXY(20, 20);
    printf("MDA\n");
    int rows, cols;
    mt_getscreensize(&rows, &cols);
    printf("%dx%d\n", rows, cols);
    return 0;
}
