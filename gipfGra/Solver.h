#pragma once
#include "Game.h"
#include <chrono>

class Solver {
private:
	Game& game;

public:
	Solver(Game& game) : game(game) {}

	//https://epublications.vu.lt/object/elaba:81917643/MAIN
	bool winningSequenceExists(int maxDepth)
	{
		std::unordered_set<gameState> gameStates = game.generateAllPossibleMovesEXT(game.m_gameState);
		pawn playerToCheck = game.m_gameState.currentPlayer;

		gameState info = game.m_gameState;
		std::unordered_map<gameState, double> transpositionTable;

		double bestScore;

		for (const gameState& gameState : gameStates) {
			bestScore = minimax(gameState, playerToCheck, maxDepth, 0, false, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), transpositionTable);
			if (bestScore == std::numeric_limits<double>::max()) {
				std::cout << "EXIST" << std::endl;
				return true;
			}
		}
		game.m_gameState = std::move(info);
		std::cout << "DOES NOT EXIST" << std::endl;
		return false;
	}


	double evaluateExist(pawn playerTocheck, const gameState& currentGameState) {
		double wr = 0.75;
		double br = 0.75;
		double wb = 0.85;
		double bb = 0.85;
		double wc = 1;
		double bc = 1;

		if (currentGameState.currentPlayer == playerTocheck && currentGameState.currentPlayer == black) { //maximizing player
			if (currentGameState.gameStatus == blackWin) {
				return std::numeric_limits<double>::max();
			}
			else if (currentGameState.gameStatus == whiteWin) {
				return std::numeric_limits<double>::lowest();
			}
			wr *= -currentGameState.whiteReserve;
			br *= currentGameState.blackReserve;
			wb *= -currentGameState.board.whiteOnBoard;
			bb *= currentGameState.board.blackOnBoard;
			wc *= currentGameState.whiteRemoved;
			bc *= -currentGameState.blackRemoved;
			return wr + br + wb + bb + wc + bc;
		}
		else if (currentGameState.currentPlayer == playerTocheck && currentGameState.currentPlayer == white) { //maximizing player
			if (currentGameState.gameStatus == whiteWin) { //jeœli poszukiwany gracz wygra³ to zwracamy najwiêksz¹ mo¿liw¹ wartoœæ
				return std::numeric_limits<double>::max();
			}
			else if (currentGameState.gameStatus == blackWin) {
				return std::numeric_limits<double>::lowest();
			}
			wr *= currentGameState.whiteReserve;
			br *= -currentGameState.blackReserve;
			wb *= currentGameState.board.whiteOnBoard;
			bb *= -currentGameState.board.blackOnBoard;
			wc *= -currentGameState.whiteRemoved;
			bc *= currentGameState.blackRemoved;
			return wr + br + wb + bb + wc + bc;
		}
		else if (currentGameState.currentPlayer == white && currentGameState.currentPlayer != playerTocheck) { //minimalizing player
			if (currentGameState.gameStatus == whiteWin) { //jesli przeciwnik wygra³ to zwracamy najmniejsz¹ mo¿liw¹ wartoœæ
				return std::numeric_limits<double>::lowest();
			}
			else if (currentGameState.gameStatus == blackWin) { //jeœli przeciwnik przegra³ to zwracamy najwiêksz¹ mo¿liw¹ wartoœæ
				return std::numeric_limits<double>::max();
			}
			wr *= currentGameState.whiteReserve;
			br *= -currentGameState.blackReserve;
			wb *= currentGameState.board.whiteOnBoard;
			bb *= -currentGameState.board.blackOnBoard;
			wc *= -currentGameState.whiteRemoved;
			bc *= currentGameState.blackRemoved;
			return wr + br + wb + bb + wc + bc;
		}
		else if (currentGameState.currentPlayer == black && currentGameState.currentPlayer != playerTocheck) { //minimalizing player
			if (currentGameState.gameStatus == blackWin) {
				return std::numeric_limits<double>::lowest();
			}
			else if (currentGameState.gameStatus == whiteWin) {
				return std::numeric_limits<double>::max();
			}
			wr *= -currentGameState.whiteReserve;
			br *= +currentGameState.blackReserve;
			wb *= -currentGameState.board.whiteOnBoard;
			bb *= +currentGameState.board.blackOnBoard;
			wc *= currentGameState.whiteRemoved;
			bc *= -currentGameState.blackRemoved;
			return wr + br + wb + bb + wc + bc;
		}
		return wr + br + wb + bb + wc + bc;
	}


	double minimax(const gameState& gameStatus, pawn playerTocheck, int maxDepth, int depth, int isMaximizing, double alpha, double beta, std::unordered_map<gameState, double>& transpositionTable)
	{
		if (depth >= maxDepth || gameStatus.gameStatus != inProgress) {
			return evaluateExist(playerTocheck, gameStatus);
		}

		auto iter = transpositionTable.find(gameStatus);
		if (iter != transpositionTable.end()) {
			return iter->second;
		}

		std::unordered_set<gameState> gameStates = game.generateAllPossibleMoves(gameStatus);

		if (isMaximizing) {
			double valueMax = std::numeric_limits<double>::lowest();

			for (const gameState& gameState : gameStates) {
				double eval = minimax(gameState, playerTocheck, maxDepth, depth, false, alpha, beta, transpositionTable);
				valueMax = std::max(valueMax, eval);
				alpha = std::max(alpha, eval);

				if (alpha >= beta) {
					break;
				}
			}
			transpositionTable[gameStatus] = valueMax;
			return valueMax;
		}
		else {
			double valueMin = std::numeric_limits<double>::max();

			for (const gameState& gameState : gameStates) {
				double eval = minimax(gameState, playerTocheck, maxDepth, depth + 1, true, alpha, beta, transpositionTable);
				valueMin = std::min(valueMin, eval);
				beta = std::min(beta, eval);

				if (alpha >= beta) {
					break;
				}
			}
			transpositionTable[gameStatus] = valueMin;
			return valueMin;
		}
	}


	//find best move for current game state
	void solveGame(int maxDepth)
	{
		if (game.m_gameState.gameStatus == inProgress) {
			double bestScore = std::numeric_limits<double>::lowest();
			bool foundWining = false;
			if (game.m_gameState.currentPlayer == white) {
				if (game.m_gameState.board.whiteOnBoard == 0 && game.m_gameState.board.blackOnBoard == 0) {
					foundWining = true;
					if (game.m_gameState.whiteReserve > game.m_gameState.blackReserve) {
						bestScore = std::numeric_limits<double>::max();
					}
					else {
						bestScore = std::numeric_limits<double>::lowest();
					}
				}
			}
			else {
				if (game.m_gameState.board.whiteOnBoard == 0 && game.m_gameState.board.blackOnBoard == 0) {
					foundWining = true;
					if (game.m_gameState.blackReserve > game.m_gameState.whiteReserve) {
						bestScore = std::numeric_limits<double>::max();
					}
					else {
						bestScore = std::numeric_limits<double>::lowest();
					}
				}
			}

			if (!foundWining) {

				std::unordered_set<gameState> gameStates = game.generateAllPossibleMovesEXT(game.m_gameState);
				pawn playerToCheck = game.m_gameState.currentPlayer;

				gameState info = game.m_gameState;
				std::unordered_map<gameState, double> transpositionTable(1000);

				//auto startTime = std::chrono::high_resolution_clock::now();

				std::vector<gameState> sortedGameStates;
				std::move(gameStates.begin(), gameStates.end(), std::back_inserter(sortedGameStates));
				sortByNumberOfReserve(sortedGameStates);

				for (const gameState& gameState : sortedGameStates) {
					double score = minimax(gameState, playerToCheck, maxDepth, 0, false, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), transpositionTable);

					if (score > bestScore) {
						bestScore = score;
						if (bestScore == std::numeric_limits<double>::max()) {
							break;
						}
					}
				}
				game.m_gameState = std::move(info);
				/*auto endTime = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
				std::cout << "Execution time: " << duration.count() << " s" << std::endl;*/
			}

			if (bestScore == std::numeric_limits<double>::max()) {
				if (game.m_gameState.currentPlayer == white)
					std::cout << "WHITE_HAS_WINNING_STRATEGY" << std::endl;
				else
					std::cout << "BLACK_HAS_WINNING_STRATEGY" << std::endl;
			}
			else if (bestScore == std::numeric_limits<double>::lowest()) {
				if (game.m_gameState.currentPlayer == white) {
					std::cout << "BLACK_HAS_WINNING_STRATEGY" << std::endl;
				}
				else {
					std::cout << "WHITE_HAS_WINNING_STRATEGY" << std::endl;
				}
			}
		}
	}

	void findBestMove(int maxDepth)
	{
		if (game.m_gameState.gameStatus == inProgress) {
			std::unordered_set<gameState> gameStates = game.generateAllPossibleMovesEXT(game.m_gameState);
			pawn playerToCheck = game.m_gameState.currentPlayer;

			gameState info = game.m_gameState;
			std::unordered_map<gameState, double> transpositionTable(1000);

			std::vector<gameState> sortedGameStates;
			std::move(gameStates.begin(), gameStates.end(), std::back_inserter(sortedGameStates));
			sortByNumberOfReserve(sortedGameStates);

			double bestScore = std::numeric_limits<double>::lowest();
			move currentMove = {};
			move bestMove = {};
			for (const gameState& gameState : sortedGameStates) {
				currentMove = gameState.lastMove;
				double score = minimax(gameState, playerToCheck, maxDepth, 0, false, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), transpositionTable);

				if (score > bestScore) {
					bestScore = score;
					bestMove = currentMove;
					std::cout << bestScore << std::endl;
					if (bestScore == std::numeric_limits<double>::max()) {
						break;
					}
				}
			}
			std::cout << currentMove.from << "-" << currentMove.to << std::endl;
			game.m_gameState = std::move(info);
		}
	}


private:
	void sortByNumberOfReserve(std::vector<gameState>& gameStates) {
		std::sort(gameStates.begin(), gameStates.end(), [](const gameState& gameState1, const gameState& gameState2) {
			if (gameState1.currentPlayer == black) {
				return gameState1.whiteReserve > gameState2.whiteReserve;
			}
			else {
				return gameState1.blackReserve > gameState2.blackReserve;
			}
			});
	}
};