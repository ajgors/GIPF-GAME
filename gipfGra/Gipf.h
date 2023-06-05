#pragma once
#include "Game.h"
#include "InputHandler.h"

class GIPF {
public:	
	void play() {
		Game game;
		Solver solver(game);
		InputHandler inputHandler(game, solver);
		inputHandler.readInput();
	}
};