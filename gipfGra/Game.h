#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <cctype>
#include <random>
#include "Point.h"
#include "utils.h"
#include "Board.h"
#include "MapCoordinates.h"
#include "ZobristHashing.h"
#include "GameState.h"


class Game {
private:
	struct lineToRemove {
		std::vector<std::string> points;
		pawn triggering;

		bool operator==(const lineToRemove& other) const
		{
			return points == other.points && triggering == other.triggering;
		}
	};

public:

	pawn getSecondPlayer(pawn current)
	{
		return current == white ? black : white;
	}


	void saveLineCoordinates(int startY, int startX, std::pair<int, int> shiftVec)
	{
		int currY = startY;
		int currX = startX;
		while (true) {

			if (currY < 0 || currY >= m_gameState.board.arr.size() || currX < 0 || currX >= m_gameState.board.arr[currY].size()) {
				break;
			}
			if (m_gameState.board.arr[currY][currX] == EMPTY_SPACE_ON_BOARD || m_gameState.board.arr[currY][currX] == SPACE_BETWEN_FIELD || m_gameState.board.arr[currY][currX] == START_CHAR) {
				break;
			}
			m_linesToRemove.back().points.push_back(m_coordinates.pointToCordinate[Point(currY, currX)]);

			currY += shiftVec.first;
			currX += shiftVec.second;
		}
	}


	bool findLine(int startY, int startX, std::pair<int, int> shiftVec, std::unordered_map<std::string, bool>& visited, bool removeSequence)
	{
		int currY = startY;
		int currX = startX;
		int whitePawnsCounter = 0;
		int blackPawnsCounter = 0;

		while (true) {
			if (currY < 0 || currY >= m_gameState.board.arr.size() || currX < 0 || currX >= m_gameState.board.arr[currY].size()) {
				return false;
			}

			if (m_gameState.board.arr[currY][currX] == white) {
				whitePawnsCounter++;
				blackPawnsCounter = 0;
			}
			if (m_gameState.board.arr[currY][currX] == black) {
				blackPawnsCounter++;
				whitePawnsCounter = 0;
			}
			if (m_gameState.board.arr[currY][currX] == EMPTY_SPACE_ON_BOARD) {
				blackPawnsCounter = 0;
				whitePawnsCounter = 0;
				startY = currY + shiftVec.first;
				startX = currX + shiftVec.second;
			}

			std::string indexWithVector;
			indexWithVector += std::to_string(currY);
			indexWithVector += std::to_string(currX);
			indexWithVector += std::to_string(shiftVec.first);
			indexWithVector += std::to_string(shiftVec.second);

			if (!visited[indexWithVector]) {
				visited[indexWithVector] = true;
			}
			else {
				return false;
			}

			if (whitePawnsCounter == m_triggering || blackPawnsCounter == m_triggering) {
				if (removeSequence) {
					m_linesToRemove.push_back(lineToRemove());
					if (whitePawnsCounter == m_triggering) {
						m_linesToRemove.back().triggering = white;
					}
					else {
						m_linesToRemove.back().triggering = black;
					}
					saveLineCoordinates(startY, startX, shiftVec);
				}
				return true;
			}
			currY += shiftVec.first;
			currX += shiftVec.second;
		}
		return false;
	}


	void removeLines()
	{
		//deleting pawns from board
		m_gameState.whiteRemoved = 0;
		m_gameState.blackRemoved = 0;
		pawn secondPlayer{};
		for (lineToRemove& line : m_linesToRemove) {
			secondPlayer = getSecondPlayer(line.triggering);

			for (std::string& index : line.points) {
				Point p = m_coordinates.m_coordinates[index];
				if (m_gameState.board.arr[p.y][p.x] == line.triggering) {
					if (line.triggering == white) {
						m_pointsThatChangedState.push_back(p);
						m_gameState.whiteReserve++;
						m_gameState.board.whiteOnBoard--;
					}
					else {
						m_pointsThatChangedState.push_back(p);
						m_gameState.blackReserve++;
						m_gameState.board.blackOnBoard--;
					}
					m_gameState.board.arr[p.y][p.x] = EMPTY_SPACE_ON_BOARD;
				}
				else if (m_gameState.board.arr[p.y][p.x] == secondPlayer) {
					if (secondPlayer == white) {
						m_pointsThatChangedState.push_back(p);
						m_gameState.board.whiteOnBoard--;
						m_gameState.whiteRemoved++;
					}
					else {
						m_pointsThatChangedState.push_back(p);
						m_gameState.board.blackOnBoard--;
						m_gameState.blackRemoved++;
					}
					m_gameState.board.arr[p.y][p.x] = EMPTY_SPACE_ON_BOARD;
				}
			}
		}
		m_linesToRemove.clear();
	}


