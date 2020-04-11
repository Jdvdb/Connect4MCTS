#include "CFArduino.h"

void CFArduino::drawWin(int startX, int startY, int endX, int endY, MCUFRIEND_kbv& tft) {
	// make sure the tft is set to draw to
    pinMode(A3, OUTPUT);
    pinMode(A2, OUTPUT);

	// values that will actually be used to draw
	int x1, y1, x2, y2;
	x1 = 28 + startX * 53;
	// since the array is recorded upside down, we need to alter startY
	y1 = 28 + (5 - startY) * 53;
	x2 = 28 + endX * 53;
	y2 = 28 + (5 - endY) * 53;

	// draw the winning line
	tft.drawLine(x1, y1, x2, y2, 0xFCB2);
}

bool CFArduino::checkWin(int col, MCUFRIEND_kbv& tft) {
	// check for invalid input (i.e. a root node)
	if (col < 0 || col > 6) {
		return false;
	}

	// find the last piece played
	unsigned int row = colHeight[col] - 1;
	char player = game[row][col];

	// we need to know the start and end points for the pieces. We will draw lines from the top left
	int startX, startY, endX, endY;
	startX = col, endX = col;
	startY = row, endY = row;

	// counter for 4 in a row
	unsigned int counter = 1;

	// iterators that holds current position to check
	int rowIter = row, colIter = col - 1;

	// first check row by expanding left from piece placed
	while (colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		colIter--;
		startX--;
	}

	// next go right from the piece placed
	colIter = col + 1;

	while (colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		colIter++;
		endX++;
	}

	// check if there were 4 consecutive pieces
	if (counter >= 4) {
		drawWin(startX, startY, endX, endY, tft);
		return true;
	} else {
		counter = 1;
	}

	// next check col, go down from current piece position
	rowIter = row - 1;
	colIter = col;

    // reset the drawing values
	startX = col, endX = col;
	startY = row, endY = row;

	while (rowIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
		startY--;
	}

	// then go up from current piece position
	rowIter = row + 1;

	while (rowIter < boardHeight && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
		endY++;
	}

	// check again if this made a win
	if (counter >= 4) {
		drawWin(startX, startY, endX, endY, tft);
		return true;
	} else {
		counter = 1;
	}

	// next check diagonal from bottom left to top right
	// expand towards bottom left from current piece first
	rowIter = row - 1;
	colIter = col - 1;

    // reset the drawing values
	startX = col, endX = col;
	startY = row, endY = row;

	while (rowIter >= 0 && colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
		colIter--;
		startX--;
		startY--;
	}

	// then expand from current piece to top right
	rowIter = row + 1;
	colIter = col + 1;

	while (rowIter < boardHeight && colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
		colIter++;
		endX++;
		endY++;
	}

	// check if there is a win in this diagonal
	if (counter >= 4) {
		drawWin(startX, startY, endX, endY, tft);
		return true;
	} else {
		counter = 1;
	}

	// next check diagonal from top left to bottom right
	// start by expanding to top left from current piece
	rowIter = row + 1;
	colIter = col - 1;

    // reset the drawing values
	startX = col, endX = col;
	startY = row, endY = row;

	while (rowIter < boardHeight && colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
		colIter--;
		startX--;
		startY++;
	}

	// then expand from current piece to bottom right
	rowIter = row - 1;
	colIter = col + 1;

	while (rowIter >= 0 && colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
		colIter++;
		endX++;
		endY--;
	}

	// check if there is a win in this diagonal. If not, then nobody has won
	if (counter >= 4) {
		drawWin(startX, startY, endX, endY, tft);
		return true;
	} else {
		return false;
	}
}

bool CFArduino::availableSpot(int col) {
	// get the current number of pieces in desired column
	unsigned int numPieces = colHeight[col];

	// check if the number of pieces is less than the board height
	if (numPieces < boardHeight) {
		return true;
	} else {
		return false;
	}
}

bool CFArduino::insertPiece(int col, char player, MCUFRIEND_kbv& tft) {
	// same code as in other function
	// start by simply checking if this is a valid spot
	bool test = availableSpot(col);

	if (test) {
		// if it is, add a piece for the proper team
		unsigned int row = colHeight[col];
		game[row][col] = player;

		// get the x and y values for the piece
		int xPos, yPos;
		xPos = 28 + 53 * col;
		// because of the set up of the array, you have to subtract screen height
		yPos = 320 - (28 + 53 * row);

		// draw the piece
		if (player == 'R') {
			tft.fillCircle(xPos, yPos, 24, TFT_RED);
		} else {
			tft.fillCircle(xPos, yPos, 24, TFT_YELLOW);
		}

		// update the colHeight
		colHeight[col] += 1;

		return true;
	} else {
		return false;
	}
}

bool CFArduino::checkDraw() {
	// draw will be returned
	bool draw = true;
	unsigned int col = 0;
	// check each column if it is full
	while (col < 7 && draw) {
		if (colHeight[col] < 6) {
			draw = false;
		}
		col++;
	}

	return draw;
}

void CFArduino::setup() {
	// iterate down each column
	for (unsigned int i = 0; i < boardWidth; i++) {
		for (unsigned int j = 0; j < boardHeight; j++) {
			game[j][i] = 'E';
		}

		// set the pieceHeight to 0 in each column
		colHeight[i] = 0;
	}
}