#include "board.h"
#include "move_generation.h"
#include "evaluate.h"
#include "validate.h"
#include <assert.h>

const int castlePermissionsBoard[64] = {
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	7, 15, 15, 15,  3, 15, 15, 11
};

#define HASH_PIECE(piece, square) (position.hashKey ^= pieceKeys[(piece)][(square)])
#define HASH_CASTLE (position.hashKey ^= castleKeys[position.castlePermissions])
#define HASH_SIDE (position.hashKey ^= sideKey)
#define HASH_EN_PASSANT (position.hashKey ^= enPassantKeys[position.enPassantSquare])

void movePiece(Board &position, int piece, int from, int to) {
	position.pieceBB[piece] ^= ((1ULL << from) | (1ULL << to));
	HASH_PIECE(piece, from);
	HASH_PIECE(piece, to);
	position.pieceBB[position.side] ^= ((1ULL << from) | (1ULL << to));
}

void flipPiece(Board &position, int piece, int square, int side) {
	position.pieceBB[piece] ^= (1ULL << square);
	HASH_PIECE(piece, square);
	position.pieceBB[side] ^= (1ULL << square);
}

bool makeMove(Board &position, const int move) {
	position.history[position.histPly].move = move;
	position.history[position.histPly].castlePermissions = position.castlePermissions;
	position.history[position.histPly].fiftyMoveCounter = position.fiftyMoveCounter;
	position.history[position.histPly].enPassantSquare = position.enPassantSquare;
	position.history[position.histPly].hashKey = position.hashKey;
	position.histPly++;
	position.ply++;

	int from = from(move);
	int to = to(move);
	int movingPiece = moving(move);

	position.fiftyMoveCounter++;
	if (position.enPassantSquare < 64) {
		HASH_EN_PASSANT;
		position.enPassantSquare = 64;
	}

	HASH_CASTLE;
	position.castlePermissions &= castlePermissionsBoard[from] & castlePermissionsBoard[to];
	HASH_CASTLE;

	movePiece(position, movingPiece, from, to);

	if (isCapture(move)) {
		// is capture
		position.fiftyMoveCounter = 0;
		int capturedPiece = captured(move);

		int target = to;
		if (isEnPassant(move)) {
			if (position.side == WHITE) target = to - 8;
			else target = to + 8;
		}
		else if (isPromote(move)) {
			// is pawn promote
			int promotedPiece = promoted(move);
			flipPiece(position, movingPiece, to, position.side);
			flipPiece(position, promotedPiece, to, position.side);
		}
		flipPiece(position, capturedPiece, target, position.side ^ 1);
	}
	else if (isCastle(move)) {
		// is castle
		switch (to) {
		case G1: movePiece(position, WHITE_ROOK, H1, F1); break;
		case C1: movePiece(position, WHITE_ROOK, A1, D1); break;
		case G8: movePiece(position, BLACK_ROOK, H8, F8); break;
		case C8: movePiece(position, BLACK_ROOK, A8, D8); break;
		}
	}
	else if (isPawnStart(move)) {
		// is pawn start
		position.fiftyMoveCounter = 0;
		position.enPassantSquare = (from + to) / 2;
		HASH_EN_PASSANT;
	}
	else if (isPromote(move)) {
		// is pawn promote
		position.fiftyMoveCounter = 0;
		int promotedPiece = promoted(move);
		flipPiece(position, movingPiece, to, position.side);
		flipPiece(position, promotedPiece, to, position.side);
	}
	else if (movingPiece == WHITE_PAWN || movingPiece == BLACK_PAWN) {
		position.fiftyMoveCounter = 0;
	}

	position.occupiedBB = position.pieceBB[WHITE] | position.pieceBB[BLACK];
	position.emptyBB = ~position.occupiedBB;

	position.side ^= 1;
	HASH_SIDE;

	//assert(checkHashKey(position));

	if (underCheck(position, position.side ^ 1)) {
		unmakeMove(position);
		return false;
	}
	return true;
}

void unmakeMove(Board &position) {
	position.histPly--;
	position.ply--;

	int move = position.history[position.histPly].move;
	position.castlePermissions = position.history[position.histPly].castlePermissions;
	position.fiftyMoveCounter = position.history[position.histPly].fiftyMoveCounter;
	position.enPassantSquare = position.history[position.histPly].enPassantSquare;
	position.hashKey = position.history[position.histPly].hashKey;

	position.side ^= 1;

	int from = from(move);
	int to = to(move);
	int movingPiece = moving(move);

	if (isPromote(move)) {
		// was pawn promote
		int promotedPiece = promoted(move);
		position.pieceBB[promotedPiece] ^= (1ULL << to);
		position.pieceBB[movingPiece] ^= (1ULL << to);
	}

	position.pieceBB[movingPiece] ^= ((1ULL << from) | (1ULL << to));
	position.pieceBB[position.side] ^= ((1ULL << from) | (1ULL << to));

	if (isCapture(move)) {
		// was capture
		int target = to;
		if (isEnPassant(move)) {
			if (position.side == WHITE) target = to - 8;
			else target = to + 8;
		}
		int capturedPiece = captured(move);
		position.pieceBB[capturedPiece] ^= (1ULL << target);
		position.pieceBB[position.side ^ 1] ^= (1ULL << target);
	}
	else if (isCastle(move)) {
		// was castle
		switch (to) {
		case G1: wR ^= ((1ULL << H1) | (1ULL << F1));
			position.pieceBB[WHITE] ^= ((1ULL << H1) | (1ULL << F1));
			break;
		case C1: wR ^= ((1ULL << A1) | (1ULL << D1));
			position.pieceBB[WHITE] ^= ((1ULL << A1) | (1ULL << D1));
			break;
		case G8: bR ^= ((1ULL << H8) | (1ULL << F8));
			position.pieceBB[BLACK] ^= ((1ULL << H8) | (1ULL << F8));
			break;
		case C8: bR ^= ((1ULL << A8) | (1ULL << D8));
			position.pieceBB[BLACK] ^= ((1ULL << A8) | (1ULL << D8));
			break;
		}
	}

	position.occupiedBB = position.pieceBB[WHITE] | position.pieceBB[BLACK];
	position.emptyBB = ~position.occupiedBB;
}