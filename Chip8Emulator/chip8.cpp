#include "chip8.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <iterator>
#include <iomanip>

#include <iostream>
unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];

unsigned short I;
unsigned short pc;

unsigned char gfx[64 * 32];

unsigned char delay_timer;
unsigned char sound_timer;

unsigned short stack[16];
unsigned short sp;

unsigned char key[16];
bool drawFlag;

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8::init() {
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

}
void chip8::load(const char *str) {
	std::ifstream file(str, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);

	if (file.read(buffer.data(), size))
	{

		for (int i = 0; i < size; ++i)
			memory[i + 512] = buffer[i];
	}
	else {
		printf("Error reading file ");
	}

}
void chip8::emulateCycle() {
	//Fetch opcode

	opcode = memory[pc] << 8 | memory[pc + 1];
	switch (opcode & 0xF000) {
	case 0xA000: {// ANNN: Sets I to the address NNN
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	}
	case 0x2000: { // 2NNN: Runs subroutine at NNN
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;
	}
	case 0x1000: { // 1NNN: Jumps to position
		pc = opcode & 0x0FFF;
		break;
	}
	case 0x6000: { // 6XNN: Set register at V[X]
		short tmp = opcode & 0x0F00;
		V[tmp] = opcode & 0x00FF;
		pc += 2;

		break;
	}
	case 0x7000: { // 7XNN: Add value to register at V[X]
		short tmp = opcode & 0x0F00;
		V[tmp] += opcode & 0x00FF;
		pc += 2;

		break;
	}
	case 0xD000: { // DXYN: Draw
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;

		break;
	}
	case 0x0000: {
		switch (opcode & 0x000F)
		{
		case 0x0000: { // 0x00E0: Clears the screen        
		  // Execute opcode
			std::fill_n(gfx, 64 * 32, 0);
			pc += 2;

			break;
		}

		case 0x000E: { // 0x00EE: Returns from subroutine  

		  // Execute opcode
			pc += 2;
			break;
		}

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;
	}
	default:
		printf("Unknown opcode: 0x%X\n", opcode);

	}


	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;

	}
}
void chip8::printMemory() {
	for (int i = 0; i < sizeof(memory); i++) {
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)memory[i]);
		std::cout << " ";
		if (i % 16 == 0) {
			std::cout << "\n";
		}
	}
	std::cout << '\n';
}
short getYCoord(short x, short y) {
	return x + y * 32;
}
void chip8::debugdraw() {
	for (int i = 0; i < sizeof(gfx); i++) {
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)gfx[i]);
		if (i % 64 == 0) {
			std::cout << "\n";
		}
	}
}