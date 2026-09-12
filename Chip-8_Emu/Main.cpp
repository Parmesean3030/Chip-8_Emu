#include <iostream>
#include "Memory.h"
#include "Chip_8.h"
#include "SDL.h"

int main(int argc, char* argv[])
{
	bool loop = true;
	int counter = 0;
	unsigned int pixels[64 * 32];
	Chip_8 chip8;

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Unable to initialize SDL: " << SDL_GetError();
		return 1;
	}

	// Create SDL window
	SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);

	if (!window) {
		std::cout << "There was an error creating the window\n\n";
		system("exit");
	}


	SDL_Event event;
	SDL_Renderer* rend = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	chip8.initialize();
	chip8.loadRom("../ROMs/Test/6-keypad.ch8");

	while (loop) {

		SDL_PollEvent(&event);

		// Exit when ESC is pressed
		if (event.key.keysym.sym == SDLK_ESCAPE) SDL_Quit();

		chip8.emulationCycle();

		chip8.handleInput(event);

		if (chip8.draw) {

			chip8.draw = false;

			for (unsigned short i = 0; i < sizeof(chip8.gfx); ++i) {

				// Move from gfx[] to pixels array
				// RBGA is 32 bits, 8 bits for each
				// Set RGB to blk or wht depending on gfx[i]
				// Always set alpha to 100%
				pixels[i] = (chip8.gfx[i] * 0x00FFFFFF) | 0xFF000000;
			

			}

			// Clear renderer
			// Update texture built from pixel array
			// Copy entire texture and update screen
			SDL_RenderClear(rend);
			SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
			SDL_RenderCopy(rend, texture, NULL, NULL);
			SDL_RenderPresent(rend);

		}

	}

	SDL_Quit();

	return 0;
}