#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

#include "CFArduino.h"

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// screen dimensions for arduino
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

// dimensions for connect 4 pieces
#define PIECE_DIAMETER 50
#define BUFFER_SPACE 3

// width of ts section for pieces
#define TS_PIECE_SPACE 54

// info for menu bar
#define MENU_X_POS 380

// dimensions for game setup screen 
#define MODE_WIDTH 150
#define MODE_HEIGHT 70
#define MODE_Y_POS 40
#define MODE_X_POS 10

#define DIFFICULTY_WIDTH 200
#define DIFFICULTY_HEIGHT 70
#define DIFFICULTY_Y_POS 160
#define DIFFICULTY_X_POS 10

#define START_Y 260
#define START_HEIGHT 60

// Pressure of zero means no pressing
#define MINPRESSURE 400 // min pressure value considered valid
#define MAXPRESSURE 800 // max pressure value considered valid

// Pin Definitions for touch screen
#define YP A3
#define XM A2
#define YM 9
#define XP 8

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// basic setup for the arduino
void setup() {
    // initialize Arduino
    init();

    // initialize serial port
    Serial.begin(9600);
    Serial.flush();

    uint16_t ID = tft.readID();
    tft.begin(ID);

	tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
}

/*
Checks if the screen is queried in the menu
Arguments:
    int& mode: the variable that tells you the current mode selected
    int& difficulty: the variable that tells you the current difficulty
    bool& startGame: variable that tells you if the game should be started
Returns: true if one of the variables is modified
*/
bool setupCheck(int& mode, int& difficulty, bool& startGame) {
    // return value
    bool newChange = false;

    // query touchscreen and then reset pin states
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    // check for a screen tap
    if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
        int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, tft.width());
        int pty = map(touch.x, TS_MINY, TS_MAXY, 0, tft.height());

        // check if AI vs Player was tapped
        if (ptx > MODE_X_POS && ptx < MODE_X_POS + MODE_WIDTH && 
        pty > MODE_Y_POS && pty < MODE_Y_POS + MODE_HEIGHT) {
            // if this wasn't already the current mode, update the screen
            if (mode != 0) {
                newChange = true;
                mode = 0;
            }
        }

        // check if Player vs AI was tapped
        else if (ptx > MODE_X_POS + MODE_WIDTH && ptx < MODE_X_POS + 2 * MODE_WIDTH && 
        pty > MODE_Y_POS && pty < MODE_Y_POS + MODE_HEIGHT) {
            // if this wasn't already the current mode, update the screen
            if (mode != 1) {
                newChange = true;
                mode = 1;
            }
        }       

        // check if No Computer was tapped
        else if (ptx > MODE_X_POS + 2 * MODE_WIDTH && ptx < MODE_X_POS + 3 * MODE_WIDTH && 
        pty > MODE_Y_POS && pty < MODE_Y_POS + MODE_HEIGHT) {
            // if this wasn't already the current mode, update the screen
            if (mode != 2) {
                newChange = true;
                mode = 2;
            }
        } 

        // check if Easy was tapped
        else if (ptx > DIFFICULTY_X_POS && ptx < DIFFICULTY_X_POS + DIFFICULTY_WIDTH &&
        pty > DIFFICULTY_Y_POS && pty < DIFFICULTY_Y_POS + DIFFICULTY_HEIGHT && mode != 2) {
            // if this wasn't already the current mode, update the screen
            if (difficulty != 0) {
                newChange = true;
                difficulty = 0;
            }
        } 

        // check if Doom was tapped
        else if (ptx > DIFFICULTY_X_POS + DIFFICULTY_WIDTH && ptx < DIFFICULTY_X_POS + 2 * DIFFICULTY_WIDTH &&
         pty > DIFFICULTY_Y_POS && pty < DIFFICULTY_Y_POS + DIFFICULTY_HEIGHT && mode != 2) {
            // if this wasn't already the current mode, update the screen
            if (difficulty != 1) {
                newChange = true;
                difficulty = 1;
            }
        }

        // check if PLAY was tapped
        else if (pty > START_Y) {
            newChange = true;
            startGame = true;
        }
    }
    return newChange;
}

