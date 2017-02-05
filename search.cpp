#include "board.h"
#include "search.h"
#include "move_generation.h"
#include "evaluate.h"
#include <Windows.h>
#include <iostream>

using namespace std;

static void checkUp() {
	// check for interrupt from GUI

}

static void clearForSearch(Board &position, SearchInfo &info) {
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 64; j++) {
			position.searchHistory[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			position.searchKillers[i][j] = 0;
		}
	}

	// should principal variation table be cleared????
	// clearPVTable(position.pvTable);

	position.ply = 0;

	info.startTime = (long) GetTickCount64();
	info.stopped = false;
	info.nodes = 0;

	info.failHigh = 0;
	info.failHighFirst = 0;
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
	info.nodes++;

	// pre-search checks

	if (depth == 0) {
		return evaluatePosition(position);
	}
	if (isRepetition(position) || position.fiftyMoveCounter >= 100) {
		return 0;
	}
	if (position.ply >= MAX_DEPTH) {
		return evaluatePosition(position);
	}

	MoveList list;
	generateMoves(position, list);

	int legal = 0;
	int oldAlpha = alpha;
	int bestMove = -1;
	int score = -100000;

	for (int i = 0; i < list.count; i++) {
		int move = list.moves[i].move;
		if (!makeMove(position, move)) {
			continue;
		}

		legal++;
		score = -alphaBeta(position, info, -beta, -alpha, depth - 1, true);
		unmakeMove(position);

		if (score > alpha) {
			if (score >= beta) {
				if (legal == 1) {
					info.failHighFirst++;
				}
				info.failHigh++;
				return beta;
			}
			alpha = score;
			bestMove = move;
		}
	}

	if (legal == 0) {
		if (underCheck(position, position.side)) {
			return -50000 + position.ply;
		}
		else {
			return 0;
		}
	}

	if (oldAlpha != alpha) {
		storePVMove(position, bestMove);
	}

	return alpha;
}

static int quiescence(Board &position, SearchInfo &info, int alpha, int beta) {

}

void searchPosition(Board &position, SearchInfo &info) {
	int bestMove = -1;
	int bestScore = -100000;

	clearForSearch(position, info);

	for (int currentDepth = 1; currentDepth <= info.depth; currentDepth++) {
		bestScore = alphaBeta(position, info, -100000, 100000, currentDepth, true);
		int pvMoves = getPV(position, currentDepth);
		bestMove = position.pvArray[0];

		cout << endl << "depth: " << currentDepth << ", score: " << bestScore << ", move: " << moveToString(bestMove) << ", # nodes: " << info.nodes << endl;
		cout << "(";
		for (int i = 0; i < pvMoves; i++) {
			cout << " " << moveToString(position.pvArray[i]);
		}
		cout << " )" << endl;
		if (info.failHigh == 0) info.failHigh = 1;
		cout << "ordering: " << (info.failHighFirst / info.failHigh) * 100 << "%" << endl;
	}
}