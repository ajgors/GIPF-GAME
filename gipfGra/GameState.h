#pragma once

enum gameStatus {
	whiteWin, blackWin, inProgress, deadLock, badMove, needToRemove
};

struct move {
	std::string from;
	std::string to;
};

struct gameState {
	int whiteReserve;
	int blackReserve;
	int whiteRemoved;
	int blackRemoved;
	pawn currentPlayer;
	gameStatus gameStatus;
	Board board;
	move lastMove = {};

	gameState() : whiteReserve(0), blackReserve(0), whiteRemoved(0), blackRemoved(0), currentPlayer(white), gameStatus(inProgress), board(), lastMove() {}

	bool operator==(const gameState& other) const
	{
		return whiteReserve == other.whiteReserve && blackReserve == other.blackReserve && currentPlayer == other.currentPlayer && board.boardHash == other.board.boardHash;
	}

	gameState(const gameState& other) {
		whiteReserve = other.whiteReserve;
		blackReserve = other.blackReserve;
		whiteRemoved = other.whiteRemoved;
		blackRemoved = other.blackRemoved;
		currentPlayer = other.currentPlayer;
		gameStatus = other.gameStatus;
		board = other.board;
		lastMove = other.lastMove;
	}

	gameState(gameState&& other) {
		whiteReserve = other.whiteReserve;
		blackReserve = other.blackReserve;
		whiteRemoved = other.whiteRemoved;
		blackRemoved = other.blackRemoved;
		currentPlayer = other.currentPlayer;
		gameStatus = other.gameStatus;
		board = std::move(other.board);
		lastMove = std::move(other.lastMove);
	}

	gameState& operator=(const gameState& other) {
		whiteReserve = other.whiteReserve;
		blackReserve = other.blackReserve;
		whiteRemoved = other.whiteRemoved;
		blackRemoved = other.blackRemoved;
		currentPlayer = other.currentPlayer;
		gameStatus = other.gameStatus;
		board = other.board;
		lastMove = other.lastMove;
		return *this;
	}

	gameState& operator=(gameState&& other) {
		whiteReserve = other.whiteReserve;
		blackReserve = other.blackReserve;
		whiteRemoved = other.whiteRemoved;
		blackRemoved = other.blackRemoved;
		currentPlayer = other.currentPlayer;
		gameStatus = other.gameStatus;
		board = std::move(other.board);
		lastMove = std::move(other.lastMove);
		return *this;
	}
};


namespace std {
	template <>
	struct hash<gameState>
	{
		std::size_t operator()(const gameState& gameState) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			size_t hashNumber = ((hash<int>()(gameState.whiteReserve)
				^ (hash<int>()(gameState.blackReserve) << 1)) >> 1)
				^ ((hash<int>()(gameState.currentPlayer) << 1) >> 1)
				^ ((gameState.board.boardHash));

			return hashNumber;
		}
	};
}