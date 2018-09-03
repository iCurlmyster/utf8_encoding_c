CC=gcc
CFLAGS=-c -Wall
TARGET=utf8

OBJECTS=main.o utf8.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(DEBUG_INFO)

main.o: main.c
	$(CC) $(CFLAGS) main.c

utf8.o: utf8.c
	$(CC) $(CFLAGS) utf8.c

clean:
	rm *.o $(TARGET)