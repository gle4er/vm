#include "bc.h"
#include "mt.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main()
{
    int fd = open("font", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int cnt = 0;
    /*int alph[] = {
        2172748287, 4286677377,
        404232216, 404232216,
        4286611711, 4278255873,
        4286611711, 4286611584,
        4286677377, 2155905152,
        4278256127, 4286611584,
        4278255873, 4286677377,
        2155905279, 2155905152,
        4286677503, 4286677377,
        4286677503, 2155905152,
        4286677503, 2172748161,
        4280361279, 4286677377,
        16843263, 4278255873,
        2172731711, 1061257601,
        4278256127, 4278255873,
        4278256127, 16843009,
        4279769088, 1579263
    };*/
    int *alph = (int*) malloc(sizeof(int) * 2 * 17);
    if (bc_bigcharread(fd, alph, 17, &cnt))
        perror("mda\n");
    for (int i = 0; i < 17; i++) {
        int big[] = {alph[i * 2], alph[i * 2 + 1]};
        bc_printbigchar(big, 2, 2 + 10 * i, white, black);
    }
    return 0;
}
