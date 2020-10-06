
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

#ifndef __GARBAGE_H
#define __GARBAGE_H

#include <SDL.h>

/***************************************************************************************************
*
* Garbage class
* - this class is used as the base class for all game objects that can be deleted by the server
*   (example: players, food, regions ...)
* - this class contains a counter ("mark_count") that is incremented by all threads; when the
*   counter is equal or greater than the number of threads, the objects can safely be deleted
* - the mutex is used to safely incrmenet the counter
* - because the methods are small and should be fast, they are declared inline
* - [IMPORTANT] each thread must call mark only one time !!!
*
***************************************************************************************************/

class Garbage
{
private:
	int mark_count;
	SDL_mutex *mutex;

public:
	/* Constructor: create mutex and set counter to 0 */
	inline Garbage()
	{
		mutex = SDL_CreateMutex();
		mark_count = 0;
	};

	/* Virtual destructor: destructor from derived class should be called */
	virtual ~Garbage() {};

	/* Mark object as ready to be deleted (each thread increments this counter) */
	inline void mark()
	{
		SDL_LockMutex(mutex);
		mark_count++;
		SDL_UnlockMutex(mutex);
	}

	/* Verify if this object can be deleted */
	inline bool canDelete(int max)
	{
		/* lock mutex */
		SDL_LockMutex(mutex);

		/* if condition is false release mutex and return */
		if ( mark_count < max )
		{
			SDL_UnlockMutex(mutex);
			return false;
		}

		/* if condition is true destroy mutex and return */
		SDL_UnlockMutex(mutex);
		SDL_DestroyMutex(mutex);
		return true;
	}
};

#endif
