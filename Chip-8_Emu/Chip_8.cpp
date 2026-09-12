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
		std::cout << "Rom failed to load: " << filename << "\n\nClosing Chip-8 Emulator...";
		exit(1);
	}

	// Get file size, reset get pointer back to beginning
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Check if the ROM fits in memory
	if (size > (4096 - 512)) {
		std::cout << "Error: ROM file is too large to fit in memory.\n\nClosing Chip-8 Emulator...";
		exit(1);
	}

	std::cout << "Loading ROM into memory...\n\n";

	// Read into memory array starting at memory location 0x200
	// reinterpret_cast to read unsigned char*
	file.read(reinterpret_cast<char*>(&memory[PC]), size);

	std::cout << "Loading complete!\n\n";
}

unsigned short Chip_8::fetch() {

	// Fetch: Opcode is 16-bit, must combine 2 instructions from memory
	opcode = memory[PC] << 8 | memory[PC + 1];
	PC += 2;

	return opcode;

}

void Chip_8::emulationCycle() {

	// Fetch: Opcode is 16-bit, must combine 2 instructions from memory
	//opcode = memory[PC] << 8 | memory[PC + 1];

	fetch();

	// Decode & execute
	switch (opcode & 0xF000) {

		case 0x0000:

			switch (opcode & 0x000F) {

			// 0x00E0 opcode
			// Clear gfx[]/screen
			case 0x0000:
				for (unsigned short i = 0; i < (64 * 32); ++i) gfx[i] = 0;
				break;

			// 0x00EE: Return
			case 0x000E:
				
				PC = stack[SP];
				--SP;

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

		// 0x2nnn: Call
		// Increment SP, add current PC address to stack
		// Set PC to nnn
		case 0x2000:

			++SP;
			stack[SP] = PC;
			PC = opcode & 0x0FFF;

			break;

		// 0x3xnn: Skip
		// x is V[x] index
		// Increment PC if V[x] == nn
		case 0x3000:
			
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
				PC += 2;
			}

			break;

		// 0x4xnn: Skip
		// x is V[x] index
		// Increment PC if V[x] != nn
		case 0x4000:
			
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				PC += 2;
			}

			break;

		// 0x5xy0: Skip
		// x is V[x] index - y is V[y] index
		// Increment PC if V[x] == V[y]
		case 0x5000:

			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
				PC += 2;
			}

			break;

		// 0x6xnn: load normal register with immediate value
		// x is the V register
		// n and n are placed into V[x] register
		case 0x6000:
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			break;

		// 0x7xnn: Add
		// V[x] += nn
		case 0x7000:
			V[((opcode & 0x0F00) >> 8)] += opcode & 0x00FF;
			break;

		// 0x8xy(0-E)
		case 0x8000:

			switch (opcode & 0x000F) {

			// 0x8xy0: Set
			// x == V[x]  y == V[y]
			// Set V[x] to V[y]
			case 0x0000:

				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy1: Set OR
			// x == V[x] - y == V[y]
			// Set V[x] == V[x] | V[y]
			case 0x0001:

				V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy2: Set AND
			// x == V[x] - y == V[y]
			// Set V[x] == V[x] & V[y]
			case 0x0002:

				V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy3: Set XOR
			// x == V[x] - y == V[y]
			// Set V[x] == V[x] ^ V[y]
			case 0x0003:

				V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy4: Add
			// x == V[x] - y == V[y]
			// Set V[x] = V[x] + V[y]
			// If V[x] > 255 set V[F] = 1, else 0
			case 0x0004: {

				unsigned short sum;
				sum = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];

				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];

				if (sum > 0xFF) {
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}

				break;
			}

			// 0x8xy5: Subtract
			// x == V[x] - y == V[y]
			// Set V[x] = V[x] - V[y]
			// If V[x] > V[y] set V[F] = 1, else 0
			case 0x0005: {

				unsigned short x = V[(opcode & 0x0F00) >> 8];

				V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];

				if (x >= V[(opcode & 0x00F0) >> 4]) {
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}

				break;
			}

			// 0x8xy6: Shift-R
			// x == V[x]
			// Lease significant bit of V[x] sets V[0xF]
			// Set V[x] >>= 1
			// Modern approach being used, some older games may not work
			case 0x0006: {

				unsigned char flag = V[(opcode & 0x0F00) >> 8] & 0x1;

				V[(opcode & 0x0F00) >> 8] >>= 1;
				V[0xF] = flag;

				break;
			}

			// 0x8xy7: Subtract
			// x == V[x] - y == V[y]
			// Set V[x] = V[y] - V[x]
			// If V[y] > V[x] set V[F] = 1, else 0
			case 0x0007:

				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];

				if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
					V[0xF] = 0;
				}
				else
				{
					V[0xF] = 1;
				}

				break;

			// 0x8xyE: Shift-L
			// x == V[x]
			// Most significant bit of V[x] sets V[0xF]
			// Set V[x] <<= 1
			// Modern approach being used, some older games may not work
			case 0x000E: {

				unsigned char flag = V[(opcode & 0x0F00) >> 8] >> 7;

				V[(opcode & 0x0F00) >> 8] <<= 1;
				V[0xF] = flag;

				break;
			}

			default:
				std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n\n";
				break;

			}

			break;

		// 0x9xy0: Skip
		// x is V[x] index - y is V[y] index
		// Increment PC if V[x] != V[y]
		case 0x9000:
			
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
				PC += 2;
			}

			break;

		// 0xAnnn: load index register with immediate value
		// n, n and n added to I
		case 0xA000:
			I = opcode & 0x0FFF;
			break;

		case 0xB000:
			PC = ((opcode & 0x0FFF) + V[0]);
			break;

		case 0xC000: {
			unsigned char min = 0;
			unsigned char max = 255;

			srand(static_cast<unsigned int>(std::time(nullptr)));

			randNum = rand() % ((max - min) + 1) + min;
			V[(opcode & 0x0F00) >> 8] = randNum & (opcode & 0x00FF);

			break;
		}

		// 0xDxyn: Draw sprites to screen
		// 0xDxyn - x is V[x], y is V[y], n is the height of sprite
		// Modulo to wrap coordinates initially
		case 0xD000: {

			unsigned short xCoord = V[(opcode & 0x0F00) >> 8] % 64;
			unsigned short yCoord = V[(opcode & 0x00F0) >> 4] % 32;
			unsigned short height = opcode & 0x000F;
			unsigned short sprite;

			// Set collision register
			V[0xF] = 0;

			// Each sprite has a height of n pixels (row) and a width (col) of 8 pixels
			for (unsigned short row = 0; row < height; ++row) {

				sprite = memory[I + row];

				// Clip sprite and protect against gfx[] overflow
				if (yCoord + row >= 32) break;

				for (unsigned short col = 0; col < 8; ++col) {

					//  Clip sprite
					if (xCoord + col >= 64) break;

					// Since graphics buffer is a 1D array but has 2 coordinates (x, y)
					// we must "flatten" the array by using (x+(y*width) formula
					unsigned short location = ((xCoord + col) + ((yCoord + row) * 64));

					// 0x80 = 10000000
					// Check each bit of the 8-bit sprite pulled from memory
					// If bit is 1, proceed, otherwise iterate to next bit
					if ((sprite & (0x80 >> col)) != 0) {

						// Check graphic buffer, if location already set to 1 set collision register to 1
						if (gfx[location] == 1) {

							V[0xF] = 1;

						}

						// XOR location in flattened graphics buffer
						gfx[location] ^= 1;
					}
				}
			}

			draw = true;
			break;
		}

		case 0xE000:

			switch (opcode & 0x00FF) {

			case 0x009E:

				if (key[V[(opcode & 0x0F00) >> 8]] != 0) { PC += 2; }

				break;

			case 0x00A1:

				if (key[V[(opcode & 0x0F00) >> 8]] == 0) { PC += 2; }

				break;

			default:
				std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n\n";
				break;

			}


			break;
		
		case 0xF000:

			switch (opcode & 0x00FF) {

			case 0x0007:

				break;

			case 0x000A:

				for (unsigned char i = 0; i < 16; ++i) {

					if (key[i] != 0) { V[(opcode & 0x0F00) >> 8] = 1; }
					else { return; }

				}

				break;

			// 0xFx1E: Add
			// I = I + V[x]
			case 0x001E:

				I += V[(opcode & 0x0F00) >> 8];

				break;

			// 0xFx33: Load
			// Stores the Binary-coded decimal representation of V[x]
			// Store each digit in memory at I, I+1 and I+2
			case 0x0033:

				memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
				memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;

				break;

			// 0xFx55: Load
			// Load V[0] through V[x] to memory starting at I
			// Incremember I by x + 1
			case 0x0055:

				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
					memory[I + i] = V[i];
				}

				I += ((opcode & 0x0F00) >> 8) + 1;

				break;
			
			// 0xFx65: Load
			// Load memory starting at I to V[0] through V[x]
			// Incremember I by x + 1
			case 0x0065:

				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
					V[i] = memory[I + i];
				}

				I += ((opcode & 0x0F00) >> 8) + 1;

				break;

			default:
				std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n\n";
				break;

			}

		default: 
			std::cout << "Opcode not recognized! \n";
	}
}

