#include "board.h"
#include "search.h"
#include "move_generation.h"
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

const string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const int INPUT_BUFFER = 500 * 6;

void parseGo(Board &position, SearchInfo &info, char* input) {
	int movesToGo = 30;
	int time = -1;
	int increment = 0;
	info.timeSet = false;

	char *ptr = NULL;

	if (position.side == WHITE) {
		if ((ptr = strstr(input, "winc"))) increment = atoi(ptr + 5);
		if ((ptr = strstr(input, "wtime"))) time = atoi(ptr + 6);
	}
	else {
		if ((ptr = strstr(input, "binc"))) increment = atoi(ptr + 5);
		if ((ptr = strstr(input, "btime"))) time = atoi(ptr + 6);
	}

	ptr = strstr(input, "movestogo");
	if (ptr) movesToGo = atoi(ptr + 10);

	ptr = strstr(input, "movetime");
	if (ptr) {
		time = atoi(ptr + 9);
		movesToGo = 1;
	}

	ptr = strstr(input, "depth");
	if (ptr) {
		info.depth = atoi(ptr + 6);
	}
	else {
		info.depth = MAX_DEPTH;
	}

	info.startTime = (long) GetTickCount64();

	if (time != -1) {
		info.timeSet = true;
		time /= movesToGo;
		info.endTime = info.startTime + time + increment;
	}
	cout << "time: " << time;
	cout << " startTime: " << info.startTime;
	cout << " endTime: " << info.endTime;
	cout << " depth: " << info.depth;
	cout << " timeSet: " << info.timeSet << endl;
	searchPosition(position, info);
}

void parsePosition(Board &position, char* input) {
	input += 9;
	char *ptr = input;
	if (!strncmp(input, "startpos", 8)) {
		initBoard(position, STARTING_FEN);
	}
	else {
		ptr += 4;
		initBoard(position, ptr);
	}

	ptr = strstr(input, "moves");
	if (ptr != NULL) {
		ptr += 6;
		while (*ptr) {
			int move = parseMove(position, ptr);
			if (move < 0) break;
			makeMove(position, move);
			while (*ptr && *ptr != ' ') {
				ptr++;
			}
			ptr++;
		}
	}

	position.ply = 0;
	printBoard(position);
}

void uci() {
	Board position;
	SearchInfo info;
	initHashTable(position.hashTable);

	char input[INPUT_BUFFER];
	while (true) {
		cin.getline(input, INPUT_BUFFER);

		if (!strncmp(input, "isready", 7)) {
			cout << "readyok" << endl;
		}
		else if (!strncmp(input, "position", 8)) {
			parsePosition(position, input);
		}
		else if (!strncmp(input, "ucinewgame", 10)) {
			parsePosition(position, "startpos\n");
		}
		else if (!strncmp(input, "go", 2)) {
			parseGo(position, info, input);
		}
		else if (!strncmp(input, "quit", 4)) {
			info.quit = true;
		}
		else if (!strncmp(input, "uci", 3)) {
			cout << "id name Butter 1.0" << endl;
			cout << "id author Akhil Velagapudi" << endl;
			cout << "uciok" << endl;
		}
		if (info.quit) {
			break;
		}
	}
}