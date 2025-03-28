OBJS	= Chip8Emulator.o chip8.o
HEADER	=
OUT	= chip8
CC	 = g++
FLAGS	 = $(INC) -g -c -Wall -O0 -D _LINUX -DCLIENT
LFLAGS	 = -lSDL2

INC=-I./include/ -I/usr/include/freetype2

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
Chip8Emulator.o: source/Chip8Emulator.cpp
	$(CC) $(FLAGS) source/Chip8Emulator.cpp -std=c++17

chip8.o: source/chip8.cpp
	$(CC) $(FLAGS) source/chip8.cpp -std=c++17
clean:
	rm -f $(OBJS) $(OUT)
