#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include <iostream>
#include <string>

using namespace std;

void addQuietMove(const Board &position, const int move, MoveList &list) {
	list.moves[list.count++] = { move, 0 };
}

void addCaptureMove(const Board &position, const int move, MoveList &list) {
	list.moves[list.count++] = { move, 0 };
}

int getWhitePieceAt(const Board &position, U64 targetSquare) {
	for (int piece = WHITE_PAWN; piece <= WHITE_KING; piece++) {
		if ((position.pieceBB[piece] & targetSquare) != 0ULL) return piece;
	}
	return -1;
}

int getBlackPieceAt(const Board &position, U64 targetSquare) {
	for (int piece = BLACK_PAWN; piece <= BLACK_KING; piece++) {
		if ((position.pieceBB[piece] & targetSquare) != 0ULL) return piece;
	}
	return -1;
}

static void generatorRoutineWhite(const Board &position, int piece, MoveList &list) {
	U64 pieceBoard = position.pieceBB[piece];
	int fromIndex;
	while (pieceBoard) {
		fromIndex = peekBit(pieceBoard); popBit(pieceBoard);

		U64 attackBoard;
		switch (piece) {
			case WHITE_KNIGHT: attackBoard = knightAttacks[fromIndex]; break;
			case WHITE_BISHOP: attackBoard = bishopAttacks(position, fromIndex); break;
			case WHITE_ROOK: attackBoard = rookAttacks(position, fromIndex); break;
			case WHITE_QUEEN: attackBoard = bishopAttacks(position, fromIndex) | rookAttacks(position, fromIndex); break;
		}
		
		int toIndex;
		U64 moves = attackBoard & position.emptyBB;
		while (moves) {
			toIndex = peekBit(moves); popBit(moves);
			int move = generateMove(fromIndex, toIndex, 0, piece, 0, 0);
			addQuietMove(position, move, list);
		}

		U64 attacks = attackBoard & position.pieceBB[BLACK];
		while (attacks) {
			toIndex = peekBit(attacks); popBit(attacks);
			int capturedPiece = getBlackPieceAt(position, setMask[toIndex]);
			int move = generateMove(fromIndex, toIndex, capturedPiece, piece, 0, 0);
			addCaptureMove(position, move, list);
		}
	}
}

static void generatorRoutineBlack(const Board &position, int piece, MoveList &list) {
	U64 pieceBoard = position.pieceBB[piece];
	int fromIndex;
	while (pieceBoard) {
		fromIndex = peekBit(pieceBoard); popBit(pieceBoard);

		U64 attackBoard;
		switch (piece) {
			case BLACK_KNIGHT: attackBoard = knightAttacks[fromIndex]; break;
			case BLACK_BISHOP: attackBoard = bishopAttacks(position, fromIndex); break;
			case BLACK_ROOK: attackBoard = rookAttacks(position, fromIndex); break;
			case BLACK_QUEEN: attackBoard = bishopAttacks(position, fromIndex) | rookAttacks(position, fromIndex); break;
		}

		int toIndex;
		U64 moves = attackBoard & position.emptyBB;
		while (moves) {
			toIndex = peekBit(moves); popBit(moves);
			int move = generateMove(fromIndex, toIndex, 0, piece, 0, 0);
			addQuietMove(position, move, list);
		}

		U64 attacks = attackBoard & position.pieceBB[WHITE];
		while (attacks) {
			toIndex = peekBit(attacks); popBit(attacks);
			int capturedPiece = getWhitePieceAt(position, setMask[toIndex]);
			int move = generateMove(fromIndex, toIndex, capturedPiece, piece, 0, 0);
			addCaptureMove(position, move, list);
		}
	}
}

