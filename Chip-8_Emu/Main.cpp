#include <iostream>
#include "Memory.h"
#include "Chip_8.h"

int main()
{

	Chip_8 chip8;

	chip8.initialize();
	chip8.loadRom("../ROMs/Test/1-chip8-logo.ch8");
	
	
	for (int i = 0; i < 10; ++i) chip8.emulationCycle();


	return 0;
}