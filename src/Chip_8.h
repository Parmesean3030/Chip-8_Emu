#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <limits>
#include "SDL.h"

class Chip_8
{
public:
	unsigned char gfx[64 * 32]; // gfx buffer
	bool draw;
	const char* rom;
	unsigned char key[16]; // Keypad

	
	void initialize();
	void startMenu();
	void loadRom(const char*);
	void emulationCycle(); // Emulate one cycle
	unsigned short fetch();
	void handleInput(SDL_Event& event); // keypad input
	void updateTimers();

	Chip_8();
	~Chip_8();

private:
	// memory is 4kb
	// V is 16, 8-bit special registers
	// stack is 16, 16-bit address storage
	// loader is 4kb - 512 for ROM location loading to memory
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned char delayTimer;
	unsigned char soundTimer;
	unsigned short PC;
	unsigned char SP;
	unsigned short stack[16];
	unsigned short opcode;
	unsigned char fontSet[80];
	unsigned char loader[3584];
	unsigned char randNum;
	bool waitKey = false;
	unsigned char pressedKey = 0;


};

