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
unsigned short sp = 0;

unsigned char key[16];
bool drawFlag;
bool legacycpu;

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
	legacycpu = true;
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
		short tmp = (opcode & 0x0F00) >> 8;
		V[tmp] = opcode & 0x00FF;
		pc += 2;

		break;
	}
	case 0x7000: { // 7XNN: Add value to register at V[X]
		short tmp = (opcode & 0x0F00) >> 8;
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
	case 0x3000: {
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
			pc += 2;
		}
		pc += 2;
		break;

	}
	case 0x4000: {
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
			pc += 2;
		}
		pc += 2;
		break;

	}
	
	case 0x5000: {
		if (V[(opcode & 0x0F00) >> 8] == V[((opcode & 0x00F0) >> 4)]) {
			pc += 2;
		}
		pc += 2;
		break;

	}

	case 0x9000: {
		if (V[(opcode & 0x0F00) >> 8] != V[((opcode & 0x00F0) >> 4)]) {
			pc += 2;
		}
		pc += 2;
		break;

	}

	case 0x8000: {
		unsigned short x = (opcode & 0x0F00) >> 8;
		unsigned short y = (opcode & 0x00F0) >> 4;

		switch (opcode & 0x000F) {
			case 0x0000: {
				V[x] = V[y];
				break;
			}
			case 0x0001: {
				V[x] = V[x] | V[y];
				break;
			}
			case 0x0002: {
				V[x] = V[x] & V[y];
				break;
			}
			case 0x0003: {
				V[x] = V[x] ^ V[y];
				break;
			}
			case 0x0004: {
				V[x] = V[x] + V[y];
				break;
			}
			case 0x0005: {
				V[x] = V[x] - V[y];
				break;
			}
			case 0x0006: {
				short lest = V[x] >> 7;
				if (legacycpu) {
					V[x] = V[y];
				}
				V[x] = V[x] >> 1;
				V[0xF] = lest;
				break;
			}
			case 0x0007: {
				V[x] = V[y] - V[x];
				break;
			}
			case 0x000E: {
				short lest = V[x] >> 7;

				if (legacycpu) {
					V[x] = V[y];
				}
				V[x] = V[x] << 1;
				V[0xF] = lest;

				break;
			}





		}
		pc += 2;
		break;

	}
	case 0xB000: {
		pc = (opcode & 0x0FFF) + V[0];
		break;
	}
	case 0xC000: {
		unsigned short x = (opcode & 0x0F00) >> 8;
		V[x] = std::rand() & opcode & 0x00FF;
		pc += 2;
		break;
	}
	case 0xE000: { //TODO: IMPLEMENT INPUT
		unsigned short x = (opcode & 0x0F00) >> 8;
		switch (opcode & 0x00FF) {
			case 0x009E: {
				if (key[V[x]] == 1) {
					pc += 2;
				}
				break;
			}
			case 0x00A1: {
				if (key[V[x]] == 0) {
					pc += 2;
				}
				break;
			}
		}
		pc += 2;
		break;
	}
	case 0xF000: {
		unsigned short x = (opcode & 0x0F00) >> 8;
		switch (opcode & 0x00FF) {
		case 0x0007:
			V[x] = delay_timer;
			break;
		case 0x0015:
			delay_timer = V[x];
			break;
		case 0x0018:
			sound_timer = V[x];
			break;
		case 0x000A:
			//IMPLEMENT INPUT LATER
			for (int i = 0; i < 16; i++) {
				if (key[i] == 1) {
					pc += 2;
					break;
				}
			}

			break;
		case 0x001E:
			I += V[x];
			break;
		case 0x0029:
			I = V[x] * 5;
			break;
		case 0x0033: {
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			break;
		}
		case 0x0055: {
			for (int i = 0; i < x; i++) {
				memory[I + i] = V[i];
			}
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;
		}
		case 0x0056: {
			for (int i = 0; i < x; i++) {
				V[i] = memory[I + i];
			}
			break;

		}
		}

		pc += 2;
		break;
	}
	case 0x0000: {
		switch (opcode & 0x000F)
		{
		case 0x0000: { // 0x00E0: Clears the screen        
		  // Execute opcode
			for (int i = 0; i < 2048; ++i)
				gfx[i] = 0x0;
			drawFlag = true;
			pc += 2;

			break;
		}

		case 0x000E: { // 0x00EE: Returns from subroutine  

		  // Execute opcode
			--sp;
			pc = stack[sp];
			pc += 2;
			break;
		}

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			break;
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
void chip8::inputout() {
	for (int i = 0; i < sizeof(key); i++) {
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)key[i]);
	}
}