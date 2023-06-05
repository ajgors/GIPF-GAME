#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Point.h"
#include <iostream>
#include "MapCoordinates.h"
#include "utils.h"

const char START_CHAR = '+';
const char EMPTY_SPACE_ON_BOARD = '_';
const char SPACE_BETWEN_FIELD = ' ';
const int  BOARD_FIELD_STATES = 3;

enum pawn {
	white = 'W', black = 'B'
};

struct Board {
	
	void printMap() const
	{
		if (arr.size() == 0) {
			std::cout << "EMPTY_BOARD" << std::endl;
			return;
		}
		
		/*for (auto& y : arr) {
			for (auto& x : y) {
				std::cout << x;
			}
			std::cout << std::endl;
		}*/

		for (int y = 1; y < arr.size() - 1; y++) {
			for (int x = 0; x < arr[y].size(); x++) {
				if (arr[y][x] == START_CHAR) {
					x += 1;
					continue;
				}
				std::cout << arr[y][x];
			}
			std::cout << std::endl;
		}
	}

	
	Coordinates loadCoordinates()
	{
		Coordinates m_coordinates;

		//find first y that is not empty
		int startX = 0;
		int startY = 0;
		for (int y = 0; y < arr.size(); y++) {
			if (arr[y][0] != SPACE_BETWEN_FIELD) {
				startY = y;
				break;
			}
		}

		std::pair<int, int> shiftVec = { -1, 1 }; // y and x shift

		char currentLetter = 'a';
		int currentNumber = 1;
		int y = startY;
		int x = 0;
		while (true) {
			std::string index;
			index.push_back(currentLetter);
			index += std::to_string(currentNumber);
			m_coordinates.m_coordinates[index].y = y;
			m_coordinates.m_coordinates[index].x = x;
			m_coordinates.pointToCordinate[Point(y, x)] = index;

			//save starting points
			if (arr[y][x] == START_CHAR) {
				m_coordinates.startPoints.push_back(Point(y, x));
			}

			//increase current index number and check if exists
			y += shiftVec.first;
			x += shiftVec.second;
			currentNumber++;

			if (y < 0 || x >= arr[y].size()) {
				//go to next index letter
				currentLetter++;
				startY++;
				bool noYChange = false;
				if (startY >= arr.size()) {
					startY--;
					noYChange = true;
				}
				startX++;
				if (noYChange) {
					startX++;
				}

				if (startX >= arr[startY].size()) {
					break;
				}

				y = startY;
				x = startX;
				currentNumber = 1; //restart number counter
			}
		}
		return m_coordinates;
	}
	

	bool loadMap()
	{
		size_t maxSize = size * 2 - 1;
		std::vector<std::vector<char>> board;
		board.resize(maxSize);

		//reading map
		size_t linesToRead = maxSize;
		std::string line;
		size_t row = 0;
		std::getline(std::cin, line);
		size_t currentWidth = size;
		bool shrinking = false;
		while (linesToRead > 0) {
			std::getline(std::cin, line);
			linesToRead--;

			//check if length of map is correct
			std::string trimedAllWhiteSpaces = line;
			trimedAllWhiteSpaces.erase(std::remove_if(trimedAllWhiteSpaces.begin(), trimedAllWhiteSpaces.end(), [](unsigned char c) {
				return std::isspace(c);
				}), trimedAllWhiteSpaces.end());
			if (trimedAllWhiteSpaces.size() != currentWidth) {
				std::cout << "WRONG_BOARD_ROW_LENGTH" << std::endl;
				return false;
			}

			if (trimedAllWhiteSpaces.size() == maxSize) {
				shrinking = true;
			}

			if (shrinking) {
				currentWidth--;
			}
			else {
				currentWidth++;
			}
			
			size_t pos = line.find_last_not_of(" \t\n\r\f\v");
			if (pos != std::string::npos)
				line.erase(pos + 1);
			
			for (char c : line) {
				if (c == white) {
					whiteOnBoard++;
					mapSpaces++;
				}
				else if (c == black) {
					blackOnBoard++;
					mapSpaces++;
				}
				else if (c == EMPTY_SPACE_ON_BOARD) {
					mapSpaces++;
				}
				board[row].push_back(c);
			}
			row++;
		}

		//find free space in first row
		size_t spacesEmpty = 0;
		for (size_t i = 0; i < size; i++) {
			if (board[0][i] == SPACE_BETWEN_FIELD) {
				spacesEmpty++;
			}
		}

		arr.clear();

		//creating map with border
		arr.resize(maxSize + 2);

		//add free space
		for (size_t i = 0; i < spacesEmpty + 1; i++) {
			arr[0].push_back(SPACE_BETWEN_FIELD);
		}
		for (size_t i = 0; i < size + 1; i++) {
			arr[0].push_back(START_CHAR);
			arr[0].push_back(SPACE_BETWEN_FIELD);
		}
		arr[0].pop_back();

		//load map and add border
		for (size_t y = 0; y < board.size(); y++) {
			bool borderAded = false;
			for (size_t x = 0; x < board[y].size(); x++) {
				if (!borderAded && board[y][x] != SPACE_BETWEN_FIELD) {
					arr[y + 1].push_back(START_CHAR);
					arr[y + 1].push_back(SPACE_BETWEN_FIELD);
					borderAded = true;
				}
				arr[y + 1].push_back(board[y][x]);
			}
			arr[y + 1].push_back(SPACE_BETWEN_FIELD);
			arr[y + 1].push_back(START_CHAR);
		}

		//add free space
		for (size_t i = 0; i < spacesEmpty + 1; i++) {
			arr[arr.size() - 1].push_back(SPACE_BETWEN_FIELD);
		}

		for (size_t i = 0; i < size + 1; i++) {
			arr[arr.size() - 1].push_back(START_CHAR);
			arr[arr.size() - 1].push_back(SPACE_BETWEN_FIELD);
		}
		arr[arr.size() - 1].pop_back();

		return true;
	}


