#include "ConnectNode.h"

using namespace std;

ConnectNode::ConnectNode(const ConnectFour& inputGame, const bool currentPlayer, int myId, int parentId, int move) {
	// set these based on input
	currentGame = inputGame;
	playerTurn = currentPlayer;
	id = myId;
	parent = parentId;
	newMove = move;

}

void ConnectNode::addChild(int childId) {
	children.insert(childId);
}