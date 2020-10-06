
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

#ifndef __THREAD_SAFE_QUEUE_H
#define __THREAD_SAFE_QUEUE_H

/***************************************************************************************************
*
* This is a class implements a threa-safe queue starting from the STL queue
*
***************************************************************************************************/

#include <SDL.h>
#include <SDL_thread.h>

#include <queue>
using namespace std;

template<class T> class ThreadSafeQueue : public queue<T>
{
private:
	SDL_sem *mutex;
public:
	inline ThreadSafeQueue()
	{
		/* queue<T>() constructor called by default */
		mutex = SDL_CreateSemaphore(1);
	}
	inline ~ThreadSafeQueue()
	{
		/* ~queue<T>() destructor called by default */
		if ( mutex != NULL ) SDL_DestroySemaphore(mutex);
	}

	inline void push ( const T& x )
	{
		SDL_SemWait(mutex);
		queue<T>::push(x);
		SDL_SemPost(mutex);
	}

	inline T pop ( )
	{
		T x;

		SDL_SemWait(mutex);
		x = queue<T>::front();
		queue<T>::pop();
		SDL_SemPost(mutex);

		return x;
	}

	/* works if T is a pointer type */
	inline bool popAndDelete ( )
	{
		T x;

		if ( SDL_SemTryWait(mutex) != 0 ) return false;
		x = queue<T>::front();
		queue<T>::pop();
		if ( x != NULL ) delete x;
		SDL_SemPost(mutex);

		return true;
	}

	inline void lock ( ) { SDL_SemWait(mutex); }
	inline void unlock ( ) { SDL_SemPost(mutex); }
};

#endif
