#include "board.h"
#include "search.h"
#include "evaluate.h"
#include "move_generation.h"

const int pvSize = 0x100000 * 64;

int probePV(const Board &position) {
	int index = position.hashKey % position.hashTable.numEntries;
	HashEntry entry = position.hashTable.entries[index];
	if (entry.hashKey == position.hashKey) {
		return entry.move;
	}
	return -1;
}

int getPV(Board &position, const int depth) {
	int move = probePV(position);
	int count = 0;

	while (move > 0 && count < depth) {
		if (moveExists(position, move)) {
			makeMove(position, move);
			position.pvArray[count++] = move;
		}
		else {
			break;
		}
		move = probePV(position);
	}
	
	while (position.ply > 0) {
		unmakeMove(position);
	}

	return count;
}

void clearHashTable(HashTable &table) {
	for (int i = 0; i < table.numEntries; i++) {
		table.entries[i].hashKey = 0ULL;
		table.entries[i].move = -1;
		table.entries[i].score = 0;
		table.entries[i].depth = 0;
		table.entries[i].flags = 0;
	}
}

void initHashTable(HashTable &table) {
	table.numEntries = pvSize / sizeof(HashEntry) - 2;
	table.entries = new HashEntry[table.numEntries];
	clearHashTable(table);
}

void storeHashEntry(Board &position, int move, int score, int depth, int flags) {
	int index = position.hashKey % position.hashTable.numEntries;

	if (position.hashTable.entries[index].hashKey == 0ULL) {
		position.hashTable.newWrite++;
	}
	else {
		position.hashTable.overWrite++;
	}

	if (score > IS_MATE) score += position.ply;
	else if (score < -IS_MATE) score -= position.ply;

	position.hashTable.entries[index].hashKey = position.hashKey;
	position.hashTable.entries[index].move = move;
	position.hashTable.entries[index].score = score;
	position.hashTable.entries[index].depth = depth;
	position.hashTable.entries[index].flags = flags;
}

bool probeHashEntry(Board &position, int &move, int &score, int alpha, int beta, int depth) {
	int index = position.hashKey % position.hashTable.numEntries;
	HashEntry entry = position.hashTable.entries[index];
	if (entry.hashKey == position.hashKey) {
		move = entry.move;
		if (entry.depth >= depth) {
			position.hashTable.hit++;
			score = entry.score;
			if (score > IS_MATE) score -= position.ply;
			else if (score < -IS_MATE) score += position.ply;

			switch (entry.flags) {
				case FLAG_ALPHA:
					if (score <= alpha) {
						score = alpha;
						return true;
					} break;
				case FLAG_BETA:
					if (score >= beta) {
						score = beta;
						return true;
					} break;
				case FLAG_EXACT:
					return true;
					break;
			}
		}
	}
	return false;
}