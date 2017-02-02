#include "bitboards.h"
#include <iostream>

using namespace std;

void printBitBoard(U64 bb) {
	for (int square = 56; square >= 0; square++) {
		if (setMask[square] & bb) cout << " X ";
		else cout << " - ";
		if ((square + 1) % 8 == 0) {
			square -= 16;
			cout << endl;
		}
	}
}

int peekBit(U64 bb) {
	unsigned long index = -1;
	_BitScanForward64(&index, bb);
	return index;
}

int peekBitReverse(U64 bb) {
	unsigned long index = -1;
	_BitScanReverse64(&index, bb);
	return index;
}