	int checkForSequencesInStartMap(bool removeSequence)
	{
		int count = 0;

		if (m_gameState.board.whiteOnBoard >= m_triggering || m_gameState.board.blackOnBoard >= m_triggering) {

			std::unordered_map<std::string, bool> visited;

			for (int mapY = 1; mapY < m_gameState.board.arr.size() - 1; mapY++) {
				for (int mapX = 0; mapX < m_gameState.board.arr[mapY].size(); mapX++) {
					if (m_gameState.board.arr[mapY][mapX] == SPACE_BETWEN_FIELD || m_gameState.board.arr[mapY][mapX] == START_CHAR) continue;
					else {
						std::vector<Point> nearPoints = m_gameState.board.generatePointsNear(mapY, mapX);
						for (Point& nearPoint : nearPoints) {
							std::pair<int, int> shiftVec = { nearPoint.y - mapY, nearPoint.x - mapX };
							if (findLine(mapY, mapX, shiftVec, visited, removeSequence)) {
								count++;
							}
						}
					}
				}
			}
		}
		return count;
	}


	Point goToStartOfLine(Point& p, int arr[])
	{
		int currY = p.y;
		int currX = p.x;

		while (true) {
			if (m_gameState.board.arr[currY][currX] == START_CHAR) {
				return { currY - arr[0], currX - arr[1] };
			}

			currY += arr[0];
			currX += arr[1];
		}
		return { 0,0 };
	}

	
	void findSequences(bool removeSequence)
	{
		m_linesToRemove.clear();
		if (m_gameState.board.whiteOnBoard >= m_triggering || m_gameState.board.blackOnBoard >= m_triggering) {
			std::unordered_map<std::string, bool> visited;
			for (Point& p : m_pointsThatChangedState) {
				int arr[][3] = { {0, -2}, {-1, -1}, {-1, 1} };
				for (int i = 0; i < 3; i++) {
					Point startPoint = goToStartOfLine(p, arr[i]);
					std::pair<int, int> shiftVec = { -arr[i][0], -arr[i][1]};
					findLine(startPoint.y, startPoint.x, shiftVec, visited, removeSequence);
				}
			}
		}
	}


	void clearGame()
	{
		m_gameState = {};
		m_linesToRemove.clear();
		m_ambigiousLinesCurrentPlayer.clear();
		m_ambigiousLinesSecondPlayer.clear();
		m_ambigiousToRemove = {};
	}


	bool checkForMapCorrectness()
	{
		//chandle wrong number of pawn in input
		if (m_gameState.board.whiteOnBoard + m_gameState.whiteReserve > m_GW) {
			std::cout << "WRONG_WHITE_PAWNS_NUMBER" << std::endl;
			m_gameState.board.arr.clear();
			return false;
		}
		if (m_gameState.board.blackOnBoard + m_gameState.blackReserve > m_GB) {
			std::cout << "WRONG_BLACK_PAWNS_NUMBER" << std::endl;
			m_gameState.board.arr.clear();
			return false;
		}

		int c = checkForSequencesInStartMap(false); //sequence in start map
		if (c == 1) {
			std::cout << "ERROR_FOUND_" << c << "_ROW_OF_LENGTH_K" << std::endl << std::endl;
			m_gameState.board.arr.clear();
			return false;
		}
		else if (c > 1) {
			std::cout << "ERROR_FOUND_" << c << "_ROWS_OF_LENGTH_K" << std::endl << std::endl;
			m_gameState.board.arr.clear();
			return false;
		}

		isGameOver();
		isDeadLock();

		std::cout << "BOARD_STATE_OK" << std::endl;
		return true;
	}


