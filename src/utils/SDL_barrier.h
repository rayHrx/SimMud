
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

#ifndef __BARRIER_H
#define __BARRIER_H

#include <SDL.h>

/* from "The Little Book of Semaphores" - Allen B. Downey */

typedef struct _SDL_barrier
{
	int n,count;
	SDL_mutex *mutex;
	SDL_semaphore *turnstile;
	SDL_semaphore *turnstile2;
};
#define SDL_barrier struct _SDL_barrier

SDL_barrier *SDL_CreateBarrier(int n);
void SDL_DestroyBarrier(SDL_barrier *b);
void SDL_WaitBarrier(SDL_barrier *b);

#endif
