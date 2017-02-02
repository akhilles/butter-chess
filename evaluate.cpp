#include "board.h"
#include "bitboards.h"
#include "evaluate.h"

int materialValueWhite(const Board &position) {
	return bitCount(wR) * pieceValue[WHITE_ROOK]
		 + bitCount(wN) * pieceValue[WHITE_KNIGHT]
		 + bitCount(wB) * pieceValue[WHITE_BISHOP]
		 + bitCount(wQ) * pieceValue[WHITE_QUEEN]
		 + bitCount(wP) * pieceValue[WHITE_PAWN]
		 + pieceValue[WHITE_KING];
}

int materialValueBlack(const Board &position) {
	return bitCount(bR) * pieceValue[BLACK_ROOK]
		 + bitCount(bN) * pieceValue[BLACK_KNIGHT]
		 + bitCount(bB) * pieceValue[BLACK_BISHOP]
		 + bitCount(bQ) * pieceValue[BLACK_QUEEN]
		 + bitCount(bP) * pieceValue[BLACK_PAWN]
		 + pieceValue[BLACK_KING];
}