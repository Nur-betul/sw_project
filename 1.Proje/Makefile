CC = gcc
CFLAGS = -Wall -std=c99 -I. -IUnity
TARGET = main
TEST_TARGET = test_mcu
OBJ = main.o variables.o
TEST_OBJ = test/test_mcu.o Unity/unity.o variables.o

# all: $(TARGET) $(TEST_TARGET)
all: $(TARGET)

# $(TARGET): main.o variables.o Unity/unity.o
# 	$(CC) $(CFLAGS) -o $(TARGET) main.o variables.o Unity/unity.o

$(TARGET): main.o variables.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o variables.o 

# $(TEST_TARGET): $(TEST_OBJ)
# 	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ)

# main.o: main.c main.h variables.h
main.o: main.c variables.h
	$(CC) $(CFLAGS) -c main.c -o main.o

variables.o: variables.c variables.h 
	$(CC) $(CFLAGS) -c variables.c -o variables.o

# test/test_mcu.o: test/test_mcu.c variables.h main.h
# 	$(CC) $(CFLAGS) -c test/test_mcu.c -o test/test_mcu.o

# Unity/unity.o: Unity/unity.c Unity/unity.h Unity/unity_internals.h
# 	$(CC) $(CFLAGS) -c Unity/unity.c -o Unity/unity.o

# clean:
# 	rm -f *.o main test_mcu test/*.o Unity/*.o
clean:
	rm -f *.o $(TARGET)
