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
	std::fill(std::begin(gfx), std::end(gfx), 0);

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

// Read rom load to memory
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

	// Read into memory array starting at memory location 0x200
	// reinterpret_cast to read unsigned char*
	file.read(reinterpret_cast<char*>(&memory[PC]), size);

}

unsigned short Chip_8::fetch() {

	// Fetch: Opcode is 16-bit, must combine 2 instructions from memory
	opcode = memory[PC] << 8 | memory[PC + 1];
	PC += 2;

	return opcode;

}

void Chip_8::emulationCycle() {

	fetch();

	// Decode & execute
	switch (opcode & 0xF000) {

		case 0x0000:

			switch (opcode & 0x000F) {

			// 0x00E0: Clear gfx[]/screen
			case 0x0000:
				for (unsigned short i = 0; i < (64 * 32); ++i) gfx[i] = 0;
				break;

			// 0x00EE: Return from subroutine
			case 0x000E:
				
				PC = stack[SP];
				--SP;

				break;

			default:
				std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n\n";
				break;
			}

			break;

		// 0x1nnn: 
		// Set PC to nnn
		case 0x1000:
			PC = opcode & 0x0FFF;
			break;

		// 0x2nnn
		// Increment SP, add current PC address to stack
		// Set PC to nnn
		case 0x2000:

			++SP;
			stack[SP] = PC;
			PC = opcode & 0x0FFF;

			break;

		// 0x3xnn
		// Increment PC if V[x] == nn
		case 0x3000:
			
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
				PC += 2;
			}

			break;

		// 0x4xnn
		// Increment PC if V[x] != nn
		case 0x4000:
			
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				PC += 2;
			}

			break;

		// 0x5xy0
		// Increment PC if V[x] == V[y]
		case 0x5000:

			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
				PC += 2;
			}

			break;

		// 0x6xnn
		// nn is placed into V[x] register
		case 0x6000:
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			break;

		// 0x7xnn:
		// V[x] += nn
		case 0x7000:
			V[((opcode & 0x0F00) >> 8)] += opcode & 0x00FF;
			break;

		// 0x8xy(0-E)
		case 0x8000:

			switch (opcode & 0x000F) {

			// 0x8xy0
			// Set V[x] to V[y]
			case 0x0000:

				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy1
			// Set V[x] |= V[y]
			case 0x0001:

				V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy2
			// Set V[x] &= V[y]
			case 0x0002:

				V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy3
			// Set V[x] ^= V[y]
			case 0x0003:

				V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];

				break;

			// 0x8xy4
			// Set V[x] += V[y]
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

			// 0x8xy5
			// x == V[x] -= V[y]
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

			// 0x8xy6
			// Lease significant bit of V[x] sets V[0xF]
			// Set V[x] >>= 1
			// Modern approach being used, some older games may not work
			case 0x0006: {

				unsigned char flag = V[(opcode & 0x0F00) >> 8] & 0x1;

				V[(opcode & 0x0F00) >> 8] >>= 1;
				V[0xF] = flag;

				break;
			}

			// 0x8xy7
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

			// 0x8xyE
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

		// 0x9xy0
		// Increment PC if V[x] != V[y]
		case 0x9000:
			
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
				PC += 2;
			}

			break;

		// 0xAnnn
		// nnn added to I
		case 0xA000:
			I = opcode & 0x0FFF;
			break;

		// 0xBnnn
		// Set PC to nnn + v[0]
		case 0xB000:
			PC = ((opcode & 0x0FFF) + V[0]);
			break;

		// 0xCxnn
		// Create random number between 0-255
		// Set V[x] to random number masked by nn
		case 0xC000: {
			unsigned char min = 0;
			unsigned char max = 255;

			srand(static_cast<unsigned int>(std::time(nullptr)));

			randNum = rand() % ((max - min) + 1) + min;
			V[(opcode & 0x0F00) >> 8] = randNum & (opcode & 0x00FF);

			break;
		}

		// 0xDxyn
		// Draw sprites to screen
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
			
			// 0xEx9E
			// If key[x] is pressed then skip
			case 0x009E:

				if (key[V[(opcode & 0x0F00) >> 8]] != 0) { PC += 2; }

				break;
			
			// 0xExA1
			// If key[x] is not pressed then skip
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

			// 0xFx07 
			// set vX to the value of the delay timer
			case 0x0007:

				V[(opcode & 0x0F00) >> 8] = delayTimer;

				break;

			// 0xFx0A
			// wait for a key pressed and released and set vX to it
			case 0x000A: {

				if (!waitKey) {
					for (unsigned char i = 0; i < 16; ++i) {
						if (key[i] != 0) {
							pressedKey = i;
							waitKey = true;
							break;
						}
					}

					PC -= 2;
				}
				else {
					if (key[pressedKey] != 0){
						PC -= 2;
					}
					else {
						V[(opcode & 0x0F00) >> 8] = pressedKey;
						waitKey = false;
					}
				}

				break;
			}

			// 0xFx15	
			// Set delay timer to vX
			case 0x0015:

				delayTimer = V[(opcode & 0x0F00) >> 8];

				break;

			// 0xFx18	
			// set sound timer to vX
			case 0x0018:

				soundTimer = V[(opcode & 0x0F00) >> 8];

				break;

			// 0xFx1E
			// I += V[x]
			case 0x001E:

				I += V[(opcode & 0x0F00) >> 8];

				break;

			// 0xFx29 
			// Set I to the 5 line high hex sprite
			case 0x0029:

				I = V[(opcode & 0x0F00) >> 8] * 5;

				break;

			// 0xFx33
			// Stores the Binary-coded decimal representation of V[x]
			// Store each digit in memory at I, I+1 and I+2
			case 0x0033:

				memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
				memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;

				break;

			// 0xFx55
			// Load V[0] through V[x] to memory starting at I
			// Incremember I by x + 1
			case 0x0055:

				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
					memory[I + i] = V[i];
				}

				I += ((opcode & 0x0F00) >> 8) + 1;

				break;
			
			// 0xFx65
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

			break;

		default: 
			std::cout << "Opcode: " << std::hex << opcode << std::dec << " not found!\n";
	}
}

