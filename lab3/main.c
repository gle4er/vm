#include "bc.h"
#include "mt.h"

int main()
{
    mt_clrscr();
    bc_box(0, 0, 10, 10);
    mt_gotoXY(1, 1);
    return 0;
}
