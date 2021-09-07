// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SDL.h>
#include "chip8.h"

chip8 cpu;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Event event;
int setupGraphics();

int main(int argc, char** argv)
{
	setupGraphics();
	//setupInput();

	cpu.init();
	cpu.load("ibm.ch8");
	//cpu.printMemory();
	bool runprogram = true;
	while(runprogram)
	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			cpu.emulateCycle();

			if (cpu.drawFlag) {
				int pixels[64 * 32];
				for (int i = 0; i < 64 * 32; i++) {
					if (cpu.gfx[i] != 0) {
						pixels[i] = 0xFFFF0000;
					}
					else {
						pixels[i] = 0xFF000000;
					}
				}
				
				SDL_UpdateTexture(texture, NULL, pixels, 256);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
				//return 0;
				//drawGraphics();
			}

			switch (evt.type)
			{
			case SDL_QUIT: runprogram = false;  break;
				/* process other events you want to handle here */
			}
		}

		
	}
	return 0;
}
int setupGraphics() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if (SDL_CreateWindowAndRenderer(256, 128, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ABGR32,
		SDL_TEXTUREACCESS_STREAMING,
		64,
		32);
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
