#include "term.h"

int mt_clrscr()
{
    printf("\E[H\E[J");
    return 0;
}

int mt_gotoXY(int x, int y)
{
    printf("\E[0%d;%dH", x, y);
    return 0;
}

int mt_getscreensize(int *rows, int *cols)
{
    struct winsize {
        unsigned short ws_row;
        unsigned short ws_cols;
        unsigned short ws_xpixel;
        unsigned short ws_ypixel;
    } ws;
    if (!ioctl(1, TIOCGWINSZ, &ws)) {
        *rows = ws.ws_row;
        *cols = ws.ws_cols;
        return 0;
    } else {
        fprintf(stderr, "Error getting size\n");
        return -1;
    }
}

int mt_setfgcolor(enum colors colors)
{
    int color;
    switch(colors) {
        case black: 
            color = 0; 
        break;
        case red: 
            color = 1; 
        break;
        case green:
            color = 2;
        break;
        case yellow:
            color = 3;
        break;
        case blue:
            color = 4;
        break;
        case purple: 
            color = 5;
        break;
        case cyan:
            color = 6;
        break;
        case white:
            color = 7;
        break;
        default:
            return -1;
    }
    printf("\E[3%dm", color);
    return 0;
}

int mt_setbgcolor(enum colors colors)
{
    int color;
    switch(colors) {
        case black: 
            color = 0; 
        break;
        case red: 
            color = 1; 
        break;
        case green:
            color = 2;
        break;
        case yellow:
            color = 3;
        break;
        case blue:
            color = 4;
        break;
        case purple: 
            color = 5;
        break;
        case cyan:
            color = 6;
        break;
        case white:
            color = 7;
        break;
        default:
            return -1;
    }
    printf("\E[4%dm", color);
    return 0;
}

