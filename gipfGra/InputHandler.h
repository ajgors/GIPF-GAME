#pragma once
#include "Game.h"
#include "Solver.h"

class InputHandler
{
private:
	Game& game;
	Solver& solver;

	move readMove(const std::string& input)
	{
		size_t colonIndex = input.find_first_of(":");
		size_t spaceIndex = input.find_first_of(" ");
		size_t dashIndex = input.find_first_of("-");

		game.m_ambigiousToRemove = {};
		move move;
		if (colonIndex != std::string::npos) { //if resolving ambigious move
			std::string coordinateIndex;
			move.from = input.substr(spaceIndex + 1, colonIndex - dashIndex - 3);
			move.to = input.substr(dashIndex + 1, colonIndex - dashIndex - 3);

			if (toupper(input[colonIndex - 1]) == white) { //save trigering pawn
				game.m_ambigiousToRemove.triggering = white;
			}
			else {
				game.m_ambigiousToRemove.triggering = black;
			};

			for (int i = colonIndex + 2; i < input.size(); i++) { //read index to next space
				if (input[i] == SPACE_BETWEN_FIELD) {
					game.m_ambigiousToRemove.points.push_back(coordinateIndex);
					coordinateIndex.clear();
					continue;
				}
				coordinateIndex += input[i];
			}
			if (coordinateIndex.size() > 0) {
				game.m_ambigiousToRemove.points.push_back(coordinateIndex);
			}
		}
		else {
			move.from = input.substr(spaceIndex + 1, input.size() - dashIndex - 1);
			move.to = input.substr(dashIndex + 1);
		}
		return move;
	}

public:
	InputHandler(Game& game, Solver& solver) : game(game), solver(solver) {}

	void readInput() {
		std::string input;
		while (std::getline(std::cin, input)) {
			if (input.size() == 0) continue;
			if (input == "LOAD_GAME_BOARD") {
				game.loadGame();
			}
			else if (input == "PRINT_GAME_BOARD") {
				game.printInformations(game.m_gameState);
				game.m_gameState.board.printMap();
			}
			else if (utils::contains(input, "DO_MOVE")) {
				game.proccessPlayerMove(readMove(input));
			}
			else if (utils::contains(input, "REMOVE")) {
				game.remove(input);
			}
			else if (input == "GEN_ALL_POS_MOV") {
				game.printPossibleMoves(game.generateAllPossibleMoves(game.m_gameState));
			}
			else if (input == "GEN_ALL_POS_MOV_NUM") {
				game.printPossibleMovesNum(game.generateAllPossibleMoves(game.m_gameState));
			}
			else if (input == "GEN_ALL_POS_MOV_EXT") {
				game.printPossibleMoves(game.generateAllPossibleMovesEXT(game.m_gameState));
			}
			else if (input == "GEN_ALL_POS_MOV_EXT_NUM") {
				game.printPossibleMovesNum(game.generateAllPossibleMovesEXT(game.m_gameState));
			}
			else if (utils::contains(input, "WINNING_SEQUENCE_EXIST")) {
				std::string num = input.substr(input.find_first_of(" ") + 1);
				int n = std::stoi(num);
				solver.winningSequenceExists(n);
			}
			else if (utils::contains(input, "SOLVE_GAME_STATE")) {
				solver.solveGame(10000);
			}
			else if (input == "IS_GAME_OVER") {
				game.printGameEndInfo();
			}
			else if (utils::contains(input, "SOLVE")) {
				std::string num = input.substr(input.find_first_of(" ") + 1);
				int n = std::stoi(num);
				solver.findBestMove(n);
			}
		}
	}
};