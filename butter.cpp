#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include "validate.h"
#include "search.h"
#include "evaluate.h"
#include <string>
#include <iostream>
#include <windows.h>
using namespace std;

void search(Board &position) {
	SearchInfo info;
	info.depth = 7;
	searchPosition(position, info);
}

int main()
{
	const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	const string POS_1 = "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -";
	const string POS_2 = "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1";
	initAll();
	
	Board position;
	initBoard(position, POS_2);
	initPVTable(position.pvTable);

	while (true) {
		cout << endl;
		printBoard(position);
		string input;
		cout << endl << "> ";
		cin >> input;

		switch (input[0]) {
			case 'q': return 0; break;
			case 't': unmakeMove(position); break;
			case 'p': perftDivided(STARTING_FEN, 5); break;
			case 's': search(position); break;
			default: makeMove(position, parseMove(position, input)); break;
		}
	}
	
	return 0;
}