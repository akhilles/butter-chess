#pragma once

#include <string>

typedef unsigned long long U64;

const int NUM_SQUARES = 64;
const int MAX_GAME_MOVES = 2048;
const int MAX_DEPTH = 64;

#define getSquare(file, rank) (((rank) * 8) + (file))
#define onBoard(file, rank) (file >= FILE_A && file <= FILE_H && rank >= RANK_1 && rank <= RANK_8)

#define wP (position.pieceBB[WHITE_PAWN])
#define wN (position.pieceBB[WHITE_KNIGHT])
#define wB (position.pieceBB[WHITE_BISHOP])
#define wR (position.pieceBB[WHITE_ROOK])
#define wQ (position.pieceBB[WHITE_QUEEN])
#define wK (position.pieceBB[WHITE_KING])
#define bP (position.pieceBB[BLACK_PAWN])
#define bN (position.pieceBB[BLACK_KNIGHT])
#define bB (position.pieceBB[BLACK_BISHOP])
#define bR (position.pieceBB[BLACK_ROOK])
#define bQ (position.pieceBB[BLACK_QUEEN])
#define bK (position.pieceBB[BLACK_KING])

enum {
	WHITE, BLACK,
	WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
	BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
};

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };
enum { WHITE_KING_CASTLE = 1, WHITE_QUEEN_CASTLE = 2, BLACK_KING_CASTLE = 4, BLACK_QUEEN_CASTLE = 8 };

enum {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
};

struct PVEntry {
	U64 hashKey;
	int move;
};

struct PVTable {
	PVEntry *entries;
	int numEntries;
};

struct Undo {
	int move;

	int castlePermissions;
	int fiftyMoveCounter;
	int enPassantSquare;
	U64 hashKey;
};

struct Board {
	U64 pieceBB[14];
	U64 emptyBB, occupiedBB;

	int side;
	int ply, histPly;

	int material[2];

	int enPassantSquare;
	int castlePermissions;
	int fiftyMoveCounter;
	U64 hashKey;
	Undo history[MAX_GAME_MOVES];

	PVTable pvTable;
	int pvArray[MAX_DEPTH];
};

extern int fileArray[64];
extern int rankArray[64];

extern U64 pieceKeys[14][64];
extern U64 sideKey;
extern U64 castleKeys[16];
extern U64 enPassantKeys[64];

extern std::string numSquareToString(const int sq);
extern void printBoard(const Board &position);
extern void initBoard(Board &position, std::string fen);
extern int parseMove(const Board &position, std::string move);
extern void initAll();