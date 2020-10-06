
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
* Node Class
* - node for the A* algorithm
* - 'x,y' are the coordinates of the node
* - 'g' is the distance from the start point
* - 'h' is the estimated distance to the destination
* - 'f = g + h' is the entire distance
* - 'next' and 'path_next' are used by other classes
* - 'left' and 'right' are used for the binary tree implementation
* - member methods are simple and will be used inline
*
***************************************************************************************************/

#ifndef __ASTAR_NODE_H
#define __ASTAR_NODE_H

class Node
{
public:
	int x,y;
	int g,h,f;
	Node *next;
	Node *path_next;
	Node *left,*right;

	inline Node( int x1, int y1 )
	{
		x = x1;
		y = y1;
		g = h = f = 0;
		next = NULL;
		path_next = NULL;
		left = right = NULL;
	}

	inline Node( int x1, int y1, int g1, int h1 )
	{
		x = x1;
		y = y1;
		g = g1;
		h = h1;
		f = g + h;
		next = NULL;
		path_next = NULL;
		left = right = NULL;
	}

	inline void ghf(int g1, int h1)
	{
		g = g1;
		h = h1;
		f = g + h;
	}
};

#endif
