
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

#ifndef __PLAYER_H
#define __PLAYER_H


#include "../General.h"
#include "GameObject.h"


class Player
{
public:
	/* player identification */
	char name[MAX_PLAYER_NAME];
	IPaddress address;
    
	/* player game state */
	Vector2D pos;	/* position */
	int life,attr;	/* attributes */
	int dir;	/* direction (0,1,2,3) = (up,right,down,left) */

	/* synchronization */
	SDL_mutex *mutex;

	/* parameter used in operations requiring retries */
	int count;
	Uint32 time_of_last_message;

public: /* constructors / destructor */
	Player(IPaddress adr);
	Player(char *data);
	~Player();

public: /* public methods */
	char *playerData();	/* serialize player data */
	int playerDataSize();
	void setName(char *name);
	double getDistance(Player* pl);
	
	void useObject(GameObject* o);
	void attackPlayer(Player* p2);
};

#endif