	void loadGame()
	{
		clearGame();
		std::cin >> m_gameState.board.size;
		std::cin >> m_triggering;
		std::cin >> m_GW;
		std::cin >> m_GB;
		std::cin >> m_gameState.whiteReserve;
		std::cin >> m_gameState.blackReserve;
		char player;
		std::cin >> player;
		m_gameState.currentPlayer = player == white ? white : black;

		if (m_gameState.board.loadMap()) {
			if (checkForMapCorrectness()) {
				m_coordinates = m_gameState.board.loadCoordinates();
				zobristHashing.initializeZobristTable(m_gameState.board);
				zobristHashing.computeZobristHash(m_gameState.board);
			}
		}
	}


	bool isMoveAmbigious()
	{
		//check for ambiguous move
		if (m_ambigiousToRemove.points.size() == 0) {
			std::unordered_map<std::string, bool> visited;
			for (lineToRemove& line : m_linesToRemove) {
				for (std::string& point : line.points) {
					std::string key = point;
					key += (char)line.triggering;
					if (visited[key] == false) {
						visited[key] = true;
					}
					else {
						m_gameState.gameStatus = badMove;
						return true;
					}
				}
			}
			return false;
		}
		return true;
	}


	bool ambigiousMoveResolve()
	{
		//check if ambigious resolver is correct
		if (m_ambigiousToRemove.points.size() > 0) {
			std::sort(m_ambigiousToRemove.points.begin(), m_ambigiousToRemove.points.end());
			for (lineToRemove& line : m_linesToRemove) {
				std::sort(line.points.begin(), line.points.end());
				bool contains = utils::containsAllElements(line.points, m_ambigiousToRemove.points);

				if (m_ambigiousToRemove.triggering != line.triggering) {
					if (contains) {
						std::cout << "WRONG_COLOR_OF_CHOSEN_ROW" << std::endl;
						return false;
					}
				}

				if (contains) {
					//check if there is k-2 pawns between first and last pawn
					size_t firstIndex = 0;
					size_t secondIndex = 0;
					int i = 0;
					for (std::string& cord : line.points) {
						if (cord == m_ambigiousToRemove.points[0]) {
							firstIndex = i;
						}
						else if (cord == m_ambigiousToRemove.points.back()) {
							secondIndex = i;
						}
						i++;
					}

					if (secondIndex - firstIndex - 1 != m_triggering - 2) {
						std::cout << "WRONG_INDEX_OF_CHOSEN_ROW" << std::endl;
						return false;
					}
					else {

						lineToRemove ambiguousLineToRemove = line;

						m_linesToRemove.clear();
						m_linesToRemove.push_back(ambiguousLineToRemove);
						removeLines();

						checkForSequencesInStartMap(true);
						saveAmbigiousLines();

						//add those that are not ambigious
						std::vector<lineToRemove> newToRemove;
						for (lineToRemove& line : m_linesToRemove) {
							auto iteratorCurrent = std::find(m_ambigiousLinesCurrentPlayer.begin(), m_ambigiousLinesCurrentPlayer.end(), line);
							auto iteratorSecond = std::find(m_ambigiousLinesSecondPlayer.begin(), m_ambigiousLinesSecondPlayer.end(), line);

							if (iteratorCurrent == m_ambigiousLinesCurrentPlayer.end() && iteratorSecond == m_ambigiousLinesSecondPlayer.end()) {
								newToRemove.push_back(line);
							}
						}
						m_linesToRemove = newToRemove;
						removeLines();

						return true;
					}
				}
			}
			std::cout << "ERROR IN AMBIGIOUS RESOLVER" << std::endl;
			return false;
		}
		return false;
	}


