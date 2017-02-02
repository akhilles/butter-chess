#pragma once

#include <string>

extern bool checkHashKey(const Board &position);
extern bool checkMaterialCount(const Board &position);

extern void perftRoot(const std::string fen, const int depth);
extern void perftDivided(const std::string fen, const int depth);

extern void debugMove(int move);