void Chip_8::handleInput(SDL_Event& event) {
	// Ignore any events that are not key presses or releases
	if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
		return;
	}

	// Determine the state to apply (1 for pressed, 0 for released)
	unsigned char state = (event.type == SDL_KEYDOWN) ? 1 : 0;

	// Map the physical SDL keyboard key to the Chip-8 hex pad index
	switch (event.key.keysym.sym) {

		// Row 1
	case SDLK_1: key[0x1] = state; break;
	case SDLK_2: key[0x2] = state; break;
	case SDLK_3: key[0x3] = state; break;
	case SDLK_4: key[0xC] = state; break;

		// Row 2
	case SDLK_q: key[0x4] = state; break;
	case SDLK_w: key[0x5] = state; break;
	case SDLK_e: key[0x6] = state; break;
	case SDLK_r: key[0xD] = state; break;

		// Row 3
	case SDLK_a: key[0x7] = state; break;
	case SDLK_s: key[0x8] = state; break;
	case SDLK_d: key[0x9] = state; break;
	case SDLK_f: key[0xE] = state; break;

		// Row 4
	case SDLK_z: key[0xA] = state; break;
	case SDLK_x: key[0x0] = state; break;
	case SDLK_c: key[0xB] = state; break;
	case SDLK_v: key[0xF] = state; break;
	}
}
