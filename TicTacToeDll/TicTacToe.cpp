#include "pch.h"
#include <utility>
#include <limits.h>
#include "TicTacToe.h"
#include <iostream>


// Game constants
const char BOARD_SIZE = 10; // Size of the board (could be changed to resize game board)
const char WIN_CONDITION = 4; // How many cells in a row should be captured by one player to win
const char EMPTY = '-'; // Default output shown for an empty space on board
const char P1 = 'X'; // Default output shown for a first player's cell on board
const char P2 = 'O'; // Default output shown for a second player's cell on board
const char filename[13] = "GameData.txt";

void print_empty() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << EMPTY << ' ';
		}
		std::cout << std::endl;
	}
}
