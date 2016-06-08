#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>
#include <condition_variable>
#include <unistd.h>
#include <ncurses.h>


#define QUEUE_LIMIT 18

#define F_X	5
#define F_Y	10
#define B_X	15
#define B_Y	10

#define SCI_X	9
#define SCI_Y	11
#define COMB_X	11
#define COMB_Y	11
#define KASA_X	20
#define KASA_Y	5

struct Client {
	int gold;

	Client(int i) : gold(i) {
		////std::cout << "Inicjalizacja z 5\n\r";
	}
};

template <typename T>
class Queue {
public:
	bool pop(T& klient) {
		std::unique_lock<std::mutex> m(_m);
		if (_cv.wait_for(m, std::chrono::seconds(1), [this](){return !_q.empty();})) {
			//std::cout << "Klient BEFORE POP: " << klient.gold << "\n\r";
			klient = _q.front();
			//std::cout << "Klient AFTER POP: " << klient.gold << "\n\r";
			_q.pop();
			return true;
		} else
			return false;
	}

	void push(const T& klient) {
		std::unique_lock<std::mutex> m(_m);
		//std::cout << "Klient BEFORE PUSH: " << klient.gold << "\n\r";
		_q.push(klient);
		m.unlock();
		_cv.notify_one();
	}

	void push(T&& klient) {
		std::unique_lock<std::mutex> m(_m);
		_q.push(std::move(klient));
		m.unlock();
		_cv.notify_one();
	}

	int size() {
		return _q.size();
	}

	Queue() = default;
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;
private:
	std::mutex _m;
	std::queue<T> _q;
	std::condition_variable _cv;
};

struct Scissors {
	int y;
	int x;
	std::mutex m;

	Scissors(int posy, int posx) : y(posy), x(posx) {}
};

struct Comb {
	int y;
	int x;
	std::mutex m;

	Comb(int posy, int posx) : y(posy), x(posx) {}
};

struct Kasa {
	int y;
	int x;
	int value;
	std::mutex m;

	Kasa(int posy, int posx) : y(posy), x(posx), value(0) {}
};

class Fryzjer {
public:
        Fryzjer(int y, int x, Queue<Client> &queue, Scissors &scissors, Comb &comb, Kasa &kasa)
		: the_thread(), _y(y), _x(x), _queue(queue), _sci(scissors), _comb(comb), _kasa(kasa), client(0) {
	}
        ~Fryzjer() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Fryzjer object\n";
        }

        void Start() {
                the_thread = std::thread(&Fryzjer::Action, this);
        }
	void Stop() {
		stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Fryzjer object\n";
	}

	int getX() { return _x; }
	int getY() { return _y; }
	bool has_client() { return _has_client; }

private:
        std::thread the_thread;
        int _y;
	int _x;
	Queue<Client> &_queue;
	Scissors &_sci;
	Comb &_comb;
	Kasa &_kasa;

	Client client;

	bool _has_client = false;

        bool stop_thread = false;
        void Action() {
                while (!stop_thread) {
                        //std::cout << "["<< _i << "] Czekam na klienta ...\n\r";

			_has_client = _queue.pop(client);
			if (!_has_client) continue;

			std::this_thread::sleep_for(std::chrono::seconds(1));

			//strzyzenie nozyczkami i grzebieniem
			{
				std::unique_lock<std::mutex> l_sci {_sci.m, std::defer_lock};
				std::unique_lock<std::mutex> l_comb {_comb.m, std::defer_lock};

				std::lock(l_sci, l_comb);
				_sci.x = _x - 1;
				_sci.y = _y;
				_comb.x = _x + 1;
				_comb.y = _y;
				std::this_thread::sleep_for(std::chrono::seconds(6));
				_sci.x = SCI_X;
				_sci.y = SCI_Y;
				_comb.x = COMB_X;
				_comb.y = COMB_Y;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));

			//czesanie grzebieniem
			{
				std::unique_lock<std::mutex> l_comb {_comb.m};

				_comb.x = _x + 1;
				_comb.y = _y;
				std::this_thread::sleep_for(std::chrono::seconds(5));
				_comb.x = COMB_X;
				_comb.y = COMB_Y;
			}

			//idziemy do kasy
			{
				std::unique_lock<std::mutex> l_kasa {_kasa.m};

				_y = _kasa.y+1;
				_x = _kasa.x-1;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_kasa.value += 50;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_has_client = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_y = F_Y;
				_x = F_X;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
                }
        }
};

