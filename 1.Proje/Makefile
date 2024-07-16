CC = gcc
CFLAGS = -Wall -std=c99 -I. -Iunity
TARGET = main
TEST_TARGET = test_mcu
TEST_OBJ = test/test_mcu.o unity/unity.o

all: $(TARGET)

$(TARGET): main.o variables.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o variables.o

main.o: main.c main.h variables.h
	$(CC) $(CFLAGS) -c main.c

variables.o: variables.c variables.h
	$(CC) $(CFLAGS) -c variables.c

$(TEST_TARGET): $(TEST_OBJ) main.o variables.o
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) main.o variables.o

test/test_mcu.o: test/test_mcu.c variables.h main.h
	$(CC) $(CFLAGS) -c test/test_mcu.c -o test/test_mcu.o

unity/unity.o: unity/unity.c unity/unity.h
	$(CC) $(CFLAGS) -c unity/unity.c -o unity/unity.o

clean:
	rm -f *.o $(TARGET) $(TEST_TARGET) test/*.o unity/*.o



# test/test_mcu.o: test/test_mcu.c main.h
# 	$(CC) $(CFLAGS) -c test/test_mcu.c -o test/test_mcu.o

