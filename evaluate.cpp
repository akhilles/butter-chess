#include "board.h"
#include "bitboards.h"
#include "evaluate.h"

const int evaluatePieceBoards(const int valueBoard[64], const int pieceValue, U64 whitePieces, U64 blackPieces) {
	int score = 0;
	while (whitePieces) {
		int index = peekBit(whitePieces); popBit(whitePieces);
		score += pieceValue + valueBoard[index];
	}
	while (blackPieces) {
		int index = peekBit(blackPieces); popBit(blackPieces);
		score -= pieceValue + valueBoard[mirror[index]];
	}
	return score;
}

int evaluatePosition(const Board &position) {
	int earlyEval = (bitCount(wP) - bitCount(bP)) * pieceValue[WHITE_PAWN];
	int lateEval = earlyEval;

	earlyEval += evaluatePieceBoards(pawnTableEarly, 0, wP, bP);
	earlyEval += evaluatePieceBoards(knightTableEarly, pieceValue[WHITE_KNIGHT], wN, bN);
	earlyEval += evaluatePieceBoards(bishopTableEarly, pieceValue[WHITE_BISHOP], wB, bB);
	earlyEval += evaluatePieceBoards(rookTableEarly, pieceValue[WHITE_ROOK], wR, bR);
	earlyEval += evaluatePieceBoards(queenTableEarly, pieceValue[WHITE_QUEEN], wQ, bQ);
	earlyEval += evaluatePieceBoards(kingTableEarly, 0, wK, bK);

	lateEval += evaluatePieceBoards(pawnTableLate, 0, wP, bP);
	lateEval += evaluatePieceBoards(knightTableLate, pieceValue[WHITE_KNIGHT], wN, bN);
	lateEval += evaluatePieceBoards(bishopTableLate, pieceValue[WHITE_BISHOP], wB, bB);
	lateEval += evaluatePieceBoards(rookTableLate, pieceValue[WHITE_ROOK], wR, bR);
	lateEval += evaluatePieceBoards(queenTableLate, pieceValue[WHITE_QUEEN], wQ, bQ);
	lateEval += evaluatePieceBoards(kingTableLate, 0, wK, bK);

	double gameProgress = (bitCount(position.occupiedBB)) / 32.0;
	int evaluation = int((gameProgress * earlyEval) + ((1 - gameProgress) * lateEval));

	if (position.side == WHITE) {
		return evaluation;
	} else {
		return -evaluation;
	}
}