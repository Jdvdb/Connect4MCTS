#include "mcts.h"

using namespace std;

int MCTS(unordered_map<int, ConnectNode *> &tree, char myColour, int &currentId, int rootId, int rollouts, double greed)
{
	// generate random seed for simulations
	srand(time(0));

	// establish how long program can run for
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	end = chrono::system_clock::now();
	chrono::duration<double> timeElapsed = end - start;

	// set the player colour
	char playerColour;
	if (myColour == 'R')
	{
		playerColour = 'Y';
	}
	else
	{
		playerColour = 'R';
	}

	// the average depth of each iteration
	double depthAvg = 0;

	// keeps track of the current depth of this simulation
	double depth = 0;

	// establish the root of the tree. false means that opponent made the last move
	ConnectNode *root = tree.at(rootId);
	currentId++;

	// node that will be used to traverse down tree
	ConnectNode *currentNode = root;

	// perform rollouts using i to keep track of current iteration
	int i = 0;
	while (i < rollouts)
	{
		// start current node ath the root
		currentNode = tree.find(root->id)->second;

		// set the depth to 0;
		depth = 0;

		// first, iterate through tree based on UCT until children of current node is empty
		while (!currentNode->children.empty())
		{
			depth++;
			// use UST to determine where to go next
			int nextNode = UCT(currentNode, tree);
			currentNode = tree.at(nextNode);
		}

		char nextColour;

		if (currentNode->playerTurn)
		{
			nextColour = playerColour;
		}
		else
		{
			nextColour = myColour;
		}

		// check for valid neighbour games and if they exist, expand
		if (!currentNode->currentGame.checkWin(currentNode->newMove) && !currentNode->currentGame.checkDraw())
		{
			expansion(currentNode, tree, nextColour, currentId);
		}

		// get a random child
		// this is to make sure you don't go out of bounds
		if (currentNode->children.empty())
		{
			depthAvg += depth;
			bool isWin = currentNode->currentGame.checkWin(currentNode->newMove);
			if (currentNode->playerTurn == false)
			{
				isWin = !isWin;
			}
			bool isDraw = currentNode->currentGame.checkDraw();
			if (isDraw)
			{
				backPropogation(currentNode, tree, 0, rootId);
			}
			else if (isWin)
			{
				backPropogation(currentNode, tree, greed / depth, rootId);
			}
			else
			{
				backPropogation(currentNode, tree, -1 / depth, rootId);
			}

			// move on to the next iteration
			i++;
		}
		else
		{
			int randChildId = *currentNode->children.begin();
			currentNode = tree.at(randChildId);
			depth++;

			// perform a simulation based on this child node and collect the win value
			double simulationWin = rollout(currentNode, myColour, playerColour, depth);

			// add the current depth to the depthAvg value
			depthAvg += depth;

			// if the simulation won, add greed to it
			if (simulationWin > 0)
			{
				simulationWin *= greed;
			}

			// back propogate the win value up the tree
			backPropogation(currentNode, tree, simulationWin, rootId);

			// move onto next iteration
			i++;
		}
	}
	// update timer
	end = chrono::system_clock::now();
	timeElapsed = end - start;

	// print time taken and tree size
	cout << "Time: " << timeElapsed.count() << endl;
	cout << "Tree size: " << tree.size() << endl;

	// set currentNode to the root and find its best child
	currentNode = root;
	int bestNodeId = maxChild(currentNode, tree);

	// output the average depth and return the best id
	cout << "Avg depth: " << depthAvg / rollouts << endl;
	return bestNodeId;
}

