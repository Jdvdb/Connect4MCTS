#include "mcts.h"
#include "ConnectNode.h"
#include "ConnectFour.h"

using namespace std;

int main() {
	// tree object for computer
	unordered_map<int, ConnectNode*> tree;

	// game to be played
	ConnectFour game;

	// the id to be used next when creating nodes
	int currentId = 0;

	// root node for computer, starts with other player
	ConnectNode* root = new ConnectNode(game, false, currentId, -1, -1);
	currentId++;

	tree.insert({root->id, root});

	// will remember the last move made by the human in the game
	int lastPlayerMove;

	// the id of the next move chose by the computer
	int newComputerId;

	// value that affects how greedy AI is
	double greed = 1;

	// the way the first move is computed for the computer depends on if the player has gone first or not
	bool firstMove = true;

	// number of rollouts in each mcts call, based on difficulty
	int rollouts = 20000;

	// first determine who is starting
	int playerStart;
	// the characters for the player and computer
	char playerColour, computerColour;

	// ask for who starts
	cout << "Input who will start. 0 for computer, 1 for player" << endl;
	// true if player starts
	bool turn;
	cin >> playerStart;
	if (playerStart == 1) {
		playerColour = 'R';
		computerColour = 'Y';
		turn = true;
	} else {
		playerColour = 'Y';
		computerColour = 'R';
		turn = false;
		firstMove = false;
	}

	// ask for the dificulty
	cout << "Input difficulty. 0 for easy, 1 for hard, anything else for doom" << endl;
	// 0 for easy, 1 for medium, 2 for hard
	int difficulty;
	cin >> difficulty;
	if (difficulty == 0) {
		rollouts = 3000;
		greed = 3.2;
	} else if (difficulty == 1) {
		rollouts = 8000;
		greed = 1.8;
	} else {
		rollouts = 20000;
		greed = 1;
	}

	cout << "Starting game! Don't forget to play moves between 0 and 6" << endl;

	// if they are starting, draw the empty board
	if (turn == 1) {
		game.niceReadArray();
	}

	while (true) {
		// player turn
		if (turn) {
			// get data on where they are playing a piece
			cout << "Please play a move: ";
			int move;
			cin >> move;
			char player = playerColour;
			lastPlayerMove = move;

			// cout << "move: " << move << " player: " << player << endl;

			if (game.insertPiece(move, player)) {
				game.niceReadArray();
				bool win = game.checkWin(move);
				bool draw = game.checkDraw();

				turn = !turn;

				if (firstMove) {
					// we can just change the root node so that mcts starts at the right spot
					tree.erase(root->id);
					root->currentGame = game;
					root->newMove = move;
					tree.insert({root->id, root});

					firstMove = false;
				} else {
					// we need to set the root based on the last move
					auto iter = root->children.begin();
					bool foundNewRoot = false;
					while (iter != root->children.end() && !foundNewRoot) {
						if (tree.find(*iter)->second->newMove == move) {
							foundNewRoot = true;
							root = tree.find(*iter)->second;
						}
						iter++;
					}
					if (!foundNewRoot) {
						cout << "BIG OOPS" << endl;
					}
				}

				if (win) {
					cout << "game over! player won!" << endl;
					break;
				} else if (draw) {
					cout << "draw" << endl;
				}
			} else {
				cout << "Invalid move, column full. Please try again" << endl;
			}
		} else {
			cout << "computer is thinking..." << endl;

			newComputerId = MCTS(game, tree, computerColour, currentId, root->id, rollouts, greed);

			root = tree.at(newComputerId);

			cout << "tree size: " << tree.size() << endl;

			lastPlayerMove = root->newMove;

			cout << "last move: " << lastPlayerMove << endl;

			game.insertPiece(lastPlayerMove, computerColour);

			game.niceReadArray();
			bool win = game.checkWin(lastPlayerMove);
			bool draw = game.checkDraw();

			turn = !turn;

			// fix the root so that it matches the computer's decision


			if (win) {
				cout << "game over! computer won!" << endl;
				break;
			} else if (draw) {
				cout << "draw" << endl;
			}
		}

	}





	return 0;
}