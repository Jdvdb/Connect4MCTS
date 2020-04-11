#ifndef _CONNECT_FOUR_H_
#define _CONNECT_FOUR_H_

#include <unordered_map>
#include <iostream>

using namespace std;

// all methods with col should receive input between 0 and 6 inclusive
class ConnectFour {
public:
	// Constructor that sets up the board
	ConnectFour();

	/*
	Checks if the last piece placed connects 4 in a row
	Arguments: unsigned int col: the last column a piece was placed in
	Returns: true if most recent piece placed is a winner, false otherwise
	*/
	bool checkWin(int col);

	/*
	Checks if there is room to place a piece in a certain column
	Arguments: unsigned int col: the column being checked (from 0 - 6)
	Returns: true if there is room, false if there is not 
	*/
	bool availableSpot(int col);

	/*
	Will check if there is room and place a piece if there is
	Arguments: 
	    unsigned int col: the column the piece is trying to be added to
	    char player: the player who made the move, either "R" or "Y";
	Returns: true if successful placement, false otherwise
	*/
	bool insertPiece(int col, char player);

	/*
	Will detemine whether the game is a draw (assuming it has not been won)
	Arguments: None
	Returns: bool: true for draw, false otherwise
	*/
	bool checkDraw();

	/*
	sets up the game board for a new game
	Arguments: None
	Returns: None
	*/
	void setup();

	/*
	Will print the current game board to the console. Mainly used for testing
	Arguments: None
	Returns: None
	*/
	void readArray();

	/*
	Will print a more legible game for humans
	Arguments: None
	Returns: None
	*/
	void niceReadArray();


private:
	// Constant values for the size of the board
	static const int boardHeight = 6;
	static const int boardWidth = 7;

	// The game board that will be used
	char game[6][7];

	// Unordered map to quickly check how many pieces are in the current column
	unordered_map<int, int> colHeight;

};


#endif