/*
Checks if the player taps and plays a piece if they do
Arguments:
    char player: the colour of the current player piece
    MCUFRIEND_kbv& tft: access to the display
    CFArduino& game: the game board being used
Returns: the position of where a piece is placed, -1 if nothing placed
*/
int playerMove(char player, MCUFRIEND_kbv& tft, CFArduino& game) {
    // boolean that tells you if a valid move was made
    bool validMove = false;

    // query touchscreen and then reset pin states
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    // check for a screen tap
    if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
        int ptx = map(touch.y, TS_MINX, TS_MAXX, 0, tft.width());
        int pty = map(touch.x, TS_MINY, TS_MAXY, 0, tft.height());

        if (ptx < TS_PIECE_SPACE) {
            validMove = game.insertPiece(0, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 0;
            }
        } else if (ptx < 2 * TS_PIECE_SPACE) {
            validMove = game.insertPiece(1, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 1;
            }
        } else if (ptx < 3 * TS_PIECE_SPACE) {
            validMove = game.insertPiece(2, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 2;
            }
        } else if (ptx < 4 * TS_PIECE_SPACE) {
            validMove = game.insertPiece(3, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 3;
            }
        } else if (ptx < 5 * TS_PIECE_SPACE) {
            validMove = game.insertPiece(4, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 4;
            }
        } else if (ptx < 6 * TS_PIECE_SPACE) {
            validMove = game.insertPiece(5, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 5;
            }
        } else if (ptx < 7 * TS_PIECE_SPACE){
            validMove = game.insertPiece(6, player, tft);
            if (validMove) {
                delay(300);
                while (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE){
                    touch = ts.getPoint();
                }
                return 6;
            }
        }

        if (!validMove) {
            return -1;
        }
    }
    // no touch so there was no valid move
    return -1;
}

/*
Will redraw the game setup page when called
Arguments:
    int mode: 0 for computer vs player, 1 for player vs computer, 2 for player vs player
    int difficulty: 0 for easy, 1 for hard
Returns: none
*/
void drawMainSetup(int mode, int difficulty) {
    // first reset the background and set the text cursor
    tft.setCursor(MODE_X_POS, 10);
    tft.setTextWrap(false);
    tft.setTextSize(3);

    // set the text colour to white on a black background
    tft.setTextColor(0xFFFF, 0x0000);
    tft.print("Choose Game Mode:");

    // draw the 3 rectangles for the game
    for (int i = 0; i < 3; i++) {
        // the current button's x value
        int currentX = MODE_X_POS + MODE_WIDTH * i;
        // draw the button rectangle and set the cursor
        // if this is the selected mode, change the colour
        if (i == mode) {
            // set the text colour to black on a white background
            tft.setTextColor(0x0000, 0xFFFF);
            tft.fillRect(currentX + i, MODE_Y_POS, MODE_WIDTH, MODE_HEIGHT, 0xFFFF);
        } else {
            // set the text colour to black on a grey background
            tft.setTextColor(0x0000, 0xE638);
            tft.fillRect(currentX + i, MODE_Y_POS, MODE_WIDTH, MODE_HEIGHT, 0xE638);
        }

        // get the cursor ready to write button details, 5 is for margin
        tft.setCursor(currentX + 5, MODE_Y_POS + MODE_HEIGHT / 2);
        tft.setTextWrap(false);
        tft.setTextSize(2);

        if (i == 0) {
            tft.print("AI vs Player");
        } else if (i == 1) {
            tft.print("Player vs AI");
        } else {
            tft.print("No Computer");
        }
    }

    // set the text colour to white on a black background
    tft.setTextColor(0xFFFF, 0x0000);
    // 20 is for a vertical margin
    tft.setCursor(MODE_X_POS, MODE_Y_POS + MODE_HEIGHT + 20);
    tft.setTextWrap(false);
    tft.setTextSize(3);
    tft.print("Choose AI Difficulty");

    // draw buttons for ai difficulty if ai is playing
    if (mode != 2) {
        for (int i = 0; i < 2; i++) {
            int currentX = DIFFICULTY_X_POS + DIFFICULTY_WIDTH * i;
            if (i == difficulty) {
                // set the text colour to black on a white background
                tft.setTextColor(0x0000, 0xFFFF);
                tft.fillRect(currentX + i, DIFFICULTY_Y_POS, DIFFICULTY_WIDTH, DIFFICULTY_HEIGHT, 0xFFFF);       
            } else {
                // set the text colour to black on a white background
                tft.setTextColor(0x0000, 0xE638);
                tft.fillRect(currentX + i, DIFFICULTY_Y_POS, DIFFICULTY_WIDTH, DIFFICULTY_HEIGHT, 0xE638);                 
            }

            // get the cursor ready to write button details, -5 is to help centre text
            tft.setCursor(currentX + DIFFICULTY_WIDTH / 3, DIFFICULTY_Y_POS + DIFFICULTY_HEIGHT / 2 - 5);
            tft.setTextWrap(false);
            tft.setTextSize(2);

            if (i == 0) {
                tft.print("Easy");
            } else {
                tft.print("Doom");
            }
        } 
    } else {
        // throw a big ol black rectangle over the difficulty
        tft.fillRect(DIFFICULTY_X_POS, DIFFICULTY_Y_POS - 30, DISPLAY_WIDTH, DIFFICULTY_HEIGHT + 30, 0x0000);  
    }

    // create the 'play button'
    tft.fillRect(0, START_Y, DISPLAY_WIDTH, START_HEIGHT, 0xFFFF);

    tft.setTextColor(0x0000, 0xFFFF);
    // add 30 to help centre the text
    tft.setCursor(DISPLAY_WIDTH / 3 + 30, START_Y + START_HEIGHT / 2);
    tft.setTextWrap(false);
    tft.setTextSize(3);

    tft.print("PLAY!");
}

