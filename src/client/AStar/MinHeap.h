
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
*
* AUTHOR:
*    Mihai Paslariu
*    Politehnica University of Bucharest, Bucharest, Romania
*    mihplaesu@yahoo.com
*
* TIME AND PLACE:
*    University of Toronto, Toronto, Canada
*    March - August 2007
*
***************************************************************************************************/

/***************************************************************************************************
*
* A minHeap class for the A* algorithm
* - used for the open set of nodes in A* (see AStar.cpp)
* - minHeap gets from the heap the node with the lowest 'f' value (see Node.h)
* - this container doubles its capacity when there is not enough space to hold new nodes
* - two heapify methods:
*	- heapify1 - used when a element in the root doesn't satisfy the min heap property
*	- heapify2 - used when the last leaf doesn't satisfy the min heap property
* - because the heap is a balanced tree we use a vector to store nodes
*	- the parent for a node i is (i-1)/2
*	- the right and left sons of a node are i*2+1 and i*2+2
* - add and getMin are performed in logarithmic time, find is performed in liniar time
*
***************************************************************************************************/

#ifndef __ASTAR_HEAP_H
#define __ASTAR_HEAP_H

#include "Node.h"

class MinHeap
{
private:
	Node **first;
	int n;
	int cap;

	/* private methods: used for mantaining the heap property */
	void heapify1(int i);	/* from root to leafs */
	void heapify2(int i);	/* from leafs to root */

public:
	/* constructor/destructor */
	MinHeap() { first = NULL; n = 0; cap = 0; }
	~MinHeap() { clear(); }

	/* heap operations */
	void add(Node *nou);
	void eraseVector();
	void eraseNodes();
	void clear();
	void clearNodes();
	Node* getMin();
	Node* find(int x, int y);
	bool empty();
	int size();
};

#endif
