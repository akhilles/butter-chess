#pragma once

const int pieceValue[14] = { 0, 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };

extern int materialValueWhite(const Board &position);
extern int materialValueBlack(const Board &position);