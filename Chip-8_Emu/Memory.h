#pragma once
class Memory
{

public:
	void loadFont();


private: 
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned char delayTimer;
	unsigned char soundTimer;
	unsigned short PC;
	unsigned char SP;
	unsigned short stack[16];


};

