#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <future>
#include <stdio.h>
#include <windows.h>
#include <excpt.h>
#include <TicTacToe.h>

using namespace std;

 //This is a Tic Tac Toe 10x10 (5 in a row) project
 //The program works like this:
 //
 //1. When program is opened user gets 2 options: Create new game, Continue
 //   1.1. If user chooses "Create new game" program runs "New game" scenario (look at points 2-7)
 //   1.2. If user chooses "Continue" program tries to read previous game from "GameField.txt" 
 //        if read succeeded game continues from the last turn
 //        if read failed programm notifies user and creates a new one
 //
 //2. When game starts it requires 2 different usernames for players
 //   If input is incorrect program asks for a new one
 //
 //3. After the input of usernames program creates new "GameField.txt" file (or clears contets if it existed)
 //   
 //4. Current game data consists of:
 //   - Usernames
 //   - Scores of each player
 //   - Current game field state
 //   - Current players turn
 //
 //5. Game data is written into a file whenever a change to a game field is made (when any user makes a turn)
 //
 //6. Game ends when any user gets 5 "circles" or "crosses" in a row, or when game field has no empty spaces
 //   So there is 3 outcomes for any game: Player 1 wins, Player 2 wins or a Draw
 //
 //7. When game is won, winner gets 1 point. When game is drawn nobody gets any points
 //
 //Use of knowledge:
 //
 //1. Data types: Analysis of data types of variables in Work Report
 //2. Dynamic data structures: Game field is a matrix which is implemented as two-dimensional array
 //3. Exception handling: Handling of exceptions related with: Files, Data input
 //4. File system, I/O management: Program uses files to save current game data and to read
 //5. Threading and multitasking: Program works with multiple threads - main thread (current game status), writer thread (updating save file in async)


// Game constants
const char BOARD_SIZE = 10; // Size of the board (could be changed to resize game board)
const char WIN_CONDITION = 4; // How many cells in a row should be captured by one player to win
const char EMPTY = '-'; // Default output shown for an empty space on board
const char P1 = 'X'; // Default output shown for a first player's cell on board
const char P2 = 'O'; // Default output shown for a second player's cell on board
const char filename[13] = "GameData.txt";

// Game variables
string p1_name; // Name of the first player
string p2_name; // Name of the second player
short p1_score; // Score of the first player
short p2_score; // Score of the second player
bool is_p1_turn; // Check wheter it is a first player's turn
short empty_spaces; // How many cells in a row should be captured by one player to win

static char gameBoard[BOARD_SIZE][BOARD_SIZE]; 

mutex mtx; // Mutex for thread synchronization

// Function to print the game stats
void printStats() {
    cout << P1 << ": " << p1_name << ". Score - " << p1_score << endl;
    cout << P2 << ": " << p2_name << ". Score - " << p2_score << endl;
    if (is_p1_turn)
        cout << "Current turn: " << P1 << " - " << p1_name << endl;
    else
        cout << "Current turn: " << P2 << " - " << p2_name << endl;
}

// Function to print the game board
void printBoard() {
    cout << "  0 1 2 3 4 5 6 7 8 9" << endl;
    for (int row = 0; row < BOARD_SIZE; row++) {
        cout << row << " ";
        for (int col = 0; col < BOARD_SIZE; col++) {
            cout << gameBoard[row][col] << " ";
        }
        cout << endl;
    }
}

// Check whether a combination of 4 was made this turn horizontally
bool checkHor(char item) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        int counter = 0;
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (gameBoard[row][col] == item)
                counter++;
            else 
                counter = 0;
            if (counter == WIN_CONDITION)
                return true;
        }
    }
    return false;
}

// Check whether a combination of 4 was made this turn vertically
bool checkVert(char item) {
    for (int col = 0; col < BOARD_SIZE; col++) {
        int counter = 0;
        for (int row = 0; row < BOARD_SIZE; row++) {
            if (gameBoard[row][col] == item)
                counter++;
            else
                counter = 0;
            if (counter == WIN_CONDITION)
                return true;
        }
    }
    return false;
}

// Check whether a combination of 4 was made this turn
bool checkWin(char item) {
    auto f1 = async(checkHor, item);
    auto f2 = async(checkVert, item);
    return f1.get() || f2.get();
}

// Read of a single nickname with validation
string nicknameValidation(int playerNum) {
    string str = "";
    do {
        cout << "Write Player " << playerNum << " nickname: ";
        cin >> str;
        if (str == "")
            cout << "Player nickname can't be empty" << endl;
        else if (str.size() > 20) {
            cout << "Player nickname can't be longer than 20 characters" << endl;
            str = "";
        }
    } while (str == "");
    cout << "Player " << playerNum << " nickname is " << str << endl;
    return str;
}

// Read of 2 nicknames
void readNicknames() {
    p1_name = nicknameValidation(1);
    system("pause");
    system("cls");
    p2_name = nicknameValidation(2);
    system("pause");
    system("cls");
}

// Clear the game board and stats
void clearData() {
    p1_name = "";
    p2_name = "";
    p1_score = 0;
    p2_score = 0;
    is_p1_turn = true;
    empty_spaces = BOARD_SIZE * BOARD_SIZE;
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            gameBoard[row][col] = EMPTY;
}

