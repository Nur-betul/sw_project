CC = gcc
CFLAGS = -Wall -std=c99 -g -I.
TARGET = main
OBJ = main.o variables.o

all: $(TARGET)

$(TARGET): $(OBJ) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) 

main.o: main.c main.h variables.c
	$(CC) $(CFLAGS) -c main.c -o main.o

variables.o: variables.c variables.h
	$(CC) $(CFLAGS) -c variables.c -o variables.o

clean:
	rm -f *.o $(TARGET)