/*
Redraws the game information menu on the side of the screen
Arguments:
    int mode: the variable that tells you the current mode selected
    int player: 0 for AI, 1 for P1, 2 for P2
    int difficulty: the variable that tells you the current difficulty
    String winner: the current winning character
Returns: true if one of the variables is modified
*/
void drawMenu(int mode, int player, int difficulty, String winner) {
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    // create a side area for game information
    tft.fillRect(MENU_X_POS, 0, DISPLAY_WIDTH - MENU_X_POS, DISPLAY_HEIGHT, TFT_WHITE);


    // text setup
    tft.setTextColor(0x0000, 0xFFFF);
    tft.setCursor(MENU_X_POS + BUFFER_SPACE, BUFFER_SPACE);
    tft.setTextWrap(false);
    tft.setTextSize(2);

    // player turn 
    tft.print("Turn:");

    tft.setCursor(MENU_X_POS + BUFFER_SPACE, 8 * BUFFER_SPACE);
    if (player == 0) {
        tft.print("AI");
    } else if (player == 1) {
        tft.print("P1");
    } else {
        tft.print("P2");
    }

    // current mode
    tft.setCursor(MENU_X_POS + BUFFER_SPACE, 26 * BUFFER_SPACE);
    tft.print("Mode:");

    tft.setCursor(MENU_X_POS + BUFFER_SPACE, 34 * BUFFER_SPACE);
    if (mode == 0) {
        tft.print("AI V P");
    } else if (mode == 1) {
        tft.print("P V AI");
    } else {
        tft.print("P V P");
    }

    // difficulty if there is computer
    if (mode == 0 || mode == 1) {
        tft.setCursor(MENU_X_POS + BUFFER_SPACE, 52 * BUFFER_SPACE);
        tft.print("AI LEVEL:");

        tft.setCursor(MENU_X_POS + BUFFER_SPACE, 60 * BUFFER_SPACE);
        if (difficulty == 0) {
            tft.print("Easy");
        } else {
            tft.print("DOOM");
        }
    }

    // print winner
    tft.setCursor(MENU_X_POS + BUFFER_SPACE, 78 * BUFFER_SPACE);
    tft.print("Winner:");
    tft.setCursor(MENU_X_POS + BUFFER_SPACE, 86 * BUFFER_SPACE);
    tft.print(winner);
}

/*
Checks if the screen is queried in the menu
Arguments:
    int& mode: the variable that tells you the current mode selected
    int& difficulty: the variable that tells you the current difficulty
    bool& startGame: variable that tells you if the game should be started
Returns: true if one of the variables is modified
*/
void drawGame(int mode, int difficulty) {
	// set the background colour
	tft.fillScreen(TFT_BLUE);

	// create the holes for all of the pieces
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			int xPos = BUFFER_SPACE + PIECE_DIAMETER / 2 + i * BUFFER_SPACE + i * PIECE_DIAMETER;
			int yPos = BUFFER_SPACE + PIECE_DIAMETER / 2 + j * BUFFER_SPACE + j * PIECE_DIAMETER;
			tft.fillCircle(xPos, yPos, PIECE_DIAMETER / 2, TFT_BLACK);
		}
	}
}

