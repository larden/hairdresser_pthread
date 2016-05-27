CC=g++
LDFLAGS=-lpthread -lncurses
FLAGS=-Wall
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=fryzjer

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)
