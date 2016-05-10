#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ncurses.h>

using namespace std;

int main()
{
    initscr(); //Init ncurses
    
    printw("Witaj Świecie!");
    getch(); //wait for character

    endwin(); //End ncurses

    return 0;
}