	void remove(std::string& input) {
		if (m_gameState.gameStatus == needToRemove) {
			size_t spaceIndex = input.find_first_of(" ");
			size_t secondSpaceIndex = input.find_first_of(" ", spaceIndex + 1);

			if (secondSpaceIndex != std::string::npos) {
				std::string coordinateIndex;
				move move;
				move.from = input.substr(spaceIndex + 1, secondSpaceIndex - spaceIndex - 1);
				move.to = input.substr(secondSpaceIndex + 1);

				checkForSequencesInStartMap(true);

				size_t firstIndex = 0;
				size_t secondIndex = 0;
				int i = 0;

				for (lineToRemove& line : m_linesToRemove) {
					auto iteratorFrom = std::find(line.points.begin(), line.points.end(), move.from);
					auto iteratorTo = std::find(line.points.begin(), line.points.end(), move.to);
					std::sort(line.points.begin(), line.points.end());

					if (iteratorFrom != line.points.end() && iteratorTo != line.points.end()) {
						for (std::string& coordinate : line.points) {
							if (coordinate == move.from) {
								firstIndex = i;
							}
							else if (coordinate == move.to) {
								secondIndex = i;
							}
							i++;
						}

						if (secondIndex - firstIndex - 1 != m_triggering - 2) {
							std::cout << "WRONG_INDEX_OF_CHOSEN_ROW" << std::endl;
						}
						else {
							lineToRemove toRemove = line;
							m_linesToRemove.clear();
							m_linesToRemove.push_back(toRemove);
							removeLines();
							std::cout << "REMOVED CORRECTLY" << std::endl;
							break;
						}
					}
				}
			}
			std::cout << "ERROR IN REMOVE" << std::endl;
		}
	}

	
	void proccessPlayerMove(move move) {
		if (m_gameState.gameStatus == inProgress || m_gameState.gameStatus == badMove) {
			doPlayerMove(move);
		}
		else if (m_gameState.gameStatus == needToRemove) {
			std::cout << "YOU NEED TO REMOVE PAWNS THAT CROSS" << std::endl;
		}
	}

	
	bool doPlayerMove(const move& move)
	{
		Point pointFrom = m_coordinates.m_coordinates[move.from];
		Point pointTo = m_coordinates.m_coordinates[move.to];
		m_gameState.lastMove = move;

		//handle wrong coordinates( point is 0 0)
		if (pointFrom.y == 0 && pointFrom.x == 0) {
			std::cout << "BAD_MOVE_" << move.from << "_IS_WRONG_INDEX" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}
		else if (pointTo.y == 0 && pointTo.x == 0) {
			std::cout << "BAD_MOVE_" << move.to << "_IS_WRONG_INDEX" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}

		std::pair<int, int> shiftVec = { pointTo.y - pointFrom.y, pointTo.x - pointFrom.x };

		if (shiftVec.second > 2 || shiftVec.first > 1) {
			std::cout << "UNKNOWN_MOVE_DIRECTION" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}

		//handle wrong starting point
		if (m_gameState.board.arr[pointFrom.y][pointFrom.x] != START_CHAR) {
			std::cout << "BAD_MOVE_" << move.from << "_IS_WRONG_STARTING_FIELD" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}
		if (m_gameState.board.arr[pointTo.y][pointTo.x] != '_' && m_gameState.board.arr[pointTo.y][pointTo.x] != white && m_gameState.board.arr[pointTo.y][pointTo.x] != black) {
			std::cout << "BAD_MOVE_" << move.to << "_IS_WRONG_DESTINATION_FIELD" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}

		//check if line is full
		if (isLineFull(pointTo, shiftVec)) {
			std::cout << "BAD_MOVE_ROW_IS_FULL" << std::endl;
			m_gameState.gameStatus = badMove;
			return false;
		}

		if (isDeadLock()) {
			return false;
		}

		gameState copy = m_gameState;

		shiftPawns(pointTo, shiftVec);

		addPlayerPawnToMap(pointTo);
		updatePawnNumbers();

		findSequences(true);

		if (ambigiousMoveResolve()) {
			changePlayer();
			isGameOver();
			zobristHashing.updateBoardHash(m_gameState.board, m_pointsThatChangedState);
			std::cout << "MOVE_COMMITTED" << std::endl;
			if (m_ambigiousLinesSecondPlayer.size() > 0) {
				std::cout << "OPPOSITE PLAYER NEEDS TO REMOVE PAWNS" << std::endl;
				m_gameState.gameStatus = needToRemove;
			}
			return true;
		}
		else if (!isMoveAmbigious()) {
			removeLines();
			changePlayer();
			isGameOver();
			zobristHashing.updateBoardHash(m_gameState.board, m_pointsThatChangedState);
			std::cout << "MOVE_COMMITTED" << std::endl;
			return true;
		}
		else {
			if (m_ambigiousToRemove.points.size() == 0 && isMoveAmbigious()) {
				std::cout << "ERROR AMBIGIOUS MOVE" << std::endl;
				saveAmbigiousLines();
				restorePawnNumbers();
			}
			m_gameState = std::move(copy);
			return false;
		}
	}


