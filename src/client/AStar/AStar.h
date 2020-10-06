
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
* Implementation of the A* pathfinding algorithm
* - we use a matrix of size m x n where 0 means empty cell where the player can move, and != 0
* means a blocked cell
* - the programmer must first call setMatrix before findPath
* - the pathfinding method returns a stack of points => then the user can call stack.pop to get the
* next point in the path
* - the stack returned doesn't contain any points if a path is not found
* - the second matrix cand be used to provide aditional blocked cells (eg. for object that change
* position in the game)
*
***************************************************************************************************/


#ifndef __ASTAR_H
#define __ASTAR_H

#include "Node.h"
#include "NodeSet.h"
#include "PointStack.h"
#include "MinHeap.h"

class AStar
{
private:
	/* main matrix (map) */
	int n,m;	/* size of map */
	char **matrix;	/* map used for searches */

	/* second matrix (eg. for dynamic objects) */
	int n2,m2;		/* size of second matrix */
	char **second_matrix;	/* second matrix */
	int offx,offy;		/* offset of the second matrix */

	MinHeap open;
	NodeSet close;
	PointStack path;

private:
	void createPath(Node *current);

public:
	/* constructor and setup */
	AStar();
	void setMatrix(int n, int m, char **matrix);
	void setSecondMatrix(int n2, int m2, char **second_matrix, int offx, int offy);

	/* pathfinding method
	   - returns a queue of points
	   - the queue doesn't contain any points if no path is found
	   - max_explored is the maximum number of locations explored
	   ( use a very large number to search the whole space )
	   - if return_partial_path is true and no path is found, the method returns
	   a path that doesn't lead to the destination but is in the right direction
	*/
	PointStack* findPathAStar(int startx, int starty,
		int stopx, int stopy,
		int max_explored, bool return_partial_path);

	/* convenience methods */
	PointStack *findPath(int startx, int starty, int stopx, int stopy);
	PointStack *findPathNear(int startx, int starty, int stopx, int stopy);
	PointStack *findPathTh(int startx, int starty, int stopx, int stopy, int explore_size);
};

#endif