int UCT(ConnectNode *currentNode, unordered_map<int, ConnectNode *> &tree)
{
	// this is the value that will be returned
	int id;
	// this will keep track of the current best node's value
	double maxUCT = -10;
	// this is the number of times the parent node was visited
	double parentSimulations = currentNode->visitCount;

	// iterate through all the children
	for (auto iter = currentNode->children.begin(); iter != currentNode->children.end(); iter++)
	{
		// currentNode is the child being investigated
		ConnectNode *currentNode = tree.find(*iter)->second;

		// values to determine whether a node should be explored
		double currentUCT, exploit, explore;

		// these are the two parts of the UCT formula
		exploit = currentNode->winCount / currentNode->visitCount;
		explore = 20 * sqrt(log(parentSimulations) / currentNode->visitCount);

		// determine if the UCT value of this node is the best so far
		currentUCT = exploit + explore;
		if (maxUCT < currentUCT)
		{
			id = *iter;
			maxUCT = currentUCT;
		}
	}
	return id;
}

void expansion(ConnectNode *currentNode, unordered_map<int, ConnectNode *> &tree, char nextColour, int &currentId)
{
	// iterate through all 7 possible piece placement positions
	for (int neighbours = 0; neighbours < 7; neighbours++)
	{
		// make a temporary connect 4 game with a possible move
		ConnectFour tempGame = currentNode->currentGame;

		// make sure putting a piece in this column is okay
		if (tempGame.insertPiece(neighbours, nextColour))
		{
			// add this new game state to the tree
			tree.insert({currentId, new ConnectNode(tempGame, !currentNode->playerTurn, currentId, currentNode->id, neighbours)});
			currentNode->addChild(currentId);

			// update currentId for next piece added
			currentId++;
		}
	}
}

double rollout(ConnectNode *currentNode, char myColour, char playerColour, double &depth)
{
	// temp node that will be used for the simulation
	ConnectNode tempNode = *currentNode;

	// set the current player colour
	char colour;
	if (tempNode.playerTurn == true)
	{
		colour = myColour;
	}
	else
	{
		colour = playerColour;
	}

	// determines if this game is done
	bool finished = false;
	// this is only raised if the end state is a draw
	bool draw = false;

	while (!finished)
	{
		// play a random move
		int play = rand() % 7;
		tempNode.currentGame.insertPiece(play, colour);

		// check if this game is done
		if (tempNode.currentGame.checkDraw())
		{
			finished = true;
			draw = true;
		}
		else if (tempNode.currentGame.checkWin(play))
		{
			finished = true;
		}

		// flip the colour for the next move
		if (colour == 'R')
		{
			colour = 'Y';
		}
		else
		{
			colour = 'R';
		}

		// increase the current depth value
		depth++;
	}

	// first check if it was a draw
	if (draw)
	{
		return 0;
	}

	// otherwise, we know that !colour won
	if (colour == playerColour)
	{
		return 2 / depth;
	}
	else
	{
		return -1 / depth;
	}
}

void backPropogation(ConnectNode *currentNode, unordered_map<int, ConnectNode *> &tree, double win, int rootId)
{
	// multiplier for the win score
	int winMultiplier = 5;

	// iterate until you hit the current rootId
	while (currentNode->id != rootId)
	{
		// add one to the visit count
		currentNode->visitCount += 1;

		// add to the win value if this is this is when the computer would play
		if (currentNode->playerTurn)
		{
			currentNode->winCount += winMultiplier * win;
		}

		// set the current node to its parent
		currentNode = tree.at(currentNode->parent);
	}

	// update the current node's information as well
	currentNode->visitCount += 1;
	currentNode->winCount += winMultiplier * win;
}

int maxChild(ConnectNode *root, unordered_map<int, ConnectNode *> &tree)
{
	// values for the max win score and that score's ID
	double max = -10;
	int maxId = 0;

	// iterate through all children of the root
	for (auto iter = root->children.begin(); iter != root->children.end(); iter++)
	{
		// tempnode is the current child being looked at
		ConnectNode *tempNode = tree[*iter];

		// calculate the winValue for this node
		double winLossRatio = tempNode->winCount / tempNode->visitCount;

		// test if it's the best node so far
		if (winLossRatio > max)
		{
			max = winLossRatio;
			maxId = *iter;
		}
		// read out the value to the console
		cout << tempNode->newMove << " ratio: " << winLossRatio << ". plays: " << tempNode->visitCount << endl;
	}
	return maxId;
}
