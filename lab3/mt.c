#include "mt.h"

int mt_clrscr()
{
    write(1, "\E[H\E[J", sizeof(char) * strlen("\E[H\E[J"));
    return 0;
}

int mt_gotoXY(int x, int y)
{
    char buf[18] = "\E[0", x1[5] = {0} , y1[5] = {0};
    sprintf(x1, "%d", x);
    sprintf(y1, "%d", y);
    strcat(buf, x1);
    strcat(buf, ";");
    strcat(buf, y1);
    strcat(buf, "H");
    write(1, buf, 18);
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
        write(2, "Error getting size\n", strlen("Error getting size\n") * sizeof(char));
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
    char buf[8] = "\E[3", color1[2] = {0};
    sprintf(color1, "%d", color);
    strcat(buf, color1);
    strcat(buf, "m");
    write(1, buf, sizeof(buf));
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
    char buf[8] = "\E[4", color1[2] = {0};
    sprintf(color1, "%d", color);
    strcat(buf, color1);
    strcat(buf, "m");
    write(1, buf, sizeof(buf));
    return 0;
}

