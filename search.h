#pragma once

#include "board.h"

struct SearchInfo {
	long startTime;
	long endTime;
	int depth;

	int depthSet;
	int timeSet;
	int movesToGo;

	long nodes;

	bool infinite;
	bool quit;
	bool stopped;

	float failHigh;
	float failHighFirst;
};

extern void initHashTable(HashTable &table);
extern void storeHashEntry(Board &position, int move, int score, int depth, int flags);
bool probeHashEntry(Board &position, int &move, int &score, int alpha, int beta, int depth);

extern int getPV(Board &position, const int depth);

extern void searchPosition(Board &position, SearchInfo &info);