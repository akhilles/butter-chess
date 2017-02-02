#include "board.h"

bool isRepetition(const Board &position) {
	for (int i = position.histPly - position.fiftyMoveCounter; i < position.histPly - 1; i++) {
		if (position.hashKey == position.history[i].hashKey) {
			return true;
		}
	}
	return false;
}