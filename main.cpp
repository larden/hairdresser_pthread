#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ncurses.h>

#define TEXT_H 2
#define MAP_H 30
#define MAP_W 100

using namespace std;

struct MapField {
	string sign;
	string color;
};

struct Map {
	int x, y;

	MapField mapFields[MAP_H][MAP_W];

	void initMap() {
		for (int i = TEXT_H; i < MAP_H; i++) {
			for (int j = 0; j < MAP_W; j++) {
				mapFields[i][j].sign = " ";
			}
		}
	}

	void writeText(int x, int y, string text) {
		for (int i = x, j=0; j < text.length(); i++, j++)
			mapFields[y][i].sign = text.at(j);
	}

	Map() {}
	
	void start() {
		initMap();
		printMap();
	}

	void printMap() {
		clear();
		//start_color();
		//init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
		//init_pair(2, COLOR_WHITE, COLOR_BLACK);
		getmaxyx(stdscr, y, x);
		if (x >= MAP_W && y >= MAP_H) {
			for (int i=0; i<MAP_H; i++) {
				for (int j=0; j<MAP_W; j++) {
					mvprintw(i, j, mapFields[i][j].sign.c_str());
				}
			}
		} else {
			const char warning[] = "Warning! Window is too small!";
			mvprintw(y/2, (x/2) - (sizeof(warning)/2), warning);
		}
		refresh();
	}

	void refreshMap() {
		refresh();
	}
};

/****************************************************************************************************/
/****************************************************************************************************/

Map map;

void simulation_loop(char c)
{
	int in;


	//Exit if user wish
	if (c == 'q' || c == 'Q') return;

	map.start();

	//Endless loop
	while (1) {
		in = getch();

		switch (in) {
		case KEY_LEFT:
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
