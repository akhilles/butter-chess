#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include "search.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include <io.h>

using namespace std;

string numSquareToString(const int sq) {
	if (sq > 63) return "-";
	else return string(1, 'a' + (sq % 8)) + string(1, '1' + (sq / 8));
}

int parseMove(const Board &position, string move) {
	int from = getSquare(move[0] - 'a', move[1] - '1');
	int to = getSquare(move[2] - 'a', move[3] - '1');

	MoveList list;
	generateMoves(position, list);

	for (int i = 0; i < list.count; i++) {
		int moveTest = list.moves[i].move;
		if (from(moveTest) == from && to(moveTest) == to) {
			if (isPromote(moveTest)) {
				char promote = move[4];
				int promotedPiece = promoted(moveTest);
				switch (promote) {
					case 'b': if (promotedPiece == WHITE_BISHOP || promotedPiece == BLACK_BISHOP) return moveTest; break;
					case 'n': if (promotedPiece == WHITE_KNIGHT || promotedPiece == BLACK_KNIGHT) return moveTest; break;
					case 'r': if (promotedPiece == WHITE_ROOK || promotedPiece == BLACK_ROOK) return moveTest; break;
					case 'q': if (promotedPiece == WHITE_QUEEN || promotedPiece == BLACK_QUEEN) return moveTest; break;
					default: break;
				}
				continue;
			}
			return moveTest;
		}
	}
	return -1;
}

void printBoard(const Board &position) {
	char ids[14] = { '!','!','P','N','B','R','Q','K','p','n','b','r','q','k' };

	for (int r = RANK_8; r >= RANK_1; r--) {
		cout << "" << r + 1 << "  ";
		for (int f = FILE_A; f <= FILE_H; f++) {
			int square = getSquare(f, r);
			bool hit = false;
			for (int piece = 2; piece < 14; piece++) {
				if (position.pieceBB[piece] & setMask[square]) {
					hit = true;
					cout << ' ' << ids[piece] << ' ';
					break;
				}
			}
			if (!hit) cout << " . ";
		}
		cout << endl;
	}
	cout << endl << "    a  b  c  d  e  f  g  h " << endl;
	cout << "side: ";
	if (position.side == WHITE) cout << "WHITE" << endl;
	else cout << "BLACK" << endl;
	cout << "castle permissions: ";
	int castlePermissions = position.castlePermissions;
	if (castlePermissions & WHITE_KING_CASTLE) cout << "K";
	if (castlePermissions & WHITE_QUEEN_CASTLE) cout << "Q";
	if (castlePermissions & BLACK_KING_CASTLE) cout << "k";
	if (castlePermissions & BLACK_QUEEN_CASTLE) cout << "q";
	cout << endl;
	cout << "en passant: " << numSquareToString(position.enPassantSquare) << endl;
	cout << "hash key: " << hex << position.hashKey << dec << endl;
}

int inputWaiting() {
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}
	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
		return dw;
	}
	else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
}

void readInput(SearchInfo &info) {
	int bytes;
	char input[256] = "", *endc;
	if (inputWaiting()) {
		info.stopped = true;
		do {
			bytes = _read(_fileno(stdin), input, 256);
		} while (bytes < 0);
		endc = strchr(input, '\n');
		if (endc) *endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4)) {
				info.quit = true;
			}
		}
		return;
	}
}