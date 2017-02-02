#include "board.h"
#include "bitboards.h"
#include "evaluate.h"
#include "validate.h"
#include "search.h"
#include <string>
#include <iostream>

using namespace std;

U64 generateHashKey(const Board &position) {
	U64 key = 0ULL;
	for (int i = 2; i < 14; i++) {
		U64 bb = position.pieceBB[i];
		for (int j = 0; j < 64; j++) {
			if (bb & setMask[j]) key ^= pieceKeys[i][j];
		}
	}
	if (position.side == WHITE) key ^= sideKey;
	if (position.enPassantSquare != 64) key ^= enPassantKeys[position.enPassantSquare];
	key ^= castleKeys[position.castlePermissions];
	return key;
}

void initBoard(Board &position, string fen) {
	for (int i = 0; i < 14; i++) {
		position.pieceBB[i] = 0ULL;
	}
	int fenPos = 0;
	int rank = RANK_8;
	int file = FILE_A;

	while (rank >= RANK_1) {
		int piece = -1;
		char c = fen[fenPos++];
		switch (c) {
		case 'r': piece = BLACK_ROOK; break;
		case 'n': piece = BLACK_KNIGHT; break;
		case 'b': piece = BLACK_BISHOP; break;
		case 'q': piece = BLACK_QUEEN; break;
		case 'k': piece = BLACK_KING; break;
		case 'p': piece = BLACK_PAWN; break;
		case 'R': piece = WHITE_ROOK; break;
		case 'N': piece = WHITE_KNIGHT; break;
		case 'B': piece = WHITE_BISHOP; break;
		case 'Q': piece = WHITE_QUEEN; break;
		case 'K': piece = WHITE_KING; break;
		case 'P': piece = WHITE_PAWN; break;
		case '/':
		case ' ': rank--; file = FILE_A; break;
		default: file += (c - '0'); break;
		}
		if (piece > 0) {
			setBit(position.pieceBB[piece], getSquare(file, rank));
			file++;
		}
	}
	char side = fen[fenPos];
	position.side = ((side == 'w') ? WHITE : BLACK);

	string part2 = fen.substr(fenPos + 2, 40);
	int spaceIndex1 = int(part2.find(" ", 0)) + 1;
	string castlePermissions = part2.substr(0, spaceIndex1);
	position.castlePermissions = 0;
	if (castlePermissions.find('K') != string::npos) position.castlePermissions |= WHITE_KING_CASTLE;
	if (castlePermissions.find('Q') != string::npos) position.castlePermissions |= WHITE_QUEEN_CASTLE;
	if (castlePermissions.find('k') != string::npos) position.castlePermissions |= BLACK_KING_CASTLE;
	if (castlePermissions.find('q') != string::npos) position.castlePermissions |= BLACK_QUEEN_CASTLE;

	int enPassantLength = (int) (part2.find(" ", spaceIndex1)) - spaceIndex1;
	string enPassantSquare = part2.substr(spaceIndex1, enPassantLength);

	if (enPassantSquare == "-") position.enPassantSquare = 64;
	else position.enPassantSquare = (enPassantSquare[1] - '1') * 8 + (enPassantSquare[0] - 'a');
	position.ply = 0;
	position.histPly = 0;
	position.fiftyMoveCounter = 0;

	position.pieceBB[WHITE] = 0ULL | position.pieceBB[WHITE_ROOK] | position.pieceBB[WHITE_KNIGHT] | position.pieceBB[WHITE_BISHOP] | position.pieceBB[WHITE_QUEEN] | position.pieceBB[WHITE_KING] | position.pieceBB[WHITE_PAWN];
	position.pieceBB[BLACK] = 0ULL | position.pieceBB[BLACK_ROOK] | position.pieceBB[BLACK_KNIGHT] | position.pieceBB[BLACK_BISHOP] | position.pieceBB[BLACK_QUEEN] | position.pieceBB[BLACK_KING] | position.pieceBB[BLACK_PAWN];
	position.occupiedBB = 0ULL | position.pieceBB[WHITE] | position.pieceBB[BLACK];
	position.emptyBB = ~position.occupiedBB;

	position.hashKey = generateHashKey(position);
	position.material[WHITE] = materialValueWhite(position);
	position.material[BLACK] = materialValueBlack(position);

	initPVTable(position.pvTable);
}

bool checkHashKey(const Board &position) {
	bool valid = generateHashKey(position) == position.hashKey;
	if (!valid) {
		for (int i = 0; i < position.histPly; i++) {
			debugMove(position.history[i].move);
		}
		printBoard(position);
	}
	return valid;
}

bool checkMaterialCount(const Board &position) {
	bool whiteMaterialValid = materialValueWhite(position) == position.material[WHITE];
	bool blackMaterialValid = materialValueBlack(position) == position.material[BLACK];
	if (!whiteMaterialValid) {
		printBoard(position);
		cout << "white material: " << position.material[WHITE] << endl;
		cout << "expected:       " << materialValueWhite(position) << endl;
		for (int i = 0; i < position.histPly; i++) {
			debugMove(position.history[i].move);
		}
	}
	if (!blackMaterialValid) {
		printBoard(position);
		cout << "black material: " << position.material[BLACK] << endl;
		cout << "expected:       " << materialValueBlack(position) << endl;
		for (int i = 0; i < position.histPly; i++) {
			debugMove(position.history[i].move);
		}
	}
	return whiteMaterialValid && blackMaterialValid;
}