void clearTable() {
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            gameBoard[row][col] = EMPTY;
}

// Function to handle the game logic
void newGame() {
    // Clear the game board and stats
    clearData();

    // Read nicknames
    readNicknames();
}

// Checks wheter there is a previous game file
bool loadGame() {
    exception InvalidItem = exception("Error. Invalid item");
    lock_guard<mutex> lock(mtx); // Lock the mutex
    ifstream file(filename);
    int p1_count = 0;
    int p2_count = 0;
    empty_spaces = 0;
    if (file.is_open()) {
        try {
            file >> p1_name >> p1_score;
            file >> p2_name >> p2_score;
            file >> is_p1_turn;
            string str;
            for (int row = 0; row < BOARD_SIZE; row++) {
                file >> str;
                for (int col = 0; col < BOARD_SIZE; col++) {
                    switch (str[col])
                    {
                    case P1:
                        p1_count++;
                        break;
                    case P2:
                        p2_count++;
                        break;
                    case EMPTY:
                        empty_spaces++;
                        break;
                    default:
                        throw InvalidItem;
                    }
                    gameBoard[row][col] = str[col];
                }
            }
            if (p1_count - p2_count > 1 && p2_count - p1_count < -1)
                throw InvalidItem;
            return true;
        }
        catch (exception InvalidItem) {
            cout << InvalidItem.what() << endl;
        }
        catch (...) {
            cout << "Error. File save is corrupted" << endl;
        }
    }
    else {
        cout << "Error. File can not be opened" << endl;
    }
    return false;
}

// Function to write the game state to a file
bool writeGameState() {
    lock_guard<mutex> lock(mtx); // Lock the mutex
    ofstream file(filename);
    if (!file.good())
        return false;
    file << p1_name << ' ' << p1_score << endl;
    file << p2_name << ' ' << p2_score << endl;
    file << is_p1_turn << endl;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            file << gameBoard[row][col];
        }
        file << endl;
    }
    return true;
}

int readNum(string msg) {
    int num = -1;
    string str;
    bool isSucceed = false;
    do {
        cout << msg;
        cin >> str;
        try {
            num = stoi(str);
            if (num < BOARD_SIZE && num >= 0)
                isSucceed = true;
            else
                throw out_of_range("Default");
        }
        catch (invalid_argument) {
            cout << "Error. Inavalid input" << endl;
        }
        catch (out_of_range) {
            cout << "Error. Index out of range" << endl;
        }
    } while (!isSucceed);
    return num;
}

void readPlace() {
    do {
        if (is_p1_turn)
            cout << "Place " << P1 << endl;
        else
            cout << "Place " << P2 << endl;
        int row = readNum("Row: ");
        int col = readNum("Column: ");

        if (gameBoard[row][col] == EMPTY) {
            gameBoard[row][col] = is_p1_turn ? P1 : P2;
            return;
        }
        cout << "Cell (" << row << "; " << col << ") is already taken. Try again." << endl;
    } while (true);
}
    
// Choose 1 of 2 options: "Create new game" or "Load previous game"
// If option 1 is chosen: creates a new game with empty board and new players and stats
// If option 2 is chosen: check whether game save file is readable if so loads a previous game, otherwise does the same as option 1
void mainMenu() {
    cout << "TicTacToe 10x10 (4 in a row)" << endl << endl;
    cout << "Choose option (write a number):" << endl;
    cout << "1. Create new game" << endl;
    cout << "2. Load previous game" << endl;
    string option;
    cin >> option;
    while (option != "1" && option != "2") {
        cout << "Wrong input. Please try again";
        cin >> option;
    }
    system("cls");
    if (option == "1" || !loadGame())
        newGame();
}

// Function that repeats turns until game is won or drawn
void playGame() {
    bool check = false;
    do {
        printStats();
        printBoard();
        thread t2(writeGameState);
        thread t1(readPlace);
        t2.join();
        t1.join();
        if (is_p1_turn)
            check = checkWin(P1);
        else
            check = checkWin(P2);
        empty_spaces--;
        if (!check)
            is_p1_turn = !is_p1_turn;
        else
            clearTable();
        system("cls");
    } while (!check && empty_spaces != 0);
    system("cls");
    if (empty_spaces == 0)
        cout << "The game was drawn!" << endl;
    else if (is_p1_turn) {
        cout << "The game was won by " << p1_name << endl;
        p1_score++;
    }
    else {
        cout << "The game was won by " << p2_name << endl;
        p2_score++;
    }  
    is_p1_turn = !is_p1_turn;
    system("pause");
    system("cls");
}


int main() {
    // Choose 1 of 2 options: "Create new game" or "Load previous game"
    // If option 1 is chosen: creates a new game with empty board and new players and stats
    // If option 2 is chosen: check whether game save file is readable if so loads a previous game, otherwise does the same as option 1
    mainMenu(); 

    while (p1_score != 3 || p2_score != 3) {
        playGame();
    }
    if (p1_score == 3)
        cout << "Player " << p1_name << " Wins the Match!" << endl;
    else
        cout << "Player " << p2_name << " Wins the Match!" << endl;
    cout << "Score - " << (int)p1_score << ":" << (int)p2_score << endl;
    system("pause");
}
