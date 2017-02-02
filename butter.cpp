#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include "validate.h"
#include "search.h"
#include <string>
#include <iostream>
#include <windows.h>
using namespace std;

int main()
{
	const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	initAll();

	
	Board position;
	initBoard(position, STARTING_FEN);

	while (true) {
		cout << endl;
		printBoard(position);
		string input;
		cout << endl << "> ";
		cin >> input;

		switch (input[0]) {
			case 'q': return 0; break;
			case 't': unmakeMove(position); break;
			case 'p': perftDivided(STARTING_FEN, 6); break;
			default: makeMove(position, parseMove(position, input)); break;
		}
	}
	
	return 0;
}