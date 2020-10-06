
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

/* see NodeSet.h for details about this class */

#include <stdio.h>
#include <stdlib.h>
#include "NodeSet.h"

void NodeSet::add(Node *nou)
{
	nset.insert(nou);
}

Node *NodeSet::find(int x, int y)
{
	Node n(x,y);
	StlNodeSet::iterator it = nset.find(&n);
	if ( it == nset.end() ) return NULL;
	return *it;
}

void NodeSet::remove(int x, int y)
{
	Node n(x,y);
	nset.erase(&n);
}

void NodeSet::clear()
{
	nset.clear();
}

void NodeSet::eraseAll()
{
	StlNodeSet::iterator it;
	for ( it = nset.begin(); it != nset.end(); it++ )
		delete *it;
	nset.clear();
}

int NodeSet::size()
{
	return nset.size();
}

bool NodeSet::empty()
{
	return nset.empty();
}

