#include "board.h"
#include "move_generation.h"
#include "validate.h"
#include <iostream>
#include <string>

using namespace std;

long leafNodes;

void perft(Board &position, const int depth) {
	if (depth == 0) {
		leafNodes++;
		return;
	}

	MoveList list;
	generateMoves(position, list);
	for (int i = 0; i < list.count; i++) {
		int move = list.moves[i].move;
		if (!makeMove(position, move)) continue;
		perft(position, depth - 1);
		unmakeMove(position);
	}
}

void perftRoot(const string fen, const int depth) {
	Board position;
	initBoard(position, fen);
	for (int i = 1; i <= depth; i++) {
		leafNodes = 0;
		perft(position, i);
		cout << "depth " << i << ": " << leafNodes << endl;
	}
}

void perftDivided(const string fen, const int depth) {
	Board position;
	initBoard(position, fen);
	printBoard(position);
	leafNodes = 0;

	MoveList list;
	generateMoves(position, list);
	for (int i = 0; i < list.count; i++) {
		int move = list.moves[i].move;
		if (!makeMove(position, move)) continue;

		long cumNodes = leafNodes;
		perft(position, depth - 1);
		unmakeMove(position);
		long oldNodes = leafNodes - cumNodes;

		cout << moveToString(move) << " (" << move << "): " << oldNodes << endl;
	}
	cout << "total: " << leafNodes << endl;
}