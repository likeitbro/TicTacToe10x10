#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

// This is a Tic Tac Toe 10x10 (5 in a row) project
// The program works like this:
// 
// 1. When program is opened user gets 2 options: Create new game, Continue
//    1.1. If user chooses "Create new game" program runs "New game" scenario (look at points 2-7)
//    1.2. If user chooses "Continue" program tries to read previous game from "GameField.txt" 
//         if read succeeded game continues from the last turn
//         if read failed programm notifies user and creates a new one
// 
// 2. When game starts it requires 2 different usernames for players
//    If input is incorrect program asks for a new one
// 
// 3. After the input of usernames program creates new "GameField.txt" file (or clears contets if it existed)
//    
// 4. Current game data consists of:
//    - Usernames
//    - Scores of each player
//    - Current game field state
//    - Current players turn
// 
// 5. Game data is written into a file whenever a change to a game field is made (when any user makes a turn)
// 
// 6. Game ends when any user gets 5 "circles" or "crosses" in a row, or when game field has no empty spaces
//    So there is 3 outcomes for any game: Player 1 wins, Player 2 wins or a Draw
// 
// 7. When game is won, winner gets 1 point. When game is drawn nobody gets any points
// 
// Use of knowledge:
// 
// 1. Data types: Analysis of data types of vriables in Work Report
// 2. Dynamic data structures: Game field is a matrix which is implemented as two-dimensional array
// 3. Exception handling: Handling of exceptions related with: Files, Data input
// 4. File system, I/O management: Program uses files to save current game data and to read
// 5. Threading and multitasking: Program works with multiple threads - main thread (current game status), writer thread (updating save file in async)

int main()
{
    std::cout << "Hello World!\n";
}