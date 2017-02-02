#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include <iostream>
#include <random>
using namespace std;

U64 setMask[65];
U64 clearMask[65];

int fileArray[64];
int rankArray[64];

U64 knightAttacks[64];
U64 kingAttacks[64];
U64 rookAttacksEmpty[64];
U64 bishopAttacksEmpty[64];

U64 bitRays[8][64];

U64 pieceKeys[14][64];
U64 sideKey;
U64 castleKeys[16];
U64 enPassantKeys[64];

void initBitMasks() {
	for (int index = 0; index < 64; index++) {
		setMask[index] = 1ULL << index;
		clearMask[index] = ~setMask[index];
	}
	setMask[64] = 0ULL;
	clearMask[64] = ~setMask[64];
}

void initFileRankArrays() {
	for (int square = 0; square < 64; square++) {
		fileArray[square] = square % 8;
		rankArray[square] = square / 8;
	}
}

void initKnightAttacks() {
	for (int square = 0; square < 64; square++) {
		int originalFile = fileArray[square];
		int originalRank = rankArray[square];

		int file, rank;
		knightAttacks[square] = 0ULL;

		file = originalFile + 2; rank = originalRank + 1;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		rank = originalRank - 1;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		file = originalFile + 1; rank = originalRank + 2;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		rank = originalRank - 2;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		file = originalFile - 1; rank = originalRank + 2;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		rank = originalRank - 2;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		file = originalFile - 2; rank = originalRank + 1;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
		rank = originalRank - 1;
		if (onBoard(file, rank)) setBit(knightAttacks[square], getSquare(file, rank));
	}
}

void initKingAttacks() {
	for (int square = 0; square < 64; square++) {
		int file = fileArray[square];
		int rank = rankArray[square];

		kingAttacks[square] = 0ULL;

		if (onBoard(file + 1, rank + 1)) setBit(kingAttacks[square], getSquare(file + 1, rank + 1));
		if (onBoard(file + 1, rank)) setBit(kingAttacks[square], getSquare(file + 1, rank));
		if (onBoard(file + 1, rank - 1)) setBit(kingAttacks[square], getSquare(file + 1, rank - 1));
		if (onBoard(file, rank + 1)) setBit(kingAttacks[square], getSquare(file, rank + 1));
		if (onBoard(file, rank - 1)) setBit(kingAttacks[square], getSquare(file, rank - 1));
		if (onBoard(file - 1, rank + 1)) setBit(kingAttacks[square], getSquare(file - 1, rank + 1));
		if (onBoard(file - 1, rank)) setBit(kingAttacks[square], getSquare(file - 1, rank));
		if (onBoard(file - 1, rank - 1)) setBit(kingAttacks[square], getSquare(file - 1, rank - 1));
	}
}

void initRookAttacks() {
	for (int square = 0; square < 64; square++) {
		rookAttacksEmpty[square] = 0ULL;

		int file = fileArray[square];
		int rank = rankArray[square];

		for (int f = file + 1; f <= FILE_H; f++) {
			setBit(rookAttacksEmpty[square], getSquare(f, rank));
		}
		for (int f = file - 1; f >= FILE_A; f--) {
			setBit(rookAttacksEmpty[square], getSquare(f, rank));
		}
		for (int r = rank + 1; r <= RANK_8; r++) {
			setBit(rookAttacksEmpty[square], getSquare(file, r));
		}
		for (int r = rank - 1; r >= RANK_1; r--) {
			setBit(rookAttacksEmpty[square], getSquare(file, r));
		}
	}
}

void initBishopAttacks() {
	for (int square = 0; square < 64; square++) {
		bishopAttacksEmpty[square] = 0ULL;

		for (int tr = square + 9; (tr % 8 > 0) && (tr < 64); tr += 9) {
			setBit(bishopAttacksEmpty[square], tr);
		}
		for (int tl = square + 7; (tl % 8 < 7) && (tl < 64); tl += 7) {
			setBit(bishopAttacksEmpty[square], tl);
		}
		for (int br = square - 7; (br % 8 > 0) && (br >= 0); br -= 7) {
			setBit(bishopAttacksEmpty[square], br);
		}
		for (int bl = square - 9; (bl % 8 < 7) && (bl >= 0); bl -= 9) {
			setBit(bishopAttacksEmpty[square], bl);
		}
	}
}

void initBitRays() {
	for (int i = 0; i < 64; i++) {
		bitRays[NORTH][i] = 0ULL;
		bitRays[NORTH_EAST][i] = 0ULL;
		bitRays[EAST][i] = 0ULL;
		bitRays[SOUTH_EAST][i] = 0ULL;
		bitRays[SOUTH][i] = 0ULL;
		bitRays[SOUTH_WEST][i] = 0ULL;
		bitRays[WEST][i] = 0ULL;
		bitRays[NORTH_WEST][i] = 0ULL;
		for (int sq = i + 8; sq < 64; sq += 8) {
			setBit(bitRays[NORTH][i], sq);
		}
		for (int sq = i + 9; sq < 64; sq += 9) {
			if (fileArray[sq] == FILE_A) break;
			setBit(bitRays[NORTH_EAST][i], sq);
		}
		for (int sq = i + 1; sq < 64; sq += 1) {
			if (fileArray[sq] == FILE_A) break;
			setBit(bitRays[EAST][i], sq);
		}
		for (int sq = i - 7; sq >= 0; sq -= 7) {
			if (fileArray[sq] == FILE_A) break;
			setBit(bitRays[SOUTH_EAST][i], sq);
		}
		for (int sq = i - 8; sq >= 0; sq -= 8) {
			setBit(bitRays[SOUTH][i], sq);
		}
		for (int sq = i - 9; sq >= 0; sq -= 9) {
			if (fileArray[sq] == FILE_H) break;
			setBit(bitRays[SOUTH_WEST][i], sq);
		}
		for (int sq = i - 1; sq >= 0; sq -= 1) {
			if (fileArray[sq] == FILE_H) break;
			setBit(bitRays[WEST][i], sq);
		}
		for (int sq = i + 7; sq < 64; sq += 7) {
			if (fileArray[sq] == FILE_H) break;
			setBit(bitRays[NORTH_WEST][i], sq);
		}
	}
}

void initHashKeys() {
	U64 limit = 18446744073709551615U;
	random_device rd;
	mt19937_64 gen(rd());
	uniform_int_distribution<U64> diss(0, limit);

	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 64; j++) {
			pieceKeys[i][j] = diss(gen);
		}
	}
	sideKey = diss(gen);
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = diss(gen);
	}
	for (int i = 0; i < 64; i++) {
		enPassantKeys[i] = diss(gen);
	}
}

void initAll() {
	initBitMasks();
	initFileRankArrays();

	initKnightAttacks();
	initKingAttacks();
	initRookAttacks();
	initBishopAttacks();

	initBitRays();

	initHashKeys();
}