	void saveAmbigiousLines() {
		//check for ambiguous move
		std::unordered_map<std::string, bool> visited;
		std::unordered_map<std::string, bool> contains;
		std::vector<std::string> pointsToCheck;
		for (lineToRemove& line : m_linesToRemove) {
			for (std::string& point : line.points) {
				if (visited[point] == false) {
					visited[point] = true;
				}
				else {
					if (contains[point] == false) {
						pointsToCheck.push_back(point);
						contains[point] = true;
					}
				}
			}
		}

		m_ambigiousLinesCurrentPlayer.clear();
		m_ambigiousLinesSecondPlayer.clear();

		pawn secondPlayer = getSecondPlayer(m_gameState.currentPlayer);

		for (std::string& pointToCheck : pointsToCheck) {
			for (lineToRemove& line : m_linesToRemove) {
				for (std::string linePoint : line.points) {
					if (pointToCheck == linePoint && line.triggering == m_gameState.currentPlayer) {
						m_ambigiousLinesCurrentPlayer.push_back(line);
						break;
					}
					else if (pointToCheck == linePoint && line.triggering == secondPlayer) {
						m_ambigiousLinesSecondPlayer.push_back(line);
						break;
					}
				}
			}
		}
	}


	bool botMove(Point& pointFrom, Point& pointTo)
	{
		m_gameState.lastMove = { m_coordinates.pointToCordinate[pointFrom], m_coordinates.pointToCordinate[pointTo] };

		std::pair<int, int> shiftVec = { pointTo.y - pointFrom.y, pointTo.x - pointFrom.x };

		if (isLineFull(pointFrom, shiftVec)) {
			return false;
		}

		shiftPawns(pointTo, shiftVec);

		addPlayerPawnToMap(pointTo);

		updatePawnNumbers();

		findSequences(true);

		if (m_ambigiousLinesCurrentPlayer.size() > 0) {
			std::vector<lineToRemove> newToRemove;
			std::vector<lineToRemove> allAmbigiousCurrentPlayer;
			std::vector<lineToRemove> allAmbigiousSecondPlayer;
			std::swap(allAmbigiousCurrentPlayer, m_ambigiousLinesCurrentPlayer);
			std::swap(allAmbigiousSecondPlayer, m_ambigiousLinesSecondPlayer);

			saveAmbigiousLines();

			std::swap(allAmbigiousCurrentPlayer, m_ambigiousLinesCurrentPlayer);
			std::swap(allAmbigiousSecondPlayer, m_ambigiousLinesSecondPlayer);

			newToRemove.push_back(m_ambigiousLinesCurrentPlayer.back());

			//addlines that are not ambigious
			for (lineToRemove& line : m_linesToRemove) {
				auto it = std::find(allAmbigiousCurrentPlayer.begin(), allAmbigiousCurrentPlayer.end(), line);
				auto it2 = std::find(allAmbigiousSecondPlayer.begin(), allAmbigiousSecondPlayer.end(), line);

				if (it == allAmbigiousCurrentPlayer.end() && it2 == allAmbigiousSecondPlayer.end()) {
					newToRemove.push_back(line);
				}
			}

			if (m_ambigiousLinesSecondPlayer.size() <= 0) {
				m_ambigiousLinesCurrentPlayer.pop_back();
			}
			else {
				newToRemove.push_back(m_ambigiousLinesSecondPlayer.back());
				m_ambigiousLinesSecondPlayer.pop_back();
			}

			m_linesToRemove = newToRemove;
		}
		else if (m_ambigiousLinesSecondPlayer.size() > 0) {
			std::vector<lineToRemove> newToRemove;
			std::vector<lineToRemove> allAmbigiousSecondPlayer;
			std::swap(allAmbigiousSecondPlayer, m_ambigiousLinesSecondPlayer);

			saveAmbigiousLines();

			std::swap(allAmbigiousSecondPlayer, m_ambigiousLinesSecondPlayer);

			newToRemove.push_back(m_ambigiousLinesSecondPlayer.back());

			//addlines that are not ambigious
			for (lineToRemove& line : m_linesToRemove) {
				auto it = std::find(allAmbigiousSecondPlayer.begin(), allAmbigiousSecondPlayer.end(), line);

				if (it == allAmbigiousSecondPlayer.end()) {
					newToRemove.push_back(line);
				}
			}

			m_ambigiousLinesSecondPlayer.pop_back();

			m_linesToRemove = newToRemove;
		}
		else {
			if (isMoveAmbigious()) {
				saveAmbigiousLines();
				m_linesToRemove.clear();
				restorePawnNumbers();
				return false;
			}
		}

		removeLines();
		changePlayer();
		isGameOver();
		zobristHashing.updateBoardHash(m_gameState.board, m_pointsThatChangedState);

		return true;
	}


