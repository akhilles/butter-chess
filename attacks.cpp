#include "board.h"
#include "bitboards.h"
#include "move_generation.h"

U64 rookAttacks(const Board &position, int sq) {
	U64 occ = position.occupiedBB | 0x8000000000000001;
	int n = peekBit(occ & (bitRays[NORTH][sq] | setMask[63]));
	int e = peekBit(occ & (bitRays[EAST][sq] | setMask[63]));
	int s = peekBitReverse(occ & (bitRays[SOUTH][sq] | setMask[0]));
	int w = peekBitReverse(occ & (bitRays[WEST][sq] | setMask[0]));
	return rookAttacksEmpty[sq] ^ bitRays[NORTH][n] ^ bitRays[EAST][e] ^ bitRays[SOUTH][s] ^ bitRays[WEST][w];
}

U64 bishopAttacks(const Board &position, int sq) {
	U64 occ = position.occupiedBB | 0x8000000000000001;
	int nw = peekBit(occ & (bitRays[NORTH_WEST][sq] | setMask[63]));
	int ne = peekBit(occ & (bitRays[NORTH_EAST][sq] | setMask[63]));
	int sw = peekBitReverse(occ & (bitRays[SOUTH_WEST][sq] | setMask[0]));
	int se = peekBitReverse(occ & (bitRays[SOUTH_EAST][sq] | setMask[0]));
	return bishopAttacksEmpty[sq] ^ bitRays[NORTH_WEST][nw] ^ bitRays[NORTH_EAST][ne] ^ bitRays[SOUTH_WEST][sw] ^ bitRays[SOUTH_EAST][se];
}

bool attackedByWhite(const Board &position, int sq) {
	if (knightAttacks[sq] & wN) return true;
	if (bishopAttacks(position, sq) & (wB | wQ)) return true;
	if (rookAttacks(position, sq) & (wR | wQ)) return true;
	if ((((wP << 7) & NOT_FILE_H_MASK) | ((wP << 9) & NOT_FILE_A_MASK)) & setMask[sq]) return true;
	if (kingAttacks[sq] & wK) return true;
	return false;
}

bool attackedByBlack(const Board &position, int sq) {
	if (knightAttacks[sq] & bN) return true;
	if (bishopAttacks(position, sq) & (bB | bQ)) return true;
	if (rookAttacks(position, sq) & (bR | bQ)) return true;
	if ((((bP >> 9) & NOT_FILE_H_MASK) | ((bP >> 7) & NOT_FILE_A_MASK)) & setMask[sq]) return true;
	if (kingAttacks[sq] & bK) return true;
	return false;
}

bool underCheck(const Board &position, int side) {
	if (side == WHITE) return attackedByBlack(position, peekBit(position.pieceBB[WHITE_KING]));
	else return attackedByWhite(position, peekBit(position.pieceBB[BLACK_KING]));
}