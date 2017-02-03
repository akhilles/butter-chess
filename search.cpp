#include "board.h"
#include "search.h"

static void checkUp() {
	// check for interrupt from GUI

}

static void clearForSearch(Board &position, SearchInfo &info) {

}

static bool isRepetition(const Board &position) {
	for (int i = position.histPly - position.fiftyMoveCounter; i < position.histPly - 1; i++) {
		if (position.hashKey == position.history[i].hashKey) {
			return true;
		}
	}
	return false;
}

static int alphaBeta(Board &position, SearchInfo &info, int alpha, int beta, int depth, bool doNull) {

}

static int quiescence(Board &position, SearchInfo &info, int alpha, int beta) {

}

void searchPosition(Board &position, SearchInfo &info) {

}