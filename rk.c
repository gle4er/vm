#include "rk.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int rk_readkey(enum keys *key)
{
    if (tcgetattr(1, &def))
        return -1;
    if (rk_mytermregime(0, 0, 1, 0, 1))
        return -1;
}

int rk_mytermsave()
{
    return tcsetattr(1, &custom);
}

int rk_mytermrestore()
{
    return tcsetattr(1, &def);
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint)
{
    if (!tcgetattr(1, &custom))
        return -1;
    if (regime)
        custom.c_lflag |= ICANON;
    else
        custom.c_lflag &= ~ICANON;
    if (echo)
        custom.c_lflag |= ECHO;
    else
        custom.c_lflag &= ~ECHO;
    if (sigint)
        custom.c_lflag |= ISIG;
    else
        custom.c_lflag &= ~ISIG;
    custom.c_cc[VTIME] = vtime;
    custom.c_cc[VMIN] = vmin;
    return rk_mytermsave();
}
