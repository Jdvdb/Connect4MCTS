#ifndef _MCTS_
#define _MCTS_

#include <chrono>
#include <unordered_map>

#include "ConnectNode.h"
#include "ConnectFour.h"

using namespace std;

/*
Main function that drives MCTS algorithm
Arguments:
    const ConnectFour& game: the current game state
    unordered_map<int, ConnectNode*>& tree: the tree used for MCTS
    char myColour: the colour the computer is in the current game
    int& currentId: the next ID value to be used to create children
    int rootId: the ID of the node with the current game state
    int rollouts: the number of rollouts to be performed in the search
    double greed: impacts how greedy ai is (greedier performs much worse)
Returns: the ID of the next best move
*/
int MCTS(const ConnectFour& game, unordered_map<int, ConnectNode*>& tree, char myColour, int& currentId, int rootId, int rollouts, double greed);

/*
Determines the next best node to travel to based on UCT
Arguments:
    ConnectNode* currentNode: the current node being checked in the tree
    unorederd_map<int, ConnectNode*>& tree: the tree used for MCTS
Returns: the ID of the most promising child node
*/
int UCT(ConnectNode* currentNode, unordered_map<int, ConnectNode*>& tree);

/*
Creates the children for a current leaf node
Arguments:
    ConnectNode* currentNode: the current node being investigated, will gain children nodes
    unordered_map<int, ConnectNode*>& tree: the tree used for MCTS, will gain more nodes
    char nextColour: the colour the pieces that will be played next
    int& currentId: the ID value that should be assigned to new children
Returns: none
*/
void expansion(ConnectNode* currentNode, unordered_map<int, ConnectNode*>& tree, char nextColour, int& currentId);

/*
Performs simulations until an end game is reached and returns the value of that game
Arguments: 
    ConnectNode* currentNode: the current node being investigated
    char myColour: the colour of computer pieces
    char playerColour: the colour of the player pieces
    double& depth: keeps track of the number of moves in the current game state
Returns: the win value for this rollout. Greater number means better scenario
*/
double rollout(ConnectNode* currentNode, char myColour, char playerColour, double& depth);

/*
Gives the win value of a rollout to all of the parent nodes above it
Arguments:
    ConnectNode* currentNode: the current node being investigated
    unordered_map<int, ConnectNode*>& tree: the tree being used for MCTS
    double win: the value to be passed to the winCount in each node
    int rootId: the ID of the root so that back propogation stops
Returns: None
*/
void backPropogation(ConnectNode* currentNode, unordered_map<int, ConnectNode*>& tree, double win, int rootId);

/*
Finds the best move to play
Arguments:
    ConnectNode* root: the root node
    unordered_map<int, ConnectNode*>& tree: the tree used for MCTS
Returns: the ID of the next most promising game state
*/
int maxChild(ConnectNode* root, unordered_map<int, ConnectNode*>& tree);


#endif