// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"

chip8 cpu;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Event event;
int setupGraphics();
int keyPressed(SDL_Keycode key);
int keyReleased(SDL_Keycode key);

int main(int argc, char** argv)
{
	const int SPEED = 500; //Hz

	setupGraphics();
	// setupInput();

	cpu.init();
	cpu.load(argv[1]);
	//cpu.printMemory();
	bool runprogram = true;


	auto last_tick = SDL_GetTicks();

	while(runprogram)
	{
		auto frame_start = SDL_GetTicks();
		SDL_Event evt;
		SDL_PollEvent(&evt);
			
			cpu.emulateCycle();
			if (cpu.drawFlag) {
				//cpu.inputout();
				int pixels[64 * 32];
				for (int i = 0; i < 64 * 32; i++) {
					if (cpu.gfx[i] != 0) {
						pixels[i] = 0x00FFFFFF;
						
					}
					else {

						pixels[i] = 0x00000000;
						pixels[i] += i / 64 << 8;
						pixels[i] += (i / 64 << 8) << 8;
						pixels[i] += ((i / 64 << 8) << 8) << 8; //I HAVE NO IDEA


					}
				}
				
				SDL_UpdateTexture(texture, NULL, pixels, 256);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
				//return 0;
				//drawGraphics();
			}
			auto currtick = SDL_GetTicks();
			if (currtick - last_tick > 1000 / 60) { //this handles the delay and sound timers. Dunno if i'm doing this right >:)
				cpu.handleTimers();
				last_tick = currtick;
			}
			auto frame_end = SDL_GetTicks();
			if (frame_end - frame_start < 1000 / SPEED) { //If running faster than desired speed, add delay
				SDL_Delay(1000 / SPEED - (frame_end - frame_start)); //run at 500Hz by default
			}
			switch (evt.type)
			{
			case SDL_QUIT: { 
				runprogram = false;
				break; 
			}
			case SDL_KEYDOWN: {

				keyPressed(evt.key.keysym.sym);
				break;
			}
			case SDL_KEYUP: {
				keyReleased(evt.key.keysym.sym);
				break;
			}

			}

	}
	return 0;
}
int setupGraphics() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
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
	return 0;
}
SDL_Keycode keys[] = {
	SDLK_1,SDLK_2,SDLK_3,SDLK_4, // 1, 2, 3, C
	SDLK_q,SDLK_w,SDLK_e,SDLK_r, // 4, 5, 6, D
	SDLK_a,SDLK_s,SDLK_d,SDLK_f, // 7, 8, 9, E
	SDLK_z,SDLK_x,SDLK_c,SDLK_v, // A, 0, B, F
};

int keyPressed(SDL_Keycode key) {
	unsigned short keynum = 0xFF;
	for (int i = 0; i < 16; i++) {
		if (keys[i] == key) {
			keynum = i;
			break;
		}
	}
	if (keynum != 0xFF) {
		cpu.key[keynum] = 1;
	}
	return 0;

}
int keyReleased(SDL_Keycode key) {
	unsigned short keynum = 0xFF;
	for (int i = 0; i < 16; i++) {
		if (keys[i] == key) {
			keynum = i;
			break;
		}
	}
	if (keynum != 0xFF) {
		cpu.key[keynum] = 0;
	}
	return 0;
}
