#include <cassert>
#include <sstream>

#include "mcts.h"
#include "ConnectNode.h"
#include "ConnectFour.h"
#include "serialport.h"

using namespace std;

int main() {
	// tree object for computer
	unordered_map<int, ConnectNode*> tree;

	// game to be played
	ConnectFour game;

	// the id to be used next when creating nodes
	int currentId = 0;

	// setup the serial port for communication with the arduino
	SerialPort Serial("/dev/ttyACM0");

	// root node for computer, starts with other player
	ConnectNode* root = new ConnectNode(game, false, currentId, -1, -1);
	currentId++;

	tree.insert({root->id, root});

	// will remember the last move made by the human in the game
	int lastPlayerMove;

	// the id of the next move chose by the computer
	int newComputerId;

	// value that affects how greedy AI is
	double greed;

	// the way the first move is computed for the computer depends on if the player has gone first or not
	bool firstMove = true;

	// number of rollouts in each mcts call, based on difficulty
	int rollouts = 0;

    // variable to be used whenever the Arduino
    string arduinoRequests;

    // string stream to help handle input
    stringstream ss;

	// the characters for the player and computer
	char playerColour, computerColour;

	/*
	the different states the game can be in
	WAIT_FOR_START: wait until arduino gives games setup
	PLAYER_MOVE: accept arduino information and update the tree
	COMPUTER_MOVE: runs mcts, updates arduino and fixes tree
	RESET: prepare to receive information for a new game
	*/
	enum {WAIT_FOR_START, PLAYER_MOVE, COMPUTER_MOVE, RESET} curr_mode = WAIT_FOR_START;

	while (true) {
		if (curr_mode == WAIT_FOR_START) {
			cout << "Waiting for Arduino to send game information..." << endl;
	        // first make sure they send a request to communicate
	        do {
	            ss = stringstream();
	            ss << Serial.readline();
	            ss >> arduinoRequests;
	        } while (arduinoRequests != "P");

	        // read in the mode and difficulty
	        int mode, difficulty;
	        ss >> mode >> difficulty;

	        if (mode == 0) {
	        	// computer starts
				playerColour = 'Y';
				computerColour = 'R';
				curr_mode = COMPUTER_MOVE;
				firstMove = false;       	
	        } else {
	        	// player starts
				playerColour = 'R';
				computerColour = 'Y';
				curr_mode = PLAYER_MOVE;     	
	        }

	        // easy is 0. else is doom
	        if (difficulty == 0) {
	        	// these values were found through some testing
	        	rollouts = 1000;
	        	greed = 2.8;
	        } else {
	        	rollouts = 5000;
	        	greed = 1;
	        }

	        // send message to confirm to arduino
	        assert(Serial.writeline("R\n"));
	        cout << "Ready to play" << endl;

		} else if (curr_mode == PLAYER_MOVE) {
			// wait for information from the arduino
			do {
				// clear the stream
	            ss = stringstream();
	            ss << Serial.readline();
	            ss >> arduinoRequests;
	        } while (arduinoRequests != "M" && arduinoRequests != "W" && arduinoRequests != "D");
	        
	        // if the oponenet won or drew then just end this game
	        if (arduinoRequests == "W" || arduinoRequests == "D") {
	        	curr_mode = RESET;
	        } else {
	        	// read in the move and update the game
	        	int move;
	        	ss >> move;
	        	lastPlayerMove = move;

	        	game.insertPiece(move, playerColour);

	        	// if this is the first move, we need to alter the root
				if (firstMove) {
					// we can just change the root node so that mcts starts at the right spot
					tree.erase(root->id);
					root->currentGame = game;
					root->newMove = move;
					tree.insert({root->id, root});

					firstMove = false;
				} else {
					// we need to set the root based on the last move by searching the tree
					auto iter = root->children.begin();
					bool foundNewRoot = false;
					while (iter != root->children.end() && !foundNewRoot) {
						if (tree.find(*iter)->second->newMove == move) {
							foundNewRoot = true;
							root = tree.find(*iter)->second;
						}
						iter++;
					}
				}
				// the game isn't done here so let the computer think
				curr_mode = COMPUTER_MOVE;
	        }

		} else if (curr_mode == COMPUTER_MOVE) {
			// run mcts and find next move
			newComputerId = MCTS(game, tree, computerColour, currentId, root->id, rollouts, greed);
			// change the root to the newest move
			root = tree.at(newComputerId);

			// record where the comupter decided the best move is
			lastPlayerMove = root->newMove;

			// insert this piece in the game
			game.insertPiece(lastPlayerMove, computerColour);

			game.readArray();
			bool win = game.checkWin(lastPlayerMove);
			bool draw = game.checkDraw();

			curr_mode = PLAYER_MOVE;


			if (win) {
				// clear the stringstream
				ss.clear();
			    ss << "W " << lastPlayerMove << "\n";
				assert(Serial.writeline(ss.str()));

				// move into the reset phase
				curr_mode = RESET;
			} else if (draw) {
				// clear the stringstream
				ss.clear();
			    ss << "D " << lastPlayerMove << "\n";
				assert(Serial.writeline(ss.str()));

				// move into reset phase
				curr_mode = RESET;
			} else {
				// clear the stringstream
				cout << "Computer Finished. Waiting for Human" << endl;
				ss.clear();
			    ss << "M " << lastPlayerMove << "\n";
				assert(Serial.writeline(ss.str()));

				// move into waiting for player response
				curr_mode = PLAYER_MOVE;
			}
		} else if (curr_mode == RESET) {
			cout << "Game Done" << endl;
			// reset the tree and game
			tree.clear();
			game.setup();

			currentId = 0;

			// reset the root 
			root = new ConnectNode(game, false, currentId, -1, -1);
			currentId++;

			// begin building the tree again
			tree.insert({root->id, root});

			// set first move to true again
			firstMove = true;

			curr_mode = WAIT_FOR_START;
		}
	}

	return 0;
}