	std::vector<Point> generatePointsNear(int y, int x)
	{
		std::vector<Point> points;

		int currX = x + 2;
		int currY = y;
		if (currX < arr[currY].size() && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		currX = x - 2;
		currY = y;
		if (currX >= 0 && arr[currY][currX] != SPACE_BETWEN_FIELD && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		currX = x - 1;
		currY = y + 1;
		if (currY < arr.size() - 1 && currX >= 0 && arr[currY][currX] != SPACE_BETWEN_FIELD && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		currX = x + 1;
		currY = y + 1;
		if (currY < arr.size() - 1 && currX < arr[currY].size() && arr[currY][currX] != SPACE_BETWEN_FIELD && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		currX = x - 1;
		currY = y - 1;
		if (currY > 0 && currX >= 0 && currX < arr[currY].size() && arr[currY][currX] != SPACE_BETWEN_FIELD && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		currX = x + 1;
		currY = y - 1;
		if (currY > 0 && currX < arr[currY].size() && arr[currY][currX] != SPACE_BETWEN_FIELD && arr[currY][currX] != START_CHAR) {
			points.push_back(Point(currY, currX));
		}

		return points;
	}

	size_t size;
	int whiteOnBoard;
	int blackOnBoard;
	int mapSpaces = 0;
	std::vector<std::vector<char>> arr;
	uint64_t boardHash;

public:
	Board() : size(0), whiteOnBoard(0), blackOnBoard(0), mapSpaces(0), boardHash(0) {};

	Board(const Board& other) 
		: size(other.size), whiteOnBoard(other.whiteOnBoard), blackOnBoard(other.blackOnBoard), mapSpaces(other.mapSpaces), arr(other.arr), boardHash(other.boardHash)
	{
	}

	Board(Board&& other) : size(other.size), whiteOnBoard(other.whiteOnBoard), blackOnBoard(other.blackOnBoard), mapSpaces(other.mapSpaces), arr(std::move(other.arr)), boardHash(other.boardHash)
	{
	}

	Board& operator=(const Board& other)
	{
		size = other.size;
		whiteOnBoard = other.whiteOnBoard;
		blackOnBoard = other.blackOnBoard;
		mapSpaces = other.mapSpaces;
		arr = other.arr;
		boardHash = other.boardHash;
		return *this;
	}

	Board& operator=(Board&& other)
	{
		size = other.size;
		whiteOnBoard = other.whiteOnBoard;
		blackOnBoard = other.blackOnBoard;
		mapSpaces = other.mapSpaces;
		arr = std::move(other.arr);
		boardHash = other.boardHash;
		return *this;
	}

	bool operator==(const Board& other) const
	{
		return (boardHash == other.boardHash);
	}
};