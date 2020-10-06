
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

#include "General.h"
#include "Quest.h"

/***************************************************************************************************
*
* Constructor/destructor
*
***************************************************************************************************/

Quest::Quest()
{
	mutex = SDL_CreateMutex();
	if ( mutex == NULL ) throw "Cannot create mutex for Quest object";

	active = false;
	active_timer = false;
	start_flag = false;
	stop_flag = false;
	x = y = 0;
}

Quest::~Quest()
{
	SDL_DestroyMutex(mutex);
}

/***************************************************************************************************
*
* Position related methods
*
***************************************************************************************************/

int Quest::getX()
{
	return x;
}

int Quest::getY()
{
	return y;
}

void Quest::setPosition(int x, int y)
{
	SDL_LockMutex(mutex);
	this->x = x;
	this->y = y;
	SDL_UnlockMutex(mutex);
}

/***************************************************************************************************
*
* Activation related methods
*
***************************************************************************************************/

bool Quest::isActive()
{
	return active || active_timer;
}

void Quest::start(Uint32 timer)
{
	SDL_LockMutex(mutex);
	end_time = SDL_GetTicks() + timer * 1000;
	active_timer = true;
	start_flag = true;
	SDL_UnlockMutex(mutex);
}

void Quest::start()
{
	SDL_LockMutex(mutex);
	active = true;
	start_flag = true;
	SDL_UnlockMutex(mutex);
}

void Quest::stop()
{
	SDL_LockMutex(mutex);
	active = false;
	active_timer = false;
	stop_flag = true;
	SDL_UnlockMutex(mutex);
}

/***************************************************************************************************
*
* Client related method
*
***************************************************************************************************/

bool Quest::mustSendStart()
{
	bool result;

	SDL_LockMutex(mutex);
	checkTimer();
	result = start_flag & (active || active_timer);
	start_flag = false;
	SDL_UnlockMutex(mutex);

	return result;
}

bool Quest::mustSendStop()
{
	bool result;

	SDL_LockMutex(mutex);
	checkTimer();
	result = stop_flag && !active && !active_timer;
	stop_flag = false;
	SDL_UnlockMutex(mutex);

	return result;
}

/***************************************************************************************************
*
* Timer method
*
***************************************************************************************************/

bool Quest::checkTimer()
{
	if ( !active_timer ) return false;
	if ( SDL_GetTicks() > end_time )
	{
		active_timer = false;
		stop_flag = true;
		return true;
	}
	return false;
}
