#ifndef _CONNECT_FOUR_H_
#define _CONNECT_FOUR_H_

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

class CFArduino {
public:

	/*
	Draws the line showing the winning pieces
	Arguments:
	    int startX: col with first piece
	    int startY: row with first piece
	    int endX: col with last piece
	    int endY: row with last piece
	    MCUFRIEND_kbv& tft: allows for drawing on the screen
	*/
	void drawWin(int startX, int startY, int endX, int endY, MCUFRIEND_kbv& tft);
	
	/*
	Checks if the last piece placed connects 4 in a row
	Arguments: 
	    int col: the last column a piece was placed in
	    MCUFRIEND_kbv& tft: allows for drawing on the screen
	Returns: true if most recent piece placed is a winner, false otherwise
	*/
	bool checkWin(int col, MCUFRIEND_kbv& tft);

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
	bool insertPiece(int col, char player, MCUFRIEND_kbv& tft);

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
	char game[6][7];

private:
	// The game board that will be used
	

	int colHeight[7] = {0,0,0,0,0,0,0};

	// Constant values for the size of the board
	static const int boardHeight = 6;
	static const int boardWidth = 7;
};

#endif