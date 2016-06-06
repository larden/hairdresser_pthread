#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>
#include <condition_variable>
#include <ncurses.h>


class Fryzjer {
public:
        Fryzjer(int i) : the_thread(), _i(i) {}
        ~Fryzjer() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Fryzjer object\n";
        }

        void Start() {
                the_thread = std::thread(&Fryzjer::Action, this);
        }

private:
	Kolejka _queue;
	Client client;
        std::thread the_thread;
        bool stop_thread = false;
        int _i;
        void Action() {
                while (!stop_thread) {
                        std::cout << "Wysylamy jakies gowno ["<< _i << "]\n\r";
                        std::this_thread::sleep_for( std::chrono::seconds(1) );
                }
        }
};

class Barber {
public:
        Barber(int i) : the_thread(), _i(i) {}
        ~Barber() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Barber object\n";
        }

        void Start() {
                the_thread = std::thread(&Barber::Action, this);
        }

private:
	Kolejka _queue;
        std::thread the_thread;
        bool stop_thread = false;
        int _i;
        void Action() {
                while (!stop_thread) {
                        std::cout << "Barber: Wysylamy jakies gowno ["<< _i << "]\n\r";
                        std::this_thread::sleep_for( std::chrono::seconds(1) );
                }
        }
};

class Client {
public:
	Client(int i) : gold(i) {};
	int gold;
};

template <typename T>
class Queue {
public:
	T pop() {
		std::unique_lock<std::mutex> m(_m);
		while (_q.empty()) {
			_cv.wait(m);
		}
		auto klient = _q.front();
		_q.pop();
		return klient;
	}
	void pop(T& klient) {
		std::unique_lock<std::mutex> m(_m);
		while (_q.empty()) {
			_cv.wait(m);
		}
		klient = _q.front();
		_q.pop();
	}
	
	void push(const T& klient) {
		std::unique_lock<std::mutex> m(_m);
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
private:
	std::mutex _m;
	std::queue<T> _q;
	std::condition_variable _cv;
};

Queue<Client> kolejka;

void simulation_loop(char c)
{
        int in;

        std::vector<std::unique_ptr<Fryzjer>> v;

        int x = 10, y = 10;

        //Exit if user wish
        if (c == 'q' || c == 'Q') return;
    
        int i = 0;
        //Endless loop
        while (1) {
                in = getch();
        
                switch (in) {
                case KEY_LEFT:
                        mvaddch(x, y, '@');
                        x++; y++;
                        refresh();
                        break;
                case 'f':
                        v.push_back(std::unique_ptr<Fryzjer>(new Fryzjer(i++)));
                        v.back()->Start();
                        std::cout << "Wystartował: " << i-1 << "\n\r";
                        break;
		case 'k':
			kolejka.push(Klient(i));
			std::cout << "Dodałem nowego klienta do kolejki" << "\n\r";
			break;
                case 'q':
                case 'Q':
                        return;
                default:
                        break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

        auto a = getch(); //wait for character

        //Clear window before simulation
        clear();
        
        simulation_loop(a);

        endwin(); //End ncurses
        
        std::cout << "Main thread\n";
        //std::this_thread::sleep_for(std::chrono::seconds(10));

        return 0;
}
