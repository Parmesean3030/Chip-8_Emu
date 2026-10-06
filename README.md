# Chip-8 Emulator

#### Emulator Menu
![Menu](https://github.com/Parmesean3030/Chip-8_Emu/blob/master/Screenshots/chip8-1.png)

#### Test Example
![Corax+ Test](https://github.com/Parmesean3030/Chip-8_Emu/blob/master/Screenshots/chip8-2.png)

#### Space Invaders
![Space Invaders](https://github.com/Parmesean3030/Chip-8_Emu/blob/master/Screenshots/chip8-3.png)


## Description

This emulator was created as a fun, extracurricular project during my junior year in college. Written in C++, the primary focus of this project was to learn about bit masking, shifting and how opcodes work. The goal was to pass most of what I considered the critical tests from [Timendus' Test Suite](https://github.com/Timendus/chip8-test-suite) and run at least one game. 

Passed Tests
- CHIP-8 splash screen
- IBM logo
- Corax+ opcode test
- Flags test
- Keypad test

Functional Game
- Space Invaders

CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker on his 1802 microprocessor. It was initially used on the COSMAC VIP and Telmac 1800, which were 8-bit microcomputers made in the mid-1970s.

CHIP-8 was designed to use less memory than other programming languages like BASIC, while still being easier to program than machine code. The language looks like machine code as it uses hexadecimal codes for instructions, but as an interpreter, like BASIC, it does not need to be assembled or linked before running.

CHIP-8 interpreters have since been made for many devices, such as home computers, microcomputers, graphing calculators, mobile phones, and video game consoles. [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

## Build

CMake will handle inclusion of SDL2 libraries and place a copy of the ROMs folder into the build directory for ease of use. C++ 14 and CMake 3.31.6 were used to build this project. Emulator is cross-platform.

General build instructions
```
mkdir build
cd build
cmake ..
cmake --build .
```
## Usage

Emulator opens with menu for user to choose what ROM they want to use. Currently, ROMs are hardcoded and cannot be changed without updating source code. Upon choosing a valid option, the emulator loads the ROM and an SDL2 window is created to display and utilize the ROM. Window can be exited via the escape key or the close window button.

Keypad is emulated as follows...

> Original
> 
> 1 2 3 C
> 
> 4 5 6 D
> 
> 7 8 9 E
> 
> A 0 B F


> Emulator
> 
> 1 2 3 4
> 
> Q W E R
> 
> A S D F
> 
> Z X C V

## Status

This project is considered complete for the time being. However, it is missing a few things that could be added at a later date.

- Add Sound
- Add adjustable cycles per frame
- Add GUI to better facilitate loading ROMs, amongst other functionality


## References

- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://chip8.gulrak.net/
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- https://wiki.libsdl.org/SDL2/FrontPage
- https://www.reddit.com/r/EmuDev/ (and their discord)