class Barber {
public:
        Barber(int y, int x, Queue<Client> &queue, Scissors &scissors, Comb &comb, Kasa &kasa)
		: the_thread(), _y(y), _x(x), _queue(queue), _sci(scissors), _comb(comb), _kasa(kasa), client(0) {}
        ~Barber() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Barber object\n";
        }

        void Start() {
                the_thread = std::thread(&Barber::Action, this);
        }
	void Stop() {
		stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Barber object\n";
	}

	int getX() { return _x; }
	int getY() { return _y; }
	bool has_client() { return _has_client; }

private:
	std::thread the_thread;
        int _y;
	int _x;
	Queue<Client> &_queue;
	Scissors &_sci;
	Comb &_comb;
	Kasa &_kasa;

	Client client;

	bool _has_client = false;

        bool stop_thread = false;
        void Action() {
                while (!stop_thread) {

                        //std::cout << "["<< _i << "] Czekam na klienta ...\n\r";
			_has_client = _queue.pop(client);
			if(!_has_client) continue;

			std::this_thread::sleep_for(std::chrono::seconds(1));

			//scinanie nozyczkami
			{
				std::unique_lock<std::mutex> l_sci {_sci.m};

				_sci.x = _x + 1;
				_sci.y = _y;
				std::this_thread::sleep_for(std::chrono::seconds(4));
				_sci.x = SCI_X;
				_sci.y = SCI_Y;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			//czesanie grzebieniem
			{
				std::unique_lock<std::mutex> l_comb {_comb.m};

				_comb.x = _x + 1;
				_comb.y = _y;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_comb.x = COMB_X;
				_comb.y = COMB_Y;
			}

			//idziemy do kasy
			{
				std::unique_lock<std::mutex> l_kasa {_kasa.m};

				_y = _kasa.y+1;
				_x = _kasa.x-1;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_kasa.value += 100;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_has_client = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_y = B_Y;
				_x = B_X;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
                }
        }
};

class Map {
public:
        Map(Queue<Client> &queue, Fryzjer &f, Barber &b,Scissors &scissors, Comb &comb, Kasa &kasa)
		: the_thread(), _queue(queue), _f(f), _b(b), _sci(scissors), _comb(comb), _kasa(kasa) {}
        ~Map() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Barber object\n";
        }

        void Start() {
                the_thread = std::thread(&Map::Draw, this);
        }
	void Stop() {
		stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                //std::cout << "End of Barber object\n";
	}

private:
	std::thread the_thread;
	Queue<Client> &_queue;
	Fryzjer &_f;
	Barber &_b;
	Scissors &_sci;
	Comb &_comb;
	Kasa &_kasa;

	const char *s_klient = "K";
	const char *s_puste = "_";
	const char *s_kasa = "$";
	const char *s_fryzjer = "F";
	const char *s_barber = "B";
	const char *s_fotel = "|";
	const char *s_sci = "X";
	const char *s_comb = "E";

        bool stop_thread = false;
        void Draw() {
                while (!stop_thread) {

			clear();
			start_color();
			init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
			init_pair(2, COLOR_WHITE, COLOR_BLACK);
			init_pair(3, COLOR_YELLOW, COLOR_BLACK);
			init_pair(4, COLOR_RED, COLOR_BLACK);
			init_pair(5, COLOR_BLUE, COLOR_BLACK);
			init_pair(6, COLOR_GREEN, COLOR_BLACK);


			//Wyswietlenie kolejki
			mvprintw(0, 0, "%s", "SYMULACJA ZAKLADU FRYZJERSKIEGO");
			mvprintw(1, 0, "%s", "Autor: Jakub Lewalski 195419");
			mvprintw(3, 33, "%s", "Legenda:");
			mvprintw(4, 33, "%s", "F - Fryzjer");
			mvprintw(5, 33, "%s", "B - Barber");
			mvprintw(6, 33, "%s", "K - Klient");
			mvprintw(7, 33, "%s", "$ - Kasa");
			mvprintw(8, 33, "%s", "X - Nozyczki");
			mvprintw(9, 33, "%s", "E - Grzebien");

			for (int i = QUEUE_LIMIT; i > 0; i--) {
				attron(COLOR_PAIR(4));
				if ((QUEUE_LIMIT - i) < _queue.size())
					mvprintw(2, i, s_klient);
				else
					mvprintw(2, i, s_puste);
				attroff(COLOR_PAIR(4));
			}

			/*******************************/
			//Stanowisko Fryzjerskie - Fryzjer
			attron(COLOR_PAIR(3));
			mvprintw(_f.getY(), _f.getX(), s_fryzjer);
			attroff(COLOR_PAIR(3));

			//lewa strona fotela
			attron(COLOR_PAIR(1));
			mvprintw(F_Y-1, F_X-1, s_fotel);
			attroff(COLOR_PAIR(1));

			//klient
			if (_f.has_client()) {
				attron(COLOR_PAIR(4));
				mvprintw(_f.getY()-1, _f.getX(), s_klient);
				attroff(COLOR_PAIR(4));
			} else {
				attron(COLOR_PAIR(1));
				mvprintw(_f.getY()-1, _f.getX(), s_puste);
				attroff(COLOR_PAIR(1));
			}

			//prawa strona fotela
			attron(COLOR_PAIR(1));
			mvprintw(F_Y-1, F_X+1, s_fotel);
			attroff(COLOR_PAIR(1));
			/*******************************/

			/*******************************/
			// Barber
			attron(COLOR_PAIR(3));
			mvprintw(_b.getY(), _b.getX(), s_barber);
			attroff(COLOR_PAIR(3));

			//lewa strona fotela
			attron(COLOR_PAIR(1));
			mvprintw(B_Y-1, B_X-1, s_fotel);
			attroff(COLOR_PAIR(1));

			//klient
			if (_b.has_client()) {
				attron(COLOR_PAIR(4));
				mvprintw(_b.getY()-1, _b.getX(), s_klient);
				attroff(COLOR_PAIR(4));
			} else {
				attron(COLOR_PAIR(1));
				mvprintw(_b.getY()-1, _b.getX(), s_puste);
				attroff(COLOR_PAIR(1));
			}

			//prawa strona fotela
			attron(COLOR_PAIR(1));
			mvprintw(B_Y-1, B_X+1, s_fotel);
			attroff(COLOR_PAIR(1));
			/*****************************/

			//nozyczki i grzebien
			attron(COLOR_PAIR(5));
			mvprintw(_sci.y, _sci.x, s_sci);
			mvprintw(_comb.y, _comb.x, s_comb);
			attroff(COLOR_PAIR(5));

			attron(COLOR_PAIR(6));
			mvprintw(_kasa.y, _kasa.x, s_kasa);
			mvprintw(_kasa.y, _kasa.x+2, "%d$", _kasa.value);
			attroff(COLOR_PAIR(6));


			refresh();
			std::this_thread::sleep_for( std::chrono::milliseconds(50) );
                }
        }
};

Queue<Client> kolejka;
Scissors sci(11,9);
Comb comb(11,11);
Kasa kasa(5, 20);

Fryzjer v(F_Y, F_X, kolejka, sci, comb, kasa);
Barber  b(B_Y, B_X, kolejka, sci, comb, kasa);
Map *map;

void simulation_loop(char c)
{
        int in;

        //Exit if user wish
        if (c == 'q' || c == 'Q') return;

	v.Start();
	b.Start();

	map = new Map(kolejka, v, b, sci, comb, kasa);
	map->Start();
	int klient_id = 1;
        //Endless loop
        while (1) {
                in = getch();

                switch (in) {
		case 'k':
			kolejka.push(Client(klient_id++));
			break;
                case 'q':
                case 'Q':
			v.Stop();
			b.Stop();
			map->Stop();
                        return;
                default:
                        break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        printw("q - wyjscie z programu\n");

        auto a = getch(); //wait for character

        //Clear window before simulation
        clear();

        simulation_loop(a);

        endwin(); //End ncurses

        //std::cout << "Main thread\n";
        //std::this_thread::sleep_for(std::chrono::seconds(10));

        return 0;
}