static void generateWhitePawnMoves(const Board &position, MoveList &list) {
	U64 advance1 = (position.pieceBB[WHITE_PAWN] << 8) & position.emptyBB;
	U64 advance2 = ((advance1 & RANK_3_MASK) << 8) & position.emptyBB;
	int to;
	while (advance1 & NOT_RANK_8_MASK) {
		to = peekBit(advance1 & NOT_RANK_8_MASK); popBit(advance1);
		int move = generateMove(to - 8, to, 0, WHITE_PAWN, 0, 0);
		addQuietMove(position, move, list);
	}
	while (advance1) {
		to = peekBit(advance1); popBit(advance1);
		int from = to - 8;
		addQuietMove(position, generateMove(from, to, 0, WHITE_PAWN, WHITE_QUEEN, 0), list);
		addQuietMove(position, generateMove(from, to, 0, WHITE_PAWN, WHITE_KNIGHT, 0), list);
		addQuietMove(position, generateMove(from, to, 0, WHITE_PAWN, WHITE_BISHOP, 0), list);
		addQuietMove(position, generateMove(from, to, 0, WHITE_PAWN, WHITE_ROOK, 0), list);
	}
	while (advance2) {
		to = peekBit(advance2); popBit(advance2);
		int move = generateMove(to - 16, to, 0, WHITE_PAWN, 0, FLAG_PS);
		addQuietMove(position, move, list);
	}

	U64 leftAttacks = (position.pieceBB[WHITE_PAWN] << 7) & NOT_FILE_H_MASK & (position.pieceBB[BLACK] | setMask[position.enPassantSquare]);
	while (leftAttacks & NOT_RANK_8_MASK) {
		to = peekBit(leftAttacks & NOT_RANK_8_MASK); popBit(leftAttacks);
		int from = to - 7;
		int captured = getBlackPieceAt(position, 1ULL << to);
		int move = generateMove(from, to, captured, WHITE_PAWN, 0, 0);
		if (to == position.enPassantSquare) move = generateMove(from, to, BLACK_PAWN, WHITE_PAWN, 0, FLAG_EP);
		addCaptureMove(position, move, list);
	}
	while (leftAttacks) {
		to = peekBit(leftAttacks); popBit(leftAttacks);
		int from = to - 7;
		int captured = getBlackPieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_QUEEN, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_KNIGHT, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_BISHOP, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_ROOK, 0), list);
	}

	U64 rightAttacks = (position.pieceBB[WHITE_PAWN] << 9) & NOT_FILE_A_MASK & (position.pieceBB[BLACK] | setMask[position.enPassantSquare]);
	while (rightAttacks & NOT_RANK_8_MASK) {
		to = peekBit(rightAttacks & NOT_RANK_8_MASK); popBit(rightAttacks);
		int from = to - 9;
		int captured = getBlackPieceAt(position, 1ULL << to);
		int move = generateMove(from, to, captured, WHITE_PAWN, 0, 0);
		if (to == position.enPassantSquare) move = generateMove(from, to, BLACK_PAWN, WHITE_PAWN, 0, FLAG_EP);
		addCaptureMove(position, move, list);
	}
	while (rightAttacks) {
		to = peekBit(rightAttacks); popBit(rightAttacks);
		int from = to - 9;
		int captured = getBlackPieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_QUEEN, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_KNIGHT, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_BISHOP, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_PAWN, WHITE_ROOK, 0), list);
	}
}

