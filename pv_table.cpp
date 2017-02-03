#include "board.h"
#include "search.h"
#include "move_generation.h"

const int pvSize = 0x100000 * 2;


int getPV(Board &position, const int depth) {
	int move = probePVTable(position);
	int count = 0;

	while (move > 0 && count < depth) {
		if (moveExists(position, move)) {
			makeMove(position, move);
			position.pvArray[count++] = move;
		}
		else {
			break;
		}
		move = probePVTable(position);
	}
	
	while (position.ply > 0) {
		unmakeMove(position);
	}

	return count;
}

void clearPVTable(PVTable &table) {
	for (int i = 0; i < table.numEntries; i++) {
		table.entries[i].hashKey = 0ULL;
		table.entries[i].move = -1;
	}
}

void initPVTable(PVTable &table) {
	table.numEntries = pvSize / sizeof(PVEntry) - 2;
	table.entries = new PVEntry[table.numEntries];
	clearPVTable(table);
}

void storePVMove(Board &position, int move) {
	int index = position.hashKey % position.pvTable.numEntries;
	position.pvTable.entries[index].move = move;
	position.pvTable.entries[index].hashKey = position.hashKey;
}

int probePVTable(const Board &position) {
	int index = position.hashKey % position.pvTable.numEntries;
	PVEntry entry = position.pvTable.entries[index];
	if (entry.hashKey == position.hashKey) {
		return entry.move;
	}
	return -1;
}