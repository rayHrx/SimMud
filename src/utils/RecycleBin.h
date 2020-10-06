
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

#ifndef __RECYCLE_BIN_H
#define __RECYCLE_BIN_H

#include <list>
using namespace std;

#include "Garbage.h"

/***************************************************************************************************
*
* RecycleBin class
* - this class contains a list of objects that will be deleted
* - all threads must mark these objects after they have finished using them, so the objects can
*   be safely deleted
* - the constructor receives the number of threads that use these objects
* - [IMPORTANT] each thread must call mark objects only one time !!! => acceptIt can help with
*   this by marking only objects from the interator to the end of the list (threads mus hold
*   their last position)
*
***************************************************************************************************/

class RecycleBin
{
protected:
	int nthreads;		/* number of threads that must mark the object as disposable */
	SDL_mutex *mutex;	/* synchronize access to the list */
	list<Garbage*> gl;	/* list of objects to be deleted */

public:
	/* Constructor / Destructor */
	RecycleBin( int max_threads );
	~RecycleBin();
	void setNumberOfThreads(int x);

	/* add a new object to the list */
	void add(Garbage *g);

	/* delete all objects marked by all threads */
	void empty();
	list<Garbage*> emptyWithList();

	/* mark all objects or only some of them */
	void accept();
	void acceptIt(Garbage *&old_ptr);
};

#endif
