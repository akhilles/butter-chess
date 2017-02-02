#pragma once

#include <intrin.h>

typedef unsigned long long U64;

const U64 FILE_A_MASK = 0x0101010101010101,
		  NOT_FILE_A_MASK = 0xFEFEFEFEFEFEFEFE,
		  NOT_FILE_H_MASK = 0x7F7F7F7F7F7F7F7F,
		  RANK_1_MASK = 0x00000000000000FF,
		  RANK_3_MASK = 0x0000000000FF0000,
		  RANK_6_MASK = 0x0000FF0000000000,
		  RANK_8_MASK = 0xFF00000000000000,
		  NOT_RANK_8_MASK = 0x00FFFFFFFFFFFFFF;

#define popBit(bb) ((bb) &= ((bb) - 1))
#define setBit(bb,sq) ((bb) |= setMask[(sq)])
#define clearBit(bb,sq) ((bb) &= clearMask[(sq)])
#define bitCount(bb) ((int) (__popcnt64(bb)))

extern U64 setMask[65];
extern U64 clearMask[65];

extern void printBitBoard(U64 bb);
extern int peekBit(U64 bb);
extern int peekBitReverse(U64 bb);