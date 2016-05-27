#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <conditional_variable>

#include <ncurses.h>

#define MAP_H 30
#define MAP_W 100

using namespace std;

struct MapField {
	string sign;
	string color;
};

struct Map {
	
	MapField mapFieldsp[MAP_H][MAP_W];

	void initMap() {
		for (int i = )
	}

	Map() {}	

	void printMap() {
		clear();
		//start_color();
		//init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
		//init_pair(2, COLOR_WHITE, COLOR_BLACK);
		getmaxyx(stdcsr, y, x);
		if ()
		
	}

	void refreshMap() {
		refresh();
	}
};
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
