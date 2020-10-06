
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

#include <SDL.h>
#include "RecycleBin.h"

/***************************************************************************************************
*
* Constructor / Destructor
*
***************************************************************************************************/

RecycleBin::RecycleBin( int max_threads )	/* number of threads that must mark objects */
{
	nthreads = max_threads;
	mutex = SDL_CreateMutex();
}

RecycleBin::~RecycleBin()
{
	SDL_DestroyMutex(mutex);
}

void RecycleBin::setNumberOfThreads(int x)
{
	nthreads = x;
}

/***************************************************************************************************
*
* add(g)
* - add a new object to the list
*
***************************************************************************************************/

void RecycleBin::add( Garbage *g )
{
	/* lock list */
	SDL_LockMutex(mutex);

	/* check if the item added is already in the list */
	for ( list<Garbage*>::iterator it = gl.begin(); it != gl.end(); it++ )
	{
		Garbage *gv = *it;
		if ( (void*)gv == (void*)g )	/* pointer compare */
		{
			/* unlock list and exit */
			SDL_UnlockMutex(mutex);
			return;
		}
	}

	/* add new item to list */
	gl.insert(gl.begin(), g);

	/* unlock list and exit */
	SDL_UnlockMutex(mutex);
}

/***************************************************************************************************
*
* accept + acceptIt
* - mark all objects to be deleted or just some of them
*
***************************************************************************************************/

void RecycleBin::accept()
{
	SDL_LockMutex(mutex);
	for ( list<Garbage*>::iterator it = gl.begin(); it != gl.end(); it++ )
	{
		Garbage *g = *it;
		g->mark();
	}
	SDL_UnlockMutex(mutex);
}

void RecycleBin::acceptIt(Garbage *&old_ptr)
{
	SDL_LockMutex(mutex);
	list<Garbage*>::iterator it;
	it = gl.begin();
	while ( it != gl.end() )
	{
		Garbage *g = *it;
		if ( (void*)old_ptr == (void*)g ) break;
		g->mark();
		it++;
	}
	if ( gl.begin() != gl.end() )
		old_ptr = *(gl.begin());
	else old_ptr = NULL;
	SDL_UnlockMutex(mutex);
}

/***************************************************************************************************
*
* empty()
* - deletes objects that have been marked by all threads
*
***************************************************************************************************/

void RecycleBin::empty()
{
	SDL_LockMutex(mutex);
	list<Garbage*>::iterator it;
	it = gl.begin();
	while ( it != gl.end() )
	{
		Garbage *g = *it;
		if ( g->canDelete(nthreads) )
		{
			it = gl.erase(it);
			delete g;
		} else it++;
	}
	SDL_UnlockMutex(mutex);
}

list<Garbage*> RecycleBin::emptyWithList()
{
	list<Garbage*> el;

	SDL_LockMutex(mutex);
	list<Garbage*>::iterator it;
	it = gl.begin();
	while ( it != gl.end() )
	{
		Garbage *g = *it;
		if ( g->canDelete(nthreads) )
		{
			it = gl.erase(it);
			el.insert(el.end(),g);
			delete g;
		} else it++;
	}
	SDL_UnlockMutex(mutex);

	return el;
}
