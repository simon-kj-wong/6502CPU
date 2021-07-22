// 6502CPUTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "main_6502.h"

int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);

    // start -- inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84;
    // end -- inline a little program

    cpu.Execute(3, mem);

    return 0;
}

