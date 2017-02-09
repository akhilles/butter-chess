#include "board.h"
#include "search.h"
#include "move_generation.h"
#include "evaluate.h"
#include <Windows.h>
#include <iostream>

using namespace std;

static void checkUp(SearchInfo &info) {
	// check for interrupt from GUI
	if (info.timeSet && GetTickCount64() > info.endTime) {
		info.stopped = true;
	}
}

static void clearForSearch(Board &position, SearchInfo &info) {
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 64; j++) {
			position.searchHistory[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < MAX_DEPTH; j++) {
			position.searchKillers[i][j] = 0;
		}
	}

	position.hashTable.overWrite = 0;
	position.hashTable.hit = 0;
	position.hashTable.cut = 0;
	position.ply = 0;

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

static void pickNextMove(int moveNum, MoveList &list) {
	int bestScore = 0;
	int bestNum = moveNum;
	
	for (int i = moveNum; i < list.count; i++) {
		if (list.moves[i].score > bestScore) {
			bestScore = list.moves[i].score;
			bestNum = i;
		}
	}

	Move temp = list.moves[moveNum];
	list.moves[moveNum] = list.moves[bestNum];
	list.moves[bestNum] = temp;
}

static int quiescence(Board &position, SearchInfo &info, int alpha, int beta) {
	if ((info.nodes & 8191) == 0) {
		checkUp(info);
	}

	info.nodes++;

	if (isRepetition(position) || position.fiftyMoveCounter >= 100) {
		return 0;
	}
	if (position.ply >= MAX_DEPTH) {
		return evaluatePosition(position);
	}

	int score = evaluatePosition(position);

	if (score >= beta) {
		return beta;
	}
	if (score > alpha) {
		alpha = score;
	}

	MoveList list;
	generateCaptures(position, list);

	int legal = 0;
	int oldAlpha = alpha;
	int bestMove = -1;
	score = -100000;

	for (int i = 0; i < list.count; i++) {
		pickNextMove(i, list);

		int move = list.moves[i].move;
		if (!makeMove(position, move)) {
			continue;
		}

		legal++;
		score = -quiescence(position, info, -beta, -alpha);
		unmakeMove(position);

		if (info.stopped) {
			return 0;
		}

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

	return alpha;
}

static int alphaBeta(Board &position, SearchInfo &info, int alpha, int beta, int depth, bool doNull) {
	// pre-search checks
	if ((info.nodes & 8191) == 0) {
		checkUp(info);
	}

	if (depth == 0) {
		//return evaluatePosition(position);
		return quiescence(position, info, alpha, beta);
	}

	info.nodes++;

	if (isRepetition(position) || position.fiftyMoveCounter >= 100) {
		return 0;
	}
	if (position.ply >= MAX_DEPTH) {
		return evaluatePosition(position);
	}

	int pvMove = -1;
	int score = -100000;
	if (probeHashEntry(position, pvMove, score, alpha, beta, depth)) {
		position.hashTable.cut++;
		return score;
	}

	MoveList list;
	generateMoves(position, list);

	int legal = 0;
	int oldAlpha = alpha;
	int bestMove = -1;
	int bestScore = -100000;

	if (pvMove > 0) {
		for (int i = 0; i < list.count; i++) {
			if (list.moves[i].move == pvMove) {
				list.moves[i].score = 20000000;
				break;
			}
		}
	}

	for (int i = 0; i < list.count; i++) {
		pickNextMove(i, list);

		int move = list.moves[i].move;
		if (!makeMove(position, move)) {
			continue;
		}

		legal++;
		int score = -alphaBeta(position, info, -beta, -alpha, depth - 1, true);
		unmakeMove(position);

		if (info.stopped) {
			return 0;
		}

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha) {
				if (score >= beta) {
					if (legal == 1) {
						info.failHighFirst++;
					}
					info.failHigh++;

					if (!isCapture(move)) {
						position.searchKillers[1][position.ply] = position.searchKillers[0][position.ply];
						position.searchKillers[0][position.ply] = move;
					}

					storeHashEntry(position, bestMove, beta, depth, FLAG_BETA);

					return beta;
				}
				alpha = score;
				bestMove = move;

				if (!isCapture(move)) {
					position.searchHistory[moving(move)][to(move)] += depth;
				}
			}
		}
	}

	if (legal == 0) {
		if (underCheck(position, position.side)) {
			return -MATE_SCORE + position.ply;
		}
		else {
			return 0;
		}
	}

	if (oldAlpha != alpha) {
		storeHashEntry(position, bestMove, bestScore, depth, FLAG_EXACT);
	}
	else {
		storeHashEntry(position, bestMove, alpha, depth, FLAG_ALPHA);
	}

	return alpha;
}

void searchPosition(Board &position, SearchInfo &info) {
	int bestMove = -1;
	int bestScore = -100000;

	clearForSearch(position, info);

	for (int currentDepth = 1; currentDepth <= info.depth; currentDepth++) {
		bestScore = alphaBeta(position, info, -100000, 100000, currentDepth, true);

		if (info.stopped) {
			break;
		}

		int pvMoves = getPV(position, currentDepth);
		bestMove = position.pvArray[0];

		cout << "info score cp " << bestScore << " depth " << currentDepth << " nodes " << info.nodes << " time " << GetTickCount64() - info.startTime << " pv";
		for (int i = 0; i < pvMoves; i++) {
			cout << " " << moveToString(position.pvArray[i]);
		}
		cout << endl;
		if (info.failHigh == 0) info.failHigh = 1;
		//cout << "ordering: " << (info.failHighFirst / info.failHigh) * 100 << "%" << endl;
	}
	cout << "bestmove " << moveToString(bestMove) << endl;
}