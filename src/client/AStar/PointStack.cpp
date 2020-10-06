
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

/* see PointStack.h for details about this class */

#include <stdio.h>
#include <string.h>
#include "PointStack.h"

PointStack::PointStack()
{
	x = y = NULL;
	n = cap = 0;
}

PointStack::~PointStack()
{
	if ( x != NULL ) delete x;
	if ( y != NULL ) delete y;
}

void PointStack::push(int nx, int ny)
{
	/* the first time the stack is used */
	if ( cap == 0 )
	{
		cap = 256;
		x = new int[cap];
		y = new int[cap];
		if ( x == NULL || y == NULL )
		{
			cap = 0;
			return;
		}
	}

	/* when there is not enough space the stack doubles itself */
	if ( n + 1 >= cap )
	{
		int *x2,*y2,cap2;
		cap2 = 2 * cap;
		x2 = new int[cap2];
		y2 = new int[cap2];
		memcpy(x2,x, cap*sizeof(int));
		memcpy(y2,y, cap*sizeof(int));
		delete x;
		delete y;
		x = x2;
		y = y2;
		cap = cap2;
	}

	/* add the new point on top of the stack */
	x[n] = nx;
	y[n] = ny;
	n++;
}

int PointStack::getX()
{
	if ( n > 0 && n <= cap ) return x[n-1];
	return 0;
}

int PointStack::getY()
{
	if ( n > 0 && n <= cap ) return y[n-1];
	return 0;
}

void PointStack::pop()
{
	if ( n > 0 ) n--;
}

int PointStack::size()
{
	return n;
}

bool PointStack::empty()
{
	return n == 0;
}

void PointStack::clear()
{
	n = 0;
}
