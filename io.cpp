#include "board.h"
#include "bitboards.h"
#include "move_generation.h"
#include <iostream>
#include <string>

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
	cout << "white value: " << position.material[WHITE] << endl;
	cout << "black value: " << position.material[BLACK] << endl;
	cout << "hash key: " << hex << position.hashKey << dec << endl;
}