void Chip_8::handleInput(SDL_Event& event) {


	// Exit when ESC key or window close button is pressed/clicked
	if (event.key.keysym.sym == SDLK_ESCAPE) SDL_Quit();
	if (event.window.event == SDL_WINDOWEVENT_CLOSE) SDL_Quit();

	// Set state depending if key pressed/released
	unsigned char state = (event.type == SDL_KEYDOWN) ? 1 : 0;

	// Bind chip-8 keypad to keyboard keys
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

void Chip_8::updateTimers() {

	if (delayTimer != 0) { --delayTimer;  }
	if (soundTimer != 0) { --soundTimer; }
	
}

void Chip_8::startMenu() {
	char input;
	bool looper = true;

	std::cout << std::setw(100) << "********************************************************************************" << std::endl;
	std::cout << std::setw(100) << "********************************************************************************" << std::endl;
	std::cout << std::setw(100) << "*                                                                              *" << std::endl;
	std::cout << std::setw(100) << "*                      PARMESEAN3030'S CHIP-8 EMULATOR                         *" << std::endl;
	std::cout << std::setw(100) << "*                                                                              *" << std::endl;
	std::cout << std::setw(100) << "*                      PLEASE SELECT WHICH ROM TO LOAD                         *" << std::endl;
	std::cout << std::setw(100) << "*                                                                              *" << std::endl;
	std::cout << std::setw(100) << "********************************************************************************" << std::endl;
	std::cout << std::setw(100) << "********************************************************************************" << std::endl;
	std::cout << std::endl << std::endl;

	std::cout << "TEST ROMS" << std::endl;
	std::cout << "1: Timendus Logo" << std::endl;
	std::cout << "2: IBM Logo" << std::endl;
	std::cout << "3: Corax+" << std::endl;
	std::cout << "4: Flags" << std::endl;
	std::cout << "5: Keypad" << std::endl;
	std::cout << std::endl;

	std::cout << "GAME ROMS" << std::endl;
	std::cout << "6: Space Invaders" << std::endl;
	std::cout << std::endl;

	while (looper) {
		std::cout << "Please choose the number associated with the ROM you want to load." << std::endl << std::endl;
		std::cout << "Load ROM Number: ";
		std::cin >> input;

		// Discard anything past the first character up to \n
		// Prevents string input from filling input buffer
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (input) {
		case '1': rom = "ROMs/Test/1-chip8-logo.ch8"; looper = false; break;
		case '2': rom = "ROMs/Test/2-ibm-logo.ch8"; looper = false; break;
		case '3': rom = "ROMs/Test/3-corax+.ch8"; looper = false; break;
		case '4': rom = "ROMs/Test/4-flags.ch8"; looper = false; break;
		case '5': rom = "ROMs/Test/5-keypad.ch8"; looper = false; break;
		case '6': rom = "ROMs/Games/INVADERS.ch8"; looper = false; break;
		default: std::cout << "\n\nInvalid option, please try again!\n\n"; break;
		}
	}

	loadRom(rom);

}
