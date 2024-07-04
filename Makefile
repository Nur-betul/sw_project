CC = gcc
CFLAGS = -Wall -std=c99
TARGET = main

all: $(TARGET)

$(TARGET): main.o variables.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o variables.o

main.o: main.c variables.h
	$(CC) $(CFLAGS) -c main.c

variables.o: variables.c variables.h
	$(CC) $(CFLAGS) -c variables.c

clean:
	rm -f *.o $(TARGET)

