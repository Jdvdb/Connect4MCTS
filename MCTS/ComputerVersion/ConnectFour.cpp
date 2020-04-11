#include "ConnectFour.h"

using namespace std;

ConnectFour::ConnectFour() {
	setup();
}

bool ConnectFour::checkWin(int col) {
	// check for invalid input (i.e. a root node)
	if (col < 0 || col > 6) {
		return false;
	}
	// find the last piece played
	unsigned int row = colHeight[col] - 1;
	char player = game[row][col];

	// counter for 4 in a row
	unsigned int counter = 1;

	// iterators that holds current position to check
	int rowIter = row, colIter = col - 1;

	// first check row by expanding left from piece placed
	while (colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		colIter--;
	}

	// next go right from the piece placed
	colIter = col + 1;

	while (colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		colIter++;
	}

	// check if there were 4 consecutive pieces
	if (counter >= 4) {
		return true;
	} else {
		counter = 1;
	}

	// next check col, go down from current piece position
	rowIter = row - 1;
	colIter = col;

	while (rowIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
	}

	// then go up from current piece position
	rowIter = row + 1;

	while (rowIter < boardHeight && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
	}

	// check again if this made a win
	if (counter >= 4) {
		return true;
	} else {
		counter = 1;
	}

	// next check diagonal from bottom left to top right
	// expand towards bottom left from current piece first
	rowIter = row - 1;
	colIter = col - 1;

	while (rowIter >= 0 && colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
		colIter--;
	}

	// then expand from current piece to top right
	rowIter = row + 1;
	colIter = col + 1;

	while (rowIter < boardHeight && colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
		colIter++;
	}

	// check if there is a win in this diagonal
	if (counter >= 4) {
		return true;
	} else {
		counter = 1;
	}

	// next check diagonal from top left to bottom right
	// start by expanding to top left from current piece
	rowIter = row + 1;
	colIter = col - 1;

	while (rowIter < boardHeight && colIter >= 0 && game[rowIter][colIter] == player) {
		counter++;
		rowIter++;
		colIter--;
	}

	// then expand from current piece to bottom right
	rowIter = row - 1;
	colIter = col + 1;

	while (rowIter >= 0 && colIter < boardWidth && game[rowIter][colIter] == player) {
		counter++;
		rowIter--;
		colIter++;
	}

	// check if there is a win in this diagonal. If not, then nobody has won
	if (counter >= 4) {
		return true;
	} else {
		return false;
	}
}

bool ConnectFour::availableSpot(int col) {
	// get the current number of pieces in desired column
	unsigned int numPieces = colHeight[col];

	// check if the number of pieces is less than the board height
	if (numPieces < boardHeight) {
		return true;
	} else {
		return false;
	}
}

bool ConnectFour::insertPiece(int col, char player) {
	// if the value isn't in the proper range return false
	if (col < 0 || col > 6) {
		return false;
	}
	// start by simply checking if this is a valid spot
	bool test = availableSpot(col);

	if (test) {
		// if it is, add a piece for the proper team
		unsigned int row = colHeight[col];
		game[row][col] = player;


		// update the colHeight
		colHeight[col] += 1;

		return true;
	} else {
		return false;
	}
}

bool ConnectFour::checkDraw() {
	// initially set it to true
	bool draw = true;

	// iterate through each column and check if it is full
	unsigned int col = 0;
	while (col < 7 && draw) {
		if (colHeight[col] < 6) {
			draw = false;
		}
		// if this column was not full, move onto the next
		col++;
	}

	return draw;
}

void ConnectFour::setup() {
	// iterate down each column
	for (unsigned int i = 0; i < boardWidth; i++) {
		for (unsigned int j = 0; j < boardHeight; j++) {
			game[j][i] = 'E';
		}

		// set the pieceHeight to 0 in each column
		colHeight[i] = 0;
	}
}

void ConnectFour::readArray() {
	// iterates through the borad to print it's state
	for (int i = boardHeight - 1; i >= 0; i--) {
		cout << "[";
		for (int j = 0; j < boardWidth; j++) {
			cout << "[" << game[i][j] << "]";
		}
		cout << "]" << endl;
	}
}

void ConnectFour::niceReadArray() {
	// iterates through the borad to print it's state
	for (int i = boardHeight - 1; i >= 0; i--) {
		for (int j = 0; j < boardWidth; j++) {
			// Red is X, Yellow is O (like tic tac toe :))
			if (game[i][j] == 'R') {
				cout << "|X|";
			} else if (game[i][j] == 'Y') {
				cout << "|O|";
			} else {
				cout << "| |";
			}
		}
		cout << endl;
	}
	// next do a row of dashes
	for (int i = 0; i < boardWidth; i++) {
		cout << "---";
	}
	cout << endl;

	// finally do a row of the input for each column
	for (int i = 0; i < boardWidth; i++) {
		cout << " " << i << " ";
	}
	cout << endl;

	// add extra newline for space
	cout << endl;
}