	void addPlayerPawnToMap(Point pointTo)
	{
		if (m_gameState.board.arr[pointTo.y][pointTo.x] != black && m_gameState.currentPlayer == black) {
			m_pointsThatChangedState.push_back(pointTo);
		}
		else if (m_gameState.board.arr[pointTo.y][pointTo.x] != white && m_gameState.currentPlayer == white) {
			m_pointsThatChangedState.push_back(pointTo);
		}

		m_gameState.board.arr[pointTo.y][pointTo.x] = m_gameState.currentPlayer;
	}


	void shiftPawns(Point startPoint, std::pair<int, int>& shiftVec)
	{
		std::vector<Point> pointsToMove;
		int currY = startPoint.y;
		int currX = startPoint.x;

		//add pawns to shift
		while (true) {
			if (currY < 1 || currY >= m_gameState.board.arr.size() - 1 || currX < 0 || currX >= m_gameState.board.arr[currY].size() - 1) {
				break;
			}
			if (m_gameState.board.arr[currY][currX] != EMPTY_SPACE_ON_BOARD && m_gameState.board.arr[currY][currX] != START_CHAR) {
				pointsToMove.push_back(Point(currY, currX));
			}
			else {
				break;
			}
			currY += shiftVec.first;
			currX += shiftVec.second;
		}

		m_pointsThatChangedState.clear();
		//do shift
		for (std::vector<Point>::reverse_iterator i = pointsToMove.rbegin(); i != pointsToMove.rend(); ++i) {
			if (m_gameState.board.arr[i->y + shiftVec.first][i->x + shiftVec.second] != m_gameState.board.arr[i->y][i->x]) {
				m_pointsThatChangedState.push_back({ i->y + shiftVec.first, i->x + shiftVec.second });
			}
			m_gameState.board.arr[i->y + shiftVec.first][i->x + shiftVec.second] = m_gameState.board.arr[i->y][i->x];
		}
	}


	void updatePawnNumbers()
	{
		if (m_gameState.currentPlayer == white) {
			m_gameState.board.whiteOnBoard++;
			m_gameState.whiteReserve--;
		}
		else {
			m_gameState.board.blackOnBoard++;
			m_gameState.blackReserve--;
		}
	}


	void restorePawnNumbers() {
		if (m_gameState.currentPlayer == white) {
			m_gameState.board.whiteOnBoard--;
			m_gameState.whiteReserve++;
		}
		else {
			m_gameState.board.blackOnBoard--;
			m_gameState.blackReserve++;
		}
	}


	bool isDeadLock()
	{
		if (m_gameState.board.blackOnBoard + m_gameState.board.whiteOnBoard < m_gameState.board.mapSpaces) {
			return false;
		}
		if (m_gameState.currentPlayer == white) {
			m_gameState.gameStatus = blackWin;
		}
		else {
			m_gameState.gameStatus = whiteWin;
		}

		return true;
	}


	bool isGameOver()
	{
		//after move is made and player is changed
		if (m_gameState.whiteReserve <= 0 && m_gameState.currentPlayer == white) {
			m_gameState.gameStatus = blackWin;
			return true;
		}
		else if (m_gameState.blackReserve <= 0 && m_gameState.currentPlayer == black) {
			m_gameState.gameStatus = whiteWin;
			return true;
		}
		m_gameState.gameStatus = inProgress;
		return false;
	}


