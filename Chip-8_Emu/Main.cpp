#include <iostream>
#include "Memory.h"
#include "Chip_8.h"

int main()
{

	Chip_8 chip8;

	chip8.initialize();
	chip8.loadRom();


}