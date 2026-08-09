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

	// Load fontSet into memory starting at location 0x50
	for (unsigned short i = 0; i < sizeof(fontSet); ++i) memory[0x50 + i] = fontSet[i];

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

	// Fetch: Opcode is 16-bit, memory is 8-bit, combine 2 8-bit memory instructions to make 1 Opcode
	opcode = memory[PC] << 8 | memory[PC + 1];

	// Decode & execute
	switch (opcode & 0xF000) {

		case 0x0000:

			switch (opcode & 0x000F) {

			// 0x00E0 opcode
			// Clear gfx[]/screen
			case 0x0000:
				for (unsigned short i = 0; i < (64 * 32); ++i) gfx[i] = 0;
				PC += 2;
				break;

			// 0x00EE: Return from subroutine
			case 0x000E:
				// FIX ME
				break;

			default:
				std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n\n";
				break;
			}

			break;

		// 0x1nnn: Jump to address
		// n, n and n added to program counter
		case 0x1000:
			PC = opcode & 0x0FFF;
			break;

		// 0x6xnn: load normal register with immediate value
		// x is the V register
		// n and n are placed into V[x] register
		case 0x6000:
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			PC += 2;
			break;

		// 0xAnnn: load index register with immediate value
		// n, n and n added to I
		case 0xA000:
			I = opcode & 0x0FFF;
			PC += 2;
			break;

		// 0xDxyn: Draw sprites to screen
		// 0xDxyn - x is V[x], y is V[y], n is the height of sprite
		// Modulo to wrap coordinates
		case 0xD000: {

			unsigned short xCoord = V[(opcode & 0x0F00) >> 8] % 64;
			unsigned short yCoord = V[(opcode & 0x00F0) >> 4] % 32;
			unsigned short height = opcode & 0x000F;
			unsigned short sprite;

			// Set collision register V[15]
			V[0xF] = 0;

			// Each sprite has a height of n pixels (row) and a width (col) of 8 pixels
			for (unsigned short row = 0; row < height; ++row) {

				sprite = memory[I + row];

				// Clip sprite and protect against gfx[] overflow
				if (yCoord + row >= 32) break;

				for (unsigned short col = 0; col < 8; ++col) {

					//  Clip sprite and protect against gfx[] overflow
					if (xCoord + col >= 64) break;

					// 0x80 = 10000000
					// Check each bit of the 8-bit sprite pulled from memory
					// If bit is 1, proceed, otherwise iterate to next bit
					if ((sprite & (0x80 >> col)) != 0) {

						// Since graphics buffer is a 1D array but has 2 coordinates (x, y)
						// we must "flatten" the array by using (x+(y*width) formula
						unsigned short location = xCoord + col + ((yCoord + row) * 64);

						// Check graphic buffer, if location already set to 1 set V[0xF] to 1
						if (gfx[location] == 1) {

							V[0xF] = 1;

						}

						// XOR location in flattened graphics buffer
						gfx[location] ^= 1;

					}
				}
			}

			PC += 2;
			break;
		}

		default: 
			std::cout << "Opcode not recognized! \n";
	}

	// Execute

}