int main() {
    setup();

    // very simple finite state machine:
    /* BREAKDOWN:
    GAME_SELECTION: The selection screen where difficulty (easy or hard) and mode (pvc, cvp, or pvp) are chosen
    GAME_SETUP: Sending information to computer about the game setup
    PLAYER_TURN: When the player is selecting where to play a piece
    COMPUTER_TURN: When the computer is receiving info about game state, making a move, and communicating back
    END_GAME: Not avengers, but when someone has won. Displays winner and gives option to return to game selection
    */
    // bool that determines if the screen should be redrawn
    bool redrawScreen = true;

    // variables for the current mode and difficulty. see their meaning in drawMainSetup description
    int mode = 0, difficulty = 0;

    // tells you if you should start the game
    bool startGame = false;

    // temp fix for p v ai
    bool noComputerInput = true;

    // the connect 4 game
    CFArduino game;
    game.setup();

    // colours for first and second player (i.e. order people play)
    char player1 = 'R', player2 = 'Y';

    // the message that will be printed under the winner
    String winner = "???";

    // current player, 0 = computer, 1 =  p1, 2 = p2
    int currentPlayer = 0;

    enum {GAME_SELECTION, GAME_SETUP, PLAYER_TURN, COMPUTER_TURN, SECOND_PLAYER, END_GAME} curr_mode = GAME_SELECTION;
    while (true) {
        if (curr_mode == GAME_SELECTION) {
            // check if the screen should be redrawn
            if (redrawScreen) {
                drawMainSetup(mode, difficulty);
            }
            redrawScreen = false;

            // check if the user is tapping anywhere on the screen
            redrawScreen = setupCheck(mode, difficulty, startGame);

            // this will be true if the user hit the play button
            if (startGame) {
                curr_mode = GAME_SETUP;
            }
        }
        else if (curr_mode == GAME_SETUP) {
            // if this is a computer game, send over info and wait for confirmation
            if (mode != 2) {
	            Serial.print("P ");
	            Serial.print(mode);
	            Serial.print(" ");
	            Serial.print(difficulty);
	            Serial.print("\n");

	            // read back what you just sent
	            while (Serial.available() == 0) {}
	            // wait until you get a received message
	            if (Serial.read() == 'R') {
                    // read in the newline
                    Serial.read();
		            // setup the arduino to be ready to play
		            if (mode == 0) {
		            	curr_mode = COMPUTER_TURN;
		                currentPlayer = 0;
		            } else {
		            	curr_mode = PLAYER_TURN;
		                currentPlayer = 1;
		            }

                    // there is no computer input to read yet
                    noComputerInput = true;

		            // draw the board background here
		            drawGame(mode, difficulty);
		            drawMenu(mode, currentPlayer, difficulty, winner);
	            }
            } else {
            	// this is simple 2 player game so draw the setup
            	currentPlayer = 1;
            	curr_mode = PLAYER_TURN;
	            // draw the board background here
	            drawGame(mode, difficulty);
	            drawMenu(mode, currentPlayer, difficulty, winner);            	
            }
        }
        else if (curr_mode == PLAYER_TURN) {
        	// lastMove will keep track of what the player just did
            int lastMove = -1;
            while (lastMove == -1) {
                if (mode == 0) {
                    // this means player 1 goes second
                    lastMove = playerMove(player2, tft, game);
                } else {
                    // this means player 1 goes first
                    lastMove = playerMove(player1, tft, game);
                }
            }
        	if (mode == 0) {
                // we know the computer is playing so this is false
                noComputerInput = false;
                // check if the game is over
                if (game.checkWin(lastMove, tft)) {
                    curr_mode = END_GAME;
                    winner = "P1";

                    // computer can't play now
                    noComputerInput = true;

                    // communicate with desktop
                    Serial.print("W ");
                } else if (game.checkDraw()) {
                    curr_mode = END_GAME;
                    winner = "Draw";

                    // computer can't play now
                    noComputerInput = true;

                    // communicate with desktop
                    Serial.print("D ");             
                } else {
                	curr_mode = COMPUTER_TURN;

                    // communicate with desktop
                    Serial.print("M ");
                    currentPlayer = 0;     
                }
                // send over move with newline
                Serial.print(lastMove);
                Serial.print('\n');

                // update the menu
                drawMenu(mode, currentPlayer, difficulty, winner);

                // read in what you just wrote and computer if needed
                while (Serial.read() != '\n') {}
                if (!noComputerInput) {
                    while (Serial.read() != '\n') {}
                }

      		
        	} else if (mode == 1) {
                // there will be something to read from the computer here
                noComputerInput = false;   
                // check if the game is over
                if (game.checkWin(lastMove, tft)) {
                    curr_mode = END_GAME;
                    winner = "P1";

                    // makes sure the arduino knows it can proceed
                    noComputerInput = true;

                    // communicate with desktop
                    Serial.print("W ");
                } else if (game.checkDraw()) {
                    curr_mode = END_GAME;
                    winner = "Draw";

                    // makes sure the arduino knows it can proceed
                    noComputerInput = true;

                    // communicate with desktop
                    Serial.print("D ");           
                } else {
                	// the game is still in progress so let the computer go
                	curr_mode = COMPUTER_TURN;

                    // communicate with desktop
                    Serial.print("M ");
                    currentPlayer = 0;     
                }
                // send over move with newline
                Serial.print(lastMove);
                Serial.print("\n");

                // update the menu
                drawMenu(mode, currentPlayer, difficulty, winner); 



                // read in what you send (and computer if needed)
                while (Serial.read() != '\n') {}
                if (!noComputerInput) {
                    while (Serial.read() != '\n') {}
                }
                    
            } else {
                // check if the game is over in PVP
                if (game.checkWin(lastMove, tft)) {
                    curr_mode = END_GAME;
                    winner = "P1";
                } else if (game.checkDraw()) {
                    curr_mode = END_GAME;
                    winner = "Draw";  
                } else {
                	curr_mode = SECOND_PLAYER;
                	currentPlayer = 2;
                }
                // update the menu
                drawMenu(mode, currentPlayer, difficulty, winner);
            }
        }
        else if (curr_mode == COMPUTER_TURN) {
        	// wait for input
            while (Serial.available() == 0) {}
            char readIn = Serial.read();
            // make sure it is valid input
            if (readIn == 'W' || readIn == 'D' || readIn == 'M') {
            	// skip the whitespace and then save the number
                while (Serial.available() == 0) {}
            	Serial.read();
                while (Serial.available() == 0) {}

                // this holds the position where the computer plays
            	char inputChar = Serial.read();

                // turn the char into a string to easily convert it
                String input(inputChar);

            	// make it a number
            	int computerMove = input.toInt();

            	// insert a piece into the game
            	if (mode == 0) {
            		game.insertPiece(computerMove, player1, tft);
            	} else {
            		game.insertPiece(computerMove, player2, tft);
            	}
            	if (readIn == 'W') {
                    // set the winner title
            		winner = "AI";
                    // draw the line for the win
                    game.checkWin(computerMove, tft);
            		curr_mode = END_GAME;
            	} else if (readIn == 'D') {
            		// move on to end game
            		winner = "DRAW";
            		curr_mode = END_GAME;
            	} else {
            		curr_mode = PLAYER_TURN;
            		currentPlayer = 1;
            	}
            	drawMenu(mode, currentPlayer, difficulty, winner);
            }
            // wait until there is something in the serial monitor
            while (Serial.available() == 0) {}
        } 
        else if (curr_mode == SECOND_PLAYER) {
        	    // this will hold the move P2 makes
                int lastMove = -1;
                while (lastMove == -1) {
                    lastMove = playerMove(player2, tft, game);
                }

                // check if the game is over
                if (game.checkWin(lastMove, tft)) {
                    curr_mode = END_GAME;
                    winner = "P2";
                } else if (game.checkDraw()) {
                    curr_mode = END_GAME;
                    winner = "DRAW";
                }
                else {
                	// it is now P1s turn
                    curr_mode = PLAYER_TURN;
                    currentPlayer = 1;
                }
                // update the menu
                drawMenu(mode, currentPlayer, difficulty, winner);
        }
        else if (curr_mode == END_GAME) {
            // start by waiting 3 seconds to allow players to see winner
            delay(3000);

            // proceed back to the menu
            curr_mode = GAME_SELECTION;

            // make sure the menu is redrawn
            tft.fillScreen(TFT_BLACK);
            redrawScreen = true;
            startGame = false;
            noComputerInput = true;

            // reset the game board
            game.setup();
            winner = "???";
        }

    }

    Serial.flush();
    return 0;
}