	bool isLineFull(Point& point, std::pair<int, int>& shiftVec)
	{
		int currY = point.y;
		int currX = point.x;
		while (true) {
			if (currY < 0 || currY >= m_gameState.board.arr.size() || currX < 0 || currX >= m_gameState.board.arr[currY].size()) {
				return true;
			}
			if (m_gameState.board.arr[currY][currX] == EMPTY_SPACE_ON_BOARD) {
				return false;
			}
			currY += shiftVec.first;
			currX += shiftVec.second;
		}
	}


	void changePlayer()
	{
		m_gameState.currentPlayer = getSecondPlayer(m_gameState.currentPlayer);
	}


	void printInformations(const gameState& gameState)
	{
		if (m_gameState.board.arr.size() > 0) {
			std::cout << m_gameState.board.size << " " << m_triggering << " " << m_GW << " " << m_GB << std::endl;
			std::cout << gameState.whiteReserve << " " << gameState.blackReserve << " " << (char)gameState.currentPlayer << std::endl;
		}
	}


	//generates all possible game states that can happen from current player moves
	std::unordered_set<gameState> generateAllPossibleMoves(const gameState& startState)
	{
		gameState gameInfoCopy = startState;
		m_gameState = gameInfoCopy;

		std::unordered_set<gameState> posGameState;
		for (Point& startPoint : m_coordinates.startPoints) {
			std::vector<Point> nearPoints = gameInfoCopy.board.generatePointsNear(startPoint.y, startPoint.x);
			for (Point& pointNear : nearPoints) {

				if (!botMove(startPoint, pointNear)) {
					int size = 0;
					int secPlayerSize = 0;
					if (m_ambigiousLinesCurrentPlayer.size() > 0) {
						size = m_ambigiousLinesCurrentPlayer.size();
						if (m_ambigiousLinesSecondPlayer.size() > 0) {
							secPlayerSize = m_ambigiousLinesSecondPlayer.size();
						}
					}
					else if (m_ambigiousLinesSecondPlayer.size() > 0) {
						size = m_ambigiousLinesSecondPlayer.size();
					}

					m_gameState = gameInfoCopy;
					std::vector<lineToRemove> back = m_ambigiousLinesSecondPlayer;

					for (int i = 0; i < size; i++) {
						if (secPlayerSize > 0) {
							for (int j = 0; j < secPlayerSize; j++) {
								botMove(startPoint, pointNear);
								posGameState.insert(std::move(m_gameState));
								m_gameState = gameInfoCopy;
							}
							m_ambigiousLinesSecondPlayer = back;
							m_ambigiousLinesCurrentPlayer.pop_back();
						}
						else {
							botMove(startPoint, pointNear);
							posGameState.insert(std::move(m_gameState));
							m_gameState = gameInfoCopy;
						}
					}
					m_ambigiousLinesCurrentPlayer.clear();
					m_ambigiousLinesSecondPlayer.clear();
					continue;
				}
				posGameState.insert(std::move(m_gameState));
				m_gameState = gameInfoCopy;
			}
		}
		return posGameState;
	}


