
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

#ifndef __PLAYER_BUCKET_H
#define __PLAYER_BUCKET_H

#include <map>
#include <list>
using namespace std;

#include "../utils/IPUtils.h"
#include "../game/Player.h"


/***************************************************************************************************
*
* Data types
*
***************************************************************************************************/

#define PlayerDicType		map<IPaddress,Player*,IpComparator>
#define PlayerDicIterator	map<IPaddress,Player*,IpComparator>::iterator

/***************************************************************************************************
*
* PlayerBucket
*
***************************************************************************************************/

class PlayerBucket
{
protected:
	SDL_mutex *lock;	/* synchronize access to this collection */
	PlayerDicType dic;	/* map with IP as key and pointer to player as value */

	/* members for iterating */
	PlayerDicIterator it;
	bool first_pos,last_pos;

public:
	/* Constructor / Destructor */
	PlayerBucket();
	~PlayerBucket();

	/* Operations */
	bool insert(Player *p);
	
	Player*	find(IPaddress addr);
	bool	erase(IPaddress addr);
	
	bool	erase(Player *p){		return erase( p->address );		}	
	int		size(){					return dic.size();				}
		
	void start();
	Player* next();

	/* Copy */
	void copyPlayers(list<Player*> *copy_list);
};

#endif
