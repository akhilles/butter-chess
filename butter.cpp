#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include "validate.h"
#include <string>
#include <iostream>
#include <windows.h>
using namespace std;
//created by Nithin Tammishetti
int main()
{
	const string STARTING_FEN = "rnbqkbnr/pPpppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	initAll();

	/*
	Board position;
	initBoard(position, STARTING_FEN);

	while (true) {
		printBoard(position);
		string input;
		cout << endl << "Enter a move: ";
		cin >> input;
		cout << endl;

		switch (input[0]) {
			case 'q': return 0; break;
			case 't': unmakeMove(position); break;
			default: makeMove(position, parseMove(position, input)); break;
		}
	}
	*/

	
	U64 startTime = GetTickCount64();
	perftDivided(STARTING_FEN, 5);
	perftDivided("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 4);
	perftDivided("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 7);
	perftDivided("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5);
	perftDivided("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 5);
	U64 endTime = GetTickCount64();
	double elapsed_secs = (endTime - startTime) / 1000.0;
	cout << "elapsed: " << elapsed_secs << endl;
	getchar();
	return 0;
}