	//generates all possible game states that can happen from current player moves if wining moves exists prints only it
	std::unordered_set<gameState> generateAllPossibleMovesEXT(const gameState& startState)
	{
		gameState gameInfoCopy = startState;
		m_gameState = gameInfoCopy;

		std::unordered_set<gameState> posGameState;
		for (Point& startPoint : m_coordinates.startPoints) {
			std::vector<Point> nearPoints = gameInfoCopy.board.generatePointsNear(startPoint.y, startPoint.x);
			for (Point& pointNear : nearPoints) {

				if (!botMove(startPoint, pointNear)) {
					int size = 0;
					int secPlayerSize = 0;
					if (m_ambigiousLinesCurrentPlayer.size() > 0) {
						size = m_ambigiousLinesCurrentPlayer.size();
						if (m_ambigiousLinesSecondPlayer.size() > 0) {
							secPlayerSize = m_ambigiousLinesSecondPlayer.size();
						}
					}
					else if (m_ambigiousLinesSecondPlayer.size() > 0) {
						size = m_ambigiousLinesSecondPlayer.size();
					}

					m_gameState = gameInfoCopy;
					std::vector<lineToRemove> back = m_ambigiousLinesSecondPlayer;

					for (int i = 0; i < size; i++) {
						if (secPlayerSize > 0) {
							for (int j = 0; j < secPlayerSize; j++) {
								botMove(startPoint, pointNear);

								if (gameInfoCopy.currentPlayer == white && m_gameState.gameStatus == whiteWin || isDeadLock()) {
									posGameState.clear();
									posGameState.insert(std::move(m_gameState));
									m_gameState = gameInfoCopy;
									return posGameState;
								}
								else if (gameInfoCopy.currentPlayer == black && m_gameState.gameStatus == blackWin || isDeadLock()) {
									posGameState.clear();

									posGameState.insert(std::move(m_gameState));
									m_gameState = gameInfoCopy;
									return posGameState;
								}
								posGameState.insert(std::move(m_gameState));
								m_gameState = gameInfoCopy;
							}
							m_ambigiousLinesSecondPlayer = back;
							m_ambigiousLinesCurrentPlayer.pop_back();
						}
						else {
							botMove(startPoint, pointNear);
							if (gameInfoCopy.currentPlayer == white && m_gameState.gameStatus == whiteWin || isDeadLock()) {
								posGameState.clear();
								posGameState.insert(std::move(m_gameState));
								m_gameState = gameInfoCopy;
								return posGameState;
							}
							else if (gameInfoCopy.currentPlayer == black && m_gameState.gameStatus == blackWin || isDeadLock()) {
								posGameState.clear();
								posGameState.insert(std::move(m_gameState));
								m_gameState = gameInfoCopy;
								return posGameState;
							}
							posGameState.insert(std::move(m_gameState));
							m_gameState = gameInfoCopy;
						}
					}
					m_ambigiousLinesCurrentPlayer.clear();
					m_ambigiousLinesSecondPlayer.clear();
					continue;
				}

				if (gameInfoCopy.currentPlayer == white && m_gameState.gameStatus == whiteWin || isDeadLock()) {
					posGameState.clear();
					posGameState.insert(std::move(m_gameState));
					m_gameState = gameInfoCopy;
					return posGameState;
				}
				else if (gameInfoCopy.currentPlayer == black && m_gameState.gameStatus == blackWin || isDeadLock()) {
					posGameState.clear();
					posGameState.insert(std::move(m_gameState));
					m_gameState = gameInfoCopy;
					return posGameState;
				}
				posGameState.insert(std::move(m_gameState));
				m_gameState = gameInfoCopy;
			}
		}
		return posGameState;
	}


	void printPossibleMovesNum(const std::unordered_set<gameState>& posMoves)
	{
		std::cout << posMoves.size() << "_UNIQUE_MOVES" << std::endl;
	}


	void printPossibleMoves(const std::unordered_set<gameState>& posMoves)
	{
		for (const gameState& gameState : posMoves) {
			printInformations(gameState);
			gameState.board.printMap();
		}
	}


	void printGameEndInfo()
	{
		if (m_gameState.gameStatus == inProgress) {
			std::cout << "GAME_IN_PROGRESS" << std::endl;
		}
		else if (m_gameState.gameStatus == whiteWin) {
			std::cout << "THE_WINNER_IS_WHITE" << std::endl;
		}
		else if (m_gameState.gameStatus == blackWin) {
			std::cout << "THE_WINNER_IS_BLACK" << std::endl;
		}
		else if (m_gameState.gameStatus == deadLock) {
			std::cout << "dead_lock" << std::endl;
		}
	}

public:
	gameState m_gameState;
	int m_triggering;
	int m_GW;
	int m_GB;
	std::vector<lineToRemove> m_linesToRemove;
	lineToRemove m_ambigiousToRemove = {};
	std::vector<lineToRemove> m_ambigiousLinesCurrentPlayer;
	std::vector<lineToRemove> m_ambigiousLinesSecondPlayer;
	Coordinates m_coordinates;
	std::vector<Point> m_pointsThatChangedState;
	ZobristHashing zobristHashing;
};
