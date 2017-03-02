#include "bc.h"
#include "mt.h"
#include <stdio.h>

int main()
{
    mt_clrscr();
    bc_box(0, 0, 10, 10);
    int big[2] = {1010991489, 1086365982};
    bc_printbigchar(big, 2, 2, red, cyan);
    return 0;
}
