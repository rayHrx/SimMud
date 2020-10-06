
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

/* see AStar.h for details about this class */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "AStar.h"

static int dxp[4] = { -1, 1, 0, 0 };
static int dyp[4] = { 0, 0, -1, 1 };

/***************************************************************************************************
*
* Constructor and setup
*
***************************************************************************************************/

AStar::AStar()
{
	matrix = NULL;
	m = n = 0;
	n2 = m2 = 0;
	second_matrix = NULL;
	offx = offy = 0;
}

void AStar::setMatrix(int n, int m, char **matrix)
{
	this->matrix = matrix;
	this->n = n;
	this->m = m;
}

void AStar::setSecondMatrix(int n2, int m2, char **second_matrix, int offx, int offy)
{
	this->n2 = n2;
	this->m2 = m2;
	this->second_matrix = second_matrix;
	this->offx = offx;
	this->offy = offy;
}

void AStar::createPath(Node *current)
{
	Node *q = current;
	while ( q != NULL )
	{
		if ( q->path_next != NULL ) /* don't add source node */
			path.push(q->x,q->y);
		q = q->path_next;
	}
}

/***************************************************************************************************
*
* Main pathfinding method ( see AStar.h for details )
*
***************************************************************************************************/

PointStack* AStar::findPathAStar(int startx, int starty, int stopx, int stopy,
	int max_explored, bool return_partial_path)
{
	/* check that data are correct */
	if ( matrix == NULL || n <= 0 || m <= 0
		|| startx < 0 || startx >= n
		|| starty < 0 || starty >= m
		|| stopx < 0 || stopx >= n
		|| stopy < 0 || stopy >= m ) return &path;
	if ( matrix[stopx][stopy] != 0 ) return &path;

	/* estimate the distance between the current point and the destination */
	#define estimate(x,y) ( abs( x - stopx ) + abs( y - stopy ) )
	/* the current node is the solution if it has the specified coordinates */
	#define is_solution(x,y) ( x == stopx && y == stopy )

	Node *node;	/* starting node */
	Node *current;	/* node currently explored */
	Node *next,*q;
	Node *closest;	/* closest node to the destination (used to return a partial path) */
	int i,x,y;
	int x2,y2;	/* coordinates in the second matrix */
	int he,ge;	/* estimate for h, and new g */

	/* create starting node */
	node = new Node( startx, starty );
	node->ghf( 0, estimate(startx,starty) );
	closest = node;

	open.add(node);
	current = NULL;

	/* explore the space around */
	while ( !open.empty() )
	{
		/* get best node (best guess) */
		current = open.getMin();

		/* check if this is the destination */
		if ( is_solution(current->x, current->y) ) break;

		/* add node to explored nodes */
		close.add(current);

		/* check if this node is the closest (used for partial path) */
		if ( closest->h > current->h ) closest = current;

		/* abort A* if limit is reached */
		if ( close.size() > max_explored )
		{
			if ( return_partial_path ) createPath(closest);
			open.clearNodes();
			close.eraseAll();
			return &path;
		}

		/* get nodes from all 4 directions */
		for ( i = 0; i < 4; i++ )
		{
			/* coordinates of new node */
			x = current->x + dxp[i];
			y = current->y + dyp[i];
			ge = current->g + 1;
			he = estimate(x,y);

			/* check coodinates */
			if ( x < 0 || x >= n || y < 0 || y >= m ) continue;
			if ( matrix[x][y] != 0 ) continue;
			if ( second_matrix != NULL )	/* check second matrix if provided */
			{
				x2 = x - offx;
				y2 = y - offy;
				if ( x2 >= 0 && y2 >= 0 && x2 < n2 && y2 < m2 )
					if ( second_matrix[x2][y2] != 0 ) continue;
			}

			/* check if the node is already in open */
			q = open.find(x,y);
			if ( q != NULL )
			{
				if ( q->g > ge )
				{
					q->ghf(ge,he);
					q->path_next = current;
				}
				continue;
			}

			/* check if the node is already in close */
			q = close.find(x,y);
			if ( q != NULL )
			{
				if ( q->g > ge )
				{
					close.remove(x,y);
					q->ghf(ge,he);
					q->path_next = current;
					open.add(q);
				}
				continue;
			}

			/* create new node and add it to open */
			next = new Node(x,y);
			next->ghf(ge,he);
			next->path_next = current;
			open.add(next);
		}
	}

	/*
	printf("A* %d,%d->%d,%d found=%s\n",
		startx,starty, stopx,stopy,
		open.empty()?"false":"true");
	*/

	/* no path found */
	if ( open.empty() )
	{
		if ( return_partial_path ) createPath(closest);
		open.clearNodes();	/* no nodes but use this call to free memory */
		close.eraseAll();
		return &path;
	}

	/* create path */
	createPath(current);
	open.clearNodes();
	close.eraseAll();

	return &path;
}

/***************************************************************************************************
*
* Convenience methods
*
***************************************************************************************************/

PointStack* AStar::findPath(int startx, int starty, int stopx, int stopy)
{
	path.clear();
	return findPathAStar(startx,starty, stopx,stopy, 1024, false);
}

PointStack* AStar::findPathNear(int startx, int starty, int stopx, int stopy)
{
	path.clear();
	return findPathAStar(startx,starty, stopx,stopy, 128, true);
}

PointStack* AStar::findPathTh(int startx, int starty, int stopx, int stopy, int explore_size)
{
	path.clear();
	return findPathAStar(startx,starty, stopx,stopy, explore_size, true);
}