static void generateBlackPawnMoves(const Board &position, MoveList &list) {
	U64 advance1 = (position.pieceBB[BLACK_PAWN] >> 8) & position.emptyBB;
	U64 advance2 = ((advance1 & RANK_6_MASK) >> 8) & position.emptyBB;
	int to;
	while (advance1 & RANK_1_MASK) {
		to = peekBit(advance1 & RANK_1_MASK); popBit(advance1);
		int from = to + 8;
		addQuietMove(position, generateMove(from, to, 0, BLACK_PAWN, BLACK_QUEEN, 0), list);
		addQuietMove(position, generateMove(from, to, 0, BLACK_PAWN, BLACK_KNIGHT, 0), list);
		addQuietMove(position, generateMove(from, to, 0, BLACK_PAWN, BLACK_BISHOP, 0), list);
		addQuietMove(position, generateMove(from, to, 0, BLACK_PAWN, BLACK_ROOK, 0), list);
	}
	while (advance1) {
		to = peekBit(advance1); popBit(advance1);
		int move = generateMove(to + 8, to, 0, BLACK_PAWN, 0, 0);
		addQuietMove(position, move, list);
	}
	while (advance2) {
		to = peekBit(advance2); popBit(advance2);
		int move = generateMove(to + 16, to, 0, BLACK_PAWN, 0, FLAG_PS);
		addQuietMove(position, move, list);
	}

	U64 leftAttacks = (position.pieceBB[BLACK_PAWN] >> 7) & NOT_FILE_A_MASK & (position.pieceBB[WHITE] | setMask[position.enPassantSquare]);
	while (leftAttacks & RANK_1_MASK) {
		to = peekBit(leftAttacks & RANK_1_MASK); popBit(leftAttacks);
		int from = to + 7;
		int captured = getWhitePieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_QUEEN, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_KNIGHT, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_BISHOP, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_ROOK, 0), list);
	}
	while (leftAttacks) {
		to = peekBit(leftAttacks); popBit(leftAttacks);
		int from = to + 7;
		int captured = getWhitePieceAt(position, 1ULL << to);
		int move = generateMove(from, to, captured, BLACK_PAWN, 0, 0);
		if (to == position.enPassantSquare) move = generateMove(from, to, WHITE_PAWN, BLACK_PAWN, 0, FLAG_EP);
		addCaptureMove(position, move, list);
	}

	U64 rightAttacks = (position.pieceBB[BLACK_PAWN] >> 9) & NOT_FILE_H_MASK & (position.pieceBB[WHITE] | setMask[position.enPassantSquare]);
	while (rightAttacks & RANK_1_MASK) {
		to = peekBit(rightAttacks & RANK_1_MASK); popBit(rightAttacks);
		int from = to + 9;
		int captured = getWhitePieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_QUEEN, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_KNIGHT, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_BISHOP, 0), list);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_PAWN, BLACK_ROOK, 0), list);
	}
	while (rightAttacks) {
		to = peekBit(rightAttacks); popBit(rightAttacks);
		int from = to + 9;
		int captured = getWhitePieceAt(position, 1ULL << to);
		int move = generateMove(from, to, captured, BLACK_PAWN, 0, 0);
		if (to == position.enPassantSquare) move = generateMove(from, to, WHITE_PAWN, BLACK_PAWN, 0, FLAG_EP);
		addCaptureMove(position, move, list);
	}
}

static void generateWhiteKingMoves(const Board &position, MoveList &list) {
	int from = peekBit(position.pieceBB[WHITE_KING]);
	U64 moves = kingAttacks[from] & position.emptyBB;
	int to;
	while (moves) {
		to = peekBit(moves); popBit(moves);
		addQuietMove(position, generateMove(from, to, 0, WHITE_KING, 0, 0), list);
	}
	U64 attacks = kingAttacks[from] & position.pieceBB[BLACK];
	while (attacks) {
		to = peekBit(attacks); popBit(attacks);
		int captured = getBlackPieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, WHITE_KING, 0, 0), list);
	}
	if ((position.castlePermissions & WHITE_KING_CASTLE) && ((position.emptyBB & 0x0000000000000060) == 0x0000000000000060)) {
		if (!attackedByBlack(position, E1) && !attackedByBlack(position, F1)) {
			addQuietMove(position, generateMove(E1, G1, 0, WHITE_KING, 0, FLAG_CA), list);
		}
	}
	if ((position.castlePermissions & WHITE_QUEEN_CASTLE) && ((position.emptyBB & 0x000000000000000E) == 0x000000000000000E)) {
		if (!attackedByBlack(position, E1) && !attackedByBlack(position, D1)) {
			addQuietMove(position, generateMove(E1, C1, 0, WHITE_KING, 0, FLAG_CA), list);
		}
	}
}

