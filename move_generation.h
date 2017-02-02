#pragma once

#include <string>

typedef unsigned long long U64;

const int MAX_POSITION_MOVES = 256;

const U64 FLAG_CA = 0x1000000;
const U64 FLAG_EP = 0x2000000;
const U64 FLAG_PS = 0x4000000;

enum { NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST };

#define generateMove(f, t, ca, mov, pro, flags) ((f) | ((t) << 6) | ((ca) << 12) | ((mov) << 16) | ((pro) << 20) | flags)

#define from(m) ((m) & 0x3F)
#define to(m) (((m) >> 6) & 0x3F)
#define captured(m) (((m) >> 12) & 0xF)
#define moving(m) (((m) >> 16) & 0xF)
#define promoted(m) (((m) >> 20) & 0xF)

#define isCastle(m) ((m) & 0x1000000)
#define isEnPassant(m) ((m) & 0x2000000)
#define isPawnStart(m) ((m) & 0x4000000)
#define isCapture(m) ((m) & 0xF000)
#define isPromote(m) ((m) & 0xF00000)

struct Move {
	int move;
	int score;
};

struct MoveList {
	Move moves[MAX_POSITION_MOVES];
	int count;
};

extern U64 knightAttacks[64];
extern U64 kingAttacks[64];
extern U64 rookAttacksEmpty[64];
extern U64 bishopAttacksEmpty[64];

extern U64 bitRays[8][64];

extern U64 rookAttacks(const Board &position, int sq);
extern U64 bishopAttacks(const Board &position, int sq);
extern bool attackedByWhite(const Board &position, int sq);
extern bool attackedByBlack(const Board &position, int sq);
extern bool underCheck(const Board &position, int side);

extern bool makeMove(Board &position, const int move);
extern void unmakeMove(Board &position);
extern bool moveExists(Board &position, const int move);

extern std::string moveToString(const int move);
extern void printMoveList(const MoveList list);
extern void generateMoves(const Board &position, MoveList &list);