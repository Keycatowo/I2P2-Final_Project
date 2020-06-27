#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int mygetch()
{
    struct termios oldt,
        newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int get_arrow()
{
    int c;
    while (1)
    {
        c = mygetch();
        switch (c)
        {
        case 10:
            return 5;
        case 27:
            c = mygetch();
            if(c!=91)
                continue;
            c = mygetch();
            switch (c)
            {
            case 65:
                return 1;
            case 66:
                return 2;
            case 68:
                return 3;
            case 67:
                return 4;
            }
        }  
    }
    return 0;
}
#endif // KEYBOARD_H
