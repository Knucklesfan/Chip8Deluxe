#pragma once
class chip8
{
	public:
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
		bool legacycpu;

		void init();
		void load(const char* str);
		void emulateCycle();
		void debugdraw();
		void inputout();
		void printMemory();

};

short getYCoord(short x, short y);
