
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
* NodeSet Class
* - a set of nodes for the A* algorithm: used for 'close' (see AStar.cpp)
* - this class uses the STL set class with a custom comparator
* - the find and erase method are called with the coordinates instead of another node object for
* convenience
*
***************************************************************************************************/

#ifndef __NODE_SET_H
#define __NODE_SET_H

#include "Node.h"

#include <set>
using namespace std;

/* custom comparator for nodes */
struct NodeComparator
{
	bool operator()( Node *n1, Node *n2 ) const
	{
		//return ( ( n1->x != n2->x ) ? false : ( n1->y == n2->y ) );
		//return  (n1->x > n2->x) ? true : (n1->y > n2->y);
		if ( n1->x != n2->x )
			return n1->x > n2->x;
		else
			return n1->y > n2->y;
	}
};

#define StlNodeSet set<Node*,NodeComparator>

/* set of nodes ( used instead of set<Node*> for convenience ) */
class NodeSet
{
private:
	StlNodeSet nset;

public:
	void add(Node *nou);
	Node* find(int x, int y);
	void remove(int x, int y);
	void clear();
	void eraseAll();
	int size();
	bool empty();
};

#endif