static void generateBlackKingMoves(const Board &position, MoveList &list) {
	int from = peekBit(position.pieceBB[BLACK_KING]);
	U64 moves = kingAttacks[from] & position.emptyBB;
	int to;
	while (moves) {
		to = peekBit(moves); popBit(moves);
		addQuietMove(position, generateMove(from, to, 0, BLACK_KING, 0, 0), list);
	}
	U64 attacks = kingAttacks[from] & position.pieceBB[WHITE];
	while (attacks) {
		to = peekBit(attacks); popBit(attacks);
		int captured = getWhitePieceAt(position, 1ULL << to);
		addCaptureMove(position, generateMove(from, to, captured, BLACK_KING, 0, 0), list);
	}
	if ((position.castlePermissions & BLACK_KING_CASTLE) && ((position.emptyBB & 0x6000000000000000) == 0x6000000000000000)) {
		if (!attackedByWhite(position, E8) && !attackedByWhite(position, F8)) {
			addQuietMove(position, generateMove(E8, G8, 0, BLACK_KING, 0, FLAG_CA), list);
		}
	}
	if ((position.castlePermissions & BLACK_QUEEN_CASTLE) && ((position.emptyBB & 0x0E00000000000000) == 0x0E00000000000000)) {
		if (!attackedByWhite(position, E8) && !attackedByWhite(position, D8)) {
			addQuietMove(position, generateMove(E8, C8, 0, BLACK_KING, 0, FLAG_CA), list);
		}
	}
}

void generateMoves(const Board &position, MoveList &list) {
	list.count = 0;
	if (position.side == WHITE) {
		generateWhitePawnMoves(position, list);
		generateWhiteKingMoves(position, list);
		generatorRoutineWhite(position, WHITE_KNIGHT, list);
		generatorRoutineWhite(position, WHITE_BISHOP, list);
		generatorRoutineWhite(position, WHITE_ROOK, list);
		generatorRoutineWhite(position, WHITE_QUEEN, list);
	}
	else {
		generateBlackPawnMoves(position, list);
		generateBlackKingMoves(position, list);
		generatorRoutineBlack(position, BLACK_KNIGHT, list);
		generatorRoutineBlack(position, BLACK_BISHOP, list);
		generatorRoutineBlack(position, BLACK_ROOK, list);
		generatorRoutineBlack(position, BLACK_QUEEN, list);
	}
}

void debugMove(int move) {
	string names[14] = { "none","error","white pawn","white knight","white bishop","white rook","white queen","white king","black pawn","black knight","black bishop","black rook","black queen","black king" };

	int from = from(move);
	int to = to(move);
	int moving = moving(move);
	int captured = captured(move);
	int promoted = promoted(move);

	cout << "move " << moveToString(move) << " (" << move << ") details:" << endl;
	cout << numSquareToString(from) << " to " << numSquareToString(to) << endl;
	cout << "moving: " << names[moving] << endl;
	cout << "captured: " << names[captured] << endl;
	cout << "promoted: " << names[promoted] << endl;
	cout << "flags: ";
	if (isCapture(move)) cout << "capture ";
	if (isPromote(move)) cout << "promote ";
	if (isCastle(move)) cout << "castle ";
	if (isPawnStart(move)) cout << "pawn_start ";
	if (isEnPassant(move)) cout << "en_passant ";
	cout << endl;
}

string moveToString(const int move) {
	char pieceIds[14] = { 'q','q','q','n','b','r','q','q','q','n','b','r','q','q' };
	int fromSquare = from(move);
	int toSquare = to(move);
	string moveString = numSquareToString(fromSquare) + numSquareToString(toSquare);
	if (isPromote(move)) {
		int promotedPiece = promoted(move);
		if (promotedPiece > 13) promotedPiece = 13;
		moveString += pieceIds[promotedPiece];
	}
	return moveString;
}

void printMoveList(const MoveList list) {
	cout << "move list:" << endl;
	for (int i = 0; i < list.count; i++) {
		cout << i + 1 << ": " << moveToString(list.moves[i].move) << endl;
	}
	cout << "total: " << list.count << endl;
}