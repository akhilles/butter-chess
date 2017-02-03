#pragma once

#include "board.h"

struct SearchInfo {
	int startTime;
	int endTime;
	int depth;

	int depthSet;
	int timeSet;
	int movesToGo;

	long nodes;

	bool infinite;
	bool quit;
	bool stopped;
};

extern void initPVTable(PVTable &table);
extern void storePVMove(Board &position, int move);
extern int probePVTable(const Board &position);

extern int getPV(Board &position, const int depth);

extern void searchPosition(Board &position, SearchInfo &info);