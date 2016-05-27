CC=g++
LDFLAGS=-lpthread -lncurses
FLAGS=-std=c++11 -Wall
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=fryzjer

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)
