
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

#include "SDL_barrier.h"
#include <stdlib.h>

SDL_barrier *SDL_CreateBarrier(int n)
{
	struct _SDL_barrier *b = (struct _SDL_barrier*)
		malloc(sizeof(struct _SDL_barrier));
	b->count = 0;
	b->n = n;
	b->mutex = SDL_CreateMutex();
	b->turnstile = SDL_CreateSemaphore(0);
	b->turnstile2 = SDL_CreateSemaphore(0);
	return b;
}

void SDL_DestroyBarrier(SDL_barrier *b)
{
	if ( b == NULL ) return;
	SDL_DestroyMutex(b->mutex);
	SDL_DestroySemaphore(b->turnstile);
	SDL_DestroySemaphore(b->turnstile2);
	free(b);
}

void SDL_WaitBarrier(SDL_barrier *b)
{
	int i;

    if ( b == NULL ) return;

    SDL_LockMutex(b->mutex);
    b->count ++;
    if ( b->count == b->n )
        for ( i = 0; i < b->n; i++ )
            SDL_SemPost(b->turnstile);
    SDL_UnlockMutex(b->mutex);
    SDL_SemWait(b->turnstile);

    SDL_LockMutex(b->mutex);
    b->count --;
    if ( b->count == 0 )
        for ( i = 0; i < b->n; i++ )
            SDL_SemPost(b->turnstile2);
    SDL_UnlockMutex(b->mutex);
    SDL_SemWait(b->turnstile2);
}
