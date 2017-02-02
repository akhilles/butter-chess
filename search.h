#pragma once

#include "board.h"

extern void initPVTable(PVTable &table);
extern void storePVMove(Board &position, int move);
extern int probePVTable(const Board &position);

extern int getPV(Board &position, const int depth);