#pragma once
#include "Game.h"

class ZobristHashing {
private:
	const int ZOBRIST_TABLE_EMPTY_INDEX = 0;
	const int ZOBRIST_TABLE_WHITE_INDEX = 1;
	const int ZOBRIST_TABLE_BLACK_INDEX = 2;

	std::vector<std::vector<std::vector<uint64_t>>> zobristTable;
	
public:

	void initializeZobristTable(Board& board)
	{
		std::random_device rd;
		std::mt19937_64 generator(rd());
		std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());

		zobristTable.resize(board.arr.size());
		for (int i = 0; i < board.arr.size(); i++) {
			zobristTable[i].resize(board.arr[i].size());

		}

		for (int y = 0; y < board.arr.size(); y++) {
			for (int x = 0; x < board.arr[y].size(); x++) {
				zobristTable[y][x].resize(BOARD_FIELD_STATES);
			}
		}

		for (int i = 0; i < board.arr.size(); i++) {
			for (int j = 0; j < board.arr[i].size(); j++) {
				if (board.arr[i][j] == SPACE_BETWEN_FIELD || board.arr[i][j] == START_CHAR) {
					continue;
				}
				for (int k = 0; k < BOARD_FIELD_STATES; k++) {
					zobristTable[i][j][k] = distribution(generator);
				}
			}
		}
	}


	void computeZobristHash(Board& board)
	{
		for (int i = 0; i < board.arr.size(); i++) {
			for (int j = 0; j < board.arr[i].size(); j++) {
				char pawnColor = board.arr[i][j];
				if (pawnColor == EMPTY_SPACE_ON_BOARD) {
					board.boardHash ^= zobristTable[i][j][ZOBRIST_TABLE_EMPTY_INDEX];
				}
				else if (pawnColor == white) {
					board.boardHash ^= zobristTable[i][j][ZOBRIST_TABLE_WHITE_INDEX];
				}
				else if (pawnColor == black) {
					board.boardHash ^= zobristTable[i][j][ZOBRIST_TABLE_BLACK_INDEX];
				}
			}
		}
	}


	void updateBoardHash(Board& board, std::vector<Point> pointsThatChangedState)
	{
		for (Point& point : pointsThatChangedState) {
			if (board.arr[point.y][point.x] == EMPTY_SPACE_ON_BOARD) {
				board.boardHash ^= zobristTable[point.y][point.x][ZOBRIST_TABLE_EMPTY_INDEX];
			}
			else if (board.arr[point.y][point.x] == white) {
				board.boardHash ^= zobristTable[point.y][point.x][ZOBRIST_TABLE_WHITE_INDEX];
			}
			else if (board.arr[point.y][point.x] == black) {
				board.boardHash ^= zobristTable[point.y][point.x][ZOBRIST_TABLE_BLACK_INDEX];
			}
		}
	}
};