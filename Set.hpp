#pragma once
#include <iostream>
#include <vector>
#include "Game.cpp"

class Set {
private:
	std::vector<Game> games;
	int games_player1;
	int games_player2;

public: 
	Set() : games_player1(0), games_player2(0) {}

	void addGame(Game game) {
		games.push_back(game);
		int winner = game.winner();
		if (winner == 1) games_player1++;
		if (winner == 2) games_player2++;
	}

	bool isWonGame() {
		return (games_player1 >= 6 && games_player1 >= games_player2 + 2) ||
			(games_player2 >= 6 && games_player2 >= games_player1 + 2);
	}

	int winner() {
		if (games_player1 >= 6 && games_player1 >= games_player2 + 2) return 1;
		if (games_player2 >= 6 && games_player2 >= games_player1 + 2) return 2;
		return 0;
	}
};