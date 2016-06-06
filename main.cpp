#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>
#include <condition_variable>
#include <ncurses.h>

#define QUEUE_

struct Client {
	int gold;

	Client(int i) : gold(i) {
		//std::cout << "Inicjalizacja z 5\n\r";
	}
};

template <typename T>
class Queue {
public:
	bool pop(T& klient) {
		std::unique_lock<std::mutex> m(_m);
		if (_cv.wait_for(m, std::chrono::seconds(1), [this](){return !_q.empty();})) {
			std::cout << "Klient BEFORE POP: " << klient.gold << "\n\r"; 
			klient = _q.front();
			std::cout << "Klient AFTER POP: " << klient.gold << "\n\r"; 
			_q.pop();
			return true;
		} else
			return false;
	}
	
	void push(const T& klient) {
		std::unique_lock<std::mutex> m(_m);
		std::cout << "Klient BEFORE PUSH: " << klient.gold << "\n\r"; 
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

	Queue() = default;
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;
private:
	std::mutex _m;
	std::queue<T> _q;
	std::condition_variable _cv;
};

struct Scissors {

};

class Fryzjer {
public:
        Fryzjer(int i, Queue<Client> &queue) : the_thread(), _i(i), _queue(queue), client(0) {
	}
        ~Fryzjer() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Fryzjer object\n";
        }

        void Start() {
                the_thread = std::thread(&Fryzjer::Action, this);
        }
	void Stop() {
		stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Fryzjer object\n";
	}

private:
        std::thread the_thread;
        int _i;
	Queue<Client> &_queue;
	
	Client client;

        bool stop_thread = false;
        void Action() {
                while (!stop_thread) {
                        std::cout << "["<< _i << "] Czekam na klienta ...\n\r";
			if(_queue.pop(client))
				std::cout << "[" << _i << "] FRYZJER Client has: " << client.gold << " gold\n\r";
			else
				std::cout << "["<< _i << "] FRYZJER Timeout \n\r";

                        std::this_thread::sleep_for( std::chrono::seconds(5) );
                }
        }
};

class Barber {
public:
        Barber(int i, Queue<Client> &queue) : the_thread(), _i(i), _queue(queue), client(0) {}
        ~Barber() {
                stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Barber object\n";
        }

        void Start() {
                the_thread = std::thread(&Barber::Action, this);
        }
	void Stop() {
		stop_thread = true;
                if (the_thread.joinable()) the_thread.join();
                std::cout << "End of Barber object\n";
	}

private:
	std::thread the_thread;
        int _i;
	Queue<Client> &_queue;

	Client client;
        
        bool stop_thread = false;
        void Action() {
                while (!stop_thread) {
                        std::cout << "["<< _i << "] Czekam na klienta ...\n\r";
			if(_queue.pop(client))
				std::cout << "[" << _i << "] BARBER: Client has: " << client.gold << " gold\n\r";
			else
				std::cout << "["<< _i << "] BARBER Timeout \n\r";
                        
			std::this_thread::sleep_for( std::chrono::seconds(4) );
                }
        }
};


Queue<Client> kolejka;

void simulation_loop(char c)
{
        int in;

        std::vector<std::unique_ptr<Fryzjer>> v;
	std::vector<std::unique_ptr<Barber>> b;

	//std::cout << "debug \n\r";
        //std::this_thread::sleep_for(std::chrono::seconds(2));
        int x = 10, y = 10;

        //Exit if user wish
        if (c == 'q' || c == 'Q') return;
    
        int i = 0;
	int klient_id = 1;
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
                        v.push_back(std::unique_ptr<Fryzjer>(new Fryzjer(i++, kolejka)));
                        v.back()->Start();
                        std::cout << "Fryzjer Wystartował: " << i-1 << "\n\r";
                        break;
                case 'b':
                        b.push_back(std::unique_ptr<Barber>(new Barber(i++, kolejka)));
                        b.back()->Start();
                        std::cout << "Barber wystartował: " << i-1 << "\n\r";
                        break;
		case 'k':
			kolejka.push(Client(klient_id++));
			std::cout << "Dodałem nowego klienta do kolejki" << "\n\r";
			break;
                case 'q':
                case 'Q':
			for(auto it = v.begin(); it != v.end(); it++)
				(*it)->Stop();
			std::cout << "Czekam na watki..." << "\n\r";
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
