#include <iostream>
#include <vector>

class Game {
private: 
	int points_player1;
	int points_player2;

public:
	Game() : points_player1(0), points_player2(0) {}

	void addPointPlayer1() { points_player1++; }
	void addPointPlayer2() { points_player2++; }

	bool isGameWon() {
		return (points_player1 >= 4 && points_player1 >= points_player2 + 2) ||
			(points_player2 >= 4 && points_player2 >= points_player1 + 2);
	}

	int winner() {
		if (points_player1 >= 4 && points_player1 >= points_player2 + 2) return 1;
		if (points_player2 >= 4 && points_player2 >= points_player1 + 2) return 2;
		return 0;
	}
};