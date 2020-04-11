#ifndef _CONNECT_TREE_
#define _CONNECT_TREE_

#include <set>
#include <math.h>
#include "ConnectFour.h"

using namespace std;

class ConnectNode {
public:
    /*
    Constructor that initializes values for the current node
    Arguments:
        const ConnectFour& inputGame: the current game state of this node
        const bool currentPlayer: whoever played the piece to get into this game state
        int myId: the id for this node
        int parentId: the id for this node's parent
        int move: the move from the parent that got it to this game state (0-6)
    Returns: none
    */
	ConnectNode(const ConnectFour& inputGame, const bool currentPlayer, int myId, int parentId, int move);
	
	/*
    Will add the id of a child to the current node
    Arguments: int childId: the id of the child node
    */
	void addChild(int childId);

    // id's of all the children
    set<int> children;

    // game state of the node
    ConnectFour currentGame;

    // id for this game and it's parent. newMove is the last move played
    int id, parent, newMove;

    // true for player, false for computer
    bool playerTurn;

    // keeps track of total times node is visited in simulation and how many wins
    double visitCount = 1, winCount = 0, uctValue;

};
#endif