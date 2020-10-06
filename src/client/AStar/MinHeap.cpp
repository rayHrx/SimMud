
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

/* see MinHeap.h for details about this class */

#include <string.h>
#include "MinHeap.h"

/***************************************************************************************************
*
* Heapify methods
*
***************************************************************************************************/

/* from root to leafs (recursive method: i is the root of the tree) */
void MinHeap::heapify1(int i)
{
	int l,r,min;
	Node *aux;

	/* get left/right nodes */
	l = i * 2 + 1;
	r = i * 2 + 2;

	/* get the lowest value */
	if ( l < n && first[l]->f < first[i]->f )
		min = l;
	else
		min = i;
	if ( r < n && first[r]->f < first[min]->f )
		min = r;

	/* put the lowest value in the root */
	if ( i != min )
	{
		aux = first[i];
		first[i] = first[min];
		first[min] = aux;
		heapify1(min);
	}
}

void MinHeap::heapify2(int i)
{
	int p;
	Node *aux;

	/* the end of recursive calls is when the root is reached */
	if ( i <= 0 ) return;

	/* get parent node */
	p = (i - 1) / 2;

	/* put the lowest value in the parent */
	/* if parent already has the lowest value => stop */
	if ( first[i]->f < first[p]->f )
	{
		aux = first[i];
		first[i] = first[p];
		first[p] = aux;
		heapify2(p);
	}
}

/***************************************************************************************************
*
* Heap operations
*
***************************************************************************************************/

void MinHeap::add(Node *nou)
{
	/* check for empty node */
	if ( nou == NULL ) return;

	/* if this is the first node in the heap */
	if ( first == NULL )
	{
		n = 1;
		cap = 256;
		first = new Node*[cap];
		first[0] = nou;
		return;
	}

	/* double the size of the heap if there is not enogh space for the new node */
	if ( n >= cap )
	{
		int cap2 = cap * 2;
		Node **first2 = new Node*[cap2];
		memcpy(first2, first, cap*sizeof(Node*));
		delete first;
		first = first2;
		cap = cap2;
	}

	/* add the new node at the end of the tree */
	first[n] = nou;
	n++;

	/* call heapify to mantain the min heap property */
	heapify2(n-1);
}

/* don't delete all nodes, but delete the heap */
void MinHeap::eraseVector()
{
	delete first;
	first = NULL;
	n = 0;
	cap = 0;
}

/* delete all nodes, then clear the heap */
void MinHeap::eraseNodes()
{
	for ( int i = 0; i < n; i++ )
		delete first[i];
	delete first;
	first = NULL;
	n = 0;
	cap = 0;
}

/* clear the heap without deleting the nodes and the main vector */
void MinHeap::clear()
{
	n = 0;
}

/* delete the nodes but not the heap */
void MinHeap::clearNodes()
{
	for ( int i = 0; i < n; i++ )
		delete first[i];
	n = 0;
}

/* get the node with the lowes value */
Node* MinHeap::getMin()
{
	Node *p;

	/* if the heap doesn't contain any nodes */
	if ( n < 1 ) return NULL;

	/* the heap contais exactly one node */
	if ( n == 1 )
	{
		p = first[0];
		n = 0;
		return p;
	}

	/* get the node from the root */
	n--;
	p = first[0];

	/* put the last leaf in the root, then call heapify */
	first[0] = first[n];
	heapify1(0);
	return p;
}

/* find the node with the specified coordinates */
/* - unfortunately by iterating though the whole heap */
Node* MinHeap::find(int x, int y)
{
	for ( int i = 0; i < n; i++ )
		if ( first[i]->x == x && first[i]->y == y )
			return first[i];
	return NULL;
}

/* check if the heap is empty */
bool MinHeap::empty()
{
	return n == 0;
}

/* the size of the heap */
int MinHeap::size()
{
	return n;
}

