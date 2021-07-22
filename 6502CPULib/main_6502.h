#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdint>

using namespace std;

using Byte = uint8_t;
//Byte represents 8 bit
using Word = uint16_t;
//Word represents 16 bit

using u32 = uint32_t;

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    /* reads 1 byte*/
    Byte operator[](u32 Address) const
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    /* write 1 byte*/
    Byte& operator[](u32 Address)
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    void WriteWord(Word Value, u32 Address, u32& Cycles)
    {
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = Value >> 8;
        Cycles -= 2;
    }
};


struct CPU
{
    Word PC;        //program counter
    Byte SP;        //stack pointer

    Byte A, X, Y;   //registers


    // represents status flags
    // bit fields restrict the range of values that the variable can obtain
    Byte C : 1;
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    // create Reset function that emulates Commodore 64 (Home computer system)
    void Reset(Mem& memory)
    {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Byte ReadByte(u32& Cycles, Byte Address, Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Byte FetchWord(u32& Cycles, Mem& memory)
    {
        // 6502 is little endian
        Word Data = memory[PC];
        PC++;

        // "<< 8" represents shifting to the left 8 bits
        Data |= (memory[PC] << 8);
        PC++;

        Cycles -= 2;

        // if wanted to handle endianness would need to swap here
        // my system is Little-Endian so no need to change

        return Data;
    }

    //opcodes
    static constexpr Byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0x20;

    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute(u32 Cycles, Mem& memory)
    {
        while (Cycles > 0)
        {
            Byte Ins = FetchByte(Cycles, memory);
            switch (Ins)
            {
            case INS_LDA_IM:
            {
                Byte Value = FetchByte(Cycles, memory);
                A = Value;
                LDASetStatus();
            } break;
            case INS_LDA_ZP:
            {
                Byte ZeroPageAddr = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddr, memory);
                LDASetStatus();
            } break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddr = FetchByte(Cycles, memory);
                ZeroPageAddr += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageAddr, memory);
                LDASetStatus();
            }break;
            case INS_JSR:
            {
                Word SubAddr = FetchWord(Cycles, memory);
                memory.WriteWord(PC - 1, SP, Cycles);
                PC = SubAddr;
                SP++;
                Cycles--;
            }break;
            default:
            {
                printf("Instruction not handled %d", Ins);
            } break;
            }
        }
    }
};