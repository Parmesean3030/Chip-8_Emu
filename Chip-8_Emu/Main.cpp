#include <iostream>
#include "Memory.h"
#include "Chip_8.h"
#include "SDL.h"

int main(int argc, char* argv[])
{

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Unable to initialize SDL: " << SDL_GetError();
		return 1;
	}


	SDL_Window* window = SDL_CreateWindow("Chip - 8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64, 32, 0);

	if (!window) {
		std::cout << "There was an error creating the window\n\n";
		return 1;
	}


	Chip_8 chip8;
	chip8.initialize();
	chip8.loadRom("../ROMs/Test/1-chip8-logo.ch8");
	
	for (int i = 0; i < 10; ++i) chip8.emulationCycle();

	//system("pause");

	SDL_Quit();

	return 0;
}

/*int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Unable to initialize SDL: " << SDL_GetError();
		return 1;
	}

	SDL_Quit();
	return 0;
}*/