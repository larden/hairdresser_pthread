#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ncurses.h>

using namespace std;

void fryzjer()
{}

void klient()
{}

void run_klient()
{}

void simulation_loop(char c)
{
    int in;

    int x = 10, y = 10;

    //Exit if user wish
    if (c == 'q' || c == 'Q') return;
    
    //Endless loop
    while (1) {
        in = getch();
        
        switch (in) {
            case KEY_LEFT:
                mvaddch(x, y, '@');
                x++; y++;
                refresh();
                break;
            case 'q':
            case 'Q':
                return;
            default:
                break;
        }
    }

}
 
int main()
{
    /* Init ncurses */
    initscr(); 
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    printw("Symulacja pracy zakladu fryzjerskiego v.01\nAutor: Jakub Lewalski\n\n\n");
    printw("Akcje dostepne podczas symulacji:\n");
    printw("k - dodanie nowego klienta\n");
    printw("f - dodanie fryzjera (max 3)\n");
    printw("q - wyjscie z programu\n");

    char a = getch(); //wait for character

    //Clear window before simulation
    clear();

    simulation_loop(a);

    endwin(); //End ncurses

    return 0;
}
