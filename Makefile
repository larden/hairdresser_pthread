CC=g++
LDFLAGS=-lpthread
FLAGS=-Wall
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=fryzjer

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)
