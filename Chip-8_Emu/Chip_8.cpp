#define _CRT_SECURE_NO_WARNINGS
#include "Chip_8.h"

// Initialize interpreter to starting values
// Load fontset into memory
void Chip_8::initialize() {

	I = 0;
	delayTimer = 0;
	soundTimer = 0;
	PC = 0x200;
	SP = 0;
	opcode = 0;

	std::fill(std::begin(memory), std::end(memory), 0);
	std::fill(std::begin(V), std::end(V), 0);
	std::fill(std::begin(stack), std::end(stack), 0);

	// Chip-8 font set
	unsigned char fontSet[80] = {
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

	// Load fontSet into memory
	for (unsigned short i = 0; i < sizeof(fontSet); ++i) memory[i] = fontSet[i];

}

// Read rom binary and add to memory
void Chip_8::loadRom() {

	FILE* file = fopen("../ROMs/Test/1-chip8-logo.ch8", "rb");
	

	if (!file) {

		std::cout << "Rom failed to load.\n";
		exit(1);
	
	}

	// load 1 byte per index of loader
	fread(loader, 1, sizeof(loader), file);

	// Rom is added at memory location 0x200 (512)
	for (unsigned short i = 0; i < sizeof(loader); ++i) {

		memory[i + 512] = loader[i];

	}

	std::cout << "Loading complete!\n\n";

	exit(0);

}
