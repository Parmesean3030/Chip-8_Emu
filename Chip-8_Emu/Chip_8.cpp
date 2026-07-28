#define _CRT_SECURE_NO_WARNINGS
#include "Chip_8.h"

Chip_8::Chip_8() {}
Chip_8::~Chip_8() {}

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
	std::fill(std::begin(key), std::end(key), 0);

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
void Chip_8::loadRom(const char* filename) {

	// Open file as binary and start at the end to get the file size
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		std::cout << "Rom failed to load: " << filename << "\n";
		exit(1);
	}

	// Get file size, reset get pointer back to beginning
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Check if the ROM fits in memory
	if (size > (4096 - 512)) {
		std::cout << "Error: ROM file is too large to fit in memory.\n";
		exit(1);
	}

	// Read into memory array starting at 0x200
	// reinterpret_cast to read unsigned char*
	file.read(reinterpret_cast<char*>(&memory[0x200]), size);

	std::cout << "Loading complete!\n\n";
}

void Chip_8::emulationCycle() {

	// Fetch
	// Op code is 16-bit, memory is 8-bit, combine 2 8-bit opcodes to make 1 instruction
	opcode = memory[PC] << 8 | memory[PC + 1];

	// Decode
	switch(opcode & 0xF000) {

		// 0x00E0 opcode
		// FIX ME: Add nested switch to factor in 0x00EE op code
		case 0x0000:
			// clear screen
			std::cout << "0x00E0 working! \n";
			PC += 2;
			break;
		
		// load normal register with immediate value
		// 3rd hex value determines the V register
		// 1st & 2nd hex values are what to add to said V register
		case 0x6000:
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			// DELETE ME - This is for testing purposes
			std::cout << "Register V[" << (opcode & 0x0F00 >> 8) << "] has had the value " << (opcode & 0x00FF) << " added to it\n";
			PC += 2;
			break;

		// load index register with immediate value
		case 0xA000:
			I = opcode & 0x0FFF;
			std::cout << "Register I has been set to " << (opcode & 0x0FFF) << "\n";
			PC += 2;
			break;

		case 0xD000:
			// Draw sprite to screen (only aligned)
			std::cout << "0xD000 working! \n";
			PC += 2;
			break;

		default: 
			std::cout << "Opcode not recognized! \n";

	}

	// Execute

}
