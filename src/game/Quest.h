
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

#ifndef __QUEST_H
#define __QUEST_H

#include "../utils/Garbage.h"

class Quest
{
private:
	int x,y;			/* quest position */
	bool active, active_timer;	/* active flag */
	SDL_mutex *mutex;		/* protect member variables */
	Uint32 end_time;		/* when in active_timer mode */
	bool start_flag, stop_flag;

public:
	/* Constructor/destructor */
	Quest();
	~Quest();

	/* position methods */
	int getX();		/* quest coordinates */
	int getY();
	void setPosition(int x, int y);

	/* activation methods */
	bool isActive();
	void start(Uint32 timer);
	void start();
	void stop();

	/* client related methods */
	bool mustSendStart();
	bool mustSendStop();

	bool checkTimer();
};

#endif
