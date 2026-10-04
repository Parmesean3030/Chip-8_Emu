#include "Chip_8.h"

int main(int argc, char* argv[])
{
	Chip_8 chip8;
	bool loop = true;
	unsigned int pixels[64 * 32];
	unsigned short CPF = 15; // cycles per frame
	const int frameDelay = 1000 / 60;
	int frameStart;
	int frameTime;

	chip8.initialize();
	chip8.startMenu();

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Unable to initialize SDL: " << SDL_GetError();
		return 1;
	}

	// Create SDL window
	SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);

	if (!window) {
		std::cout << "There was an error creating the window\n\n";
		return 1;
	}


	SDL_Event event;
	SDL_Renderer* rend = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	while (loop) {

		// Set starting frame value
		frameStart = SDL_GetTicks();

		// Check for input and handle
		while (SDL_PollEvent(&event)) {
			chip8.handleInput(event);
		}

		// Run CPU cycles for this specific frame
		for (int i = 0; i < CPF; ++i) {
			chip8.emulationCycle();
		}

		chip8.updateTimers();

		if (chip8.draw) {

			chip8.draw = false;

			for (unsigned short i = 0; i < sizeof(chip8.gfx); ++i) {

				// Move from gfx[] to pixels array
				// ARGB is 32 bits, each being 8 bits or 2 hex
				// Set RGB to blk or wht depending on gfx[i]
				// Always set alpha to 100%, so we OR to set alpha
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

		// Determine how much time has passed and if needed, delay
		// Helps maintain 60 FPS
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) { SDL_Delay(frameDelay - frameTime); }

	}

	return 0;
}