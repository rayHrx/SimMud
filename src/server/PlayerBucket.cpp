
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

#include "PlayerBucket.h"

/***************************************************************************************************
*
* Constructor and destructor
*
***************************************************************************************************/

PlayerBucket::PlayerBucket()
{
	assert( lock = SDL_CreateMutex() );
	first_pos = true;
	last_pos = false;
}

PlayerBucket::~PlayerBucket()
{
	SDL_DestroyMutex(lock);
}

bool PlayerBucket::insert(Player *p)
{
	assert(p);

	SDL_LockMutex(lock);
	pair<PlayerDicIterator,bool> res = dic.insert( make_pair(p->address,p) );
	SDL_UnlockMutex(lock);

	return res.second;
}

Player* PlayerBucket::find(IPaddress addr)
{
	Player* p = NULL;

	SDL_LockMutex(lock);

	PlayerDicIterator result = dic.find(addr);
	if ( result != dic.end() ) p = result->second;

	SDL_UnlockMutex(lock);

	return p;
}

bool PlayerBucket::erase(IPaddress addr)
{
	int res = false;
	SDL_LockMutex(lock);
	PlayerDicIterator result = dic.find(addr);
	if ( result != dic.end() )
	{
		if ( it == result ) it++;
        dic.erase(result);
        res = true;
	}
	SDL_UnlockMutex(lock);

	return res;
}

/***************************************************************************************************
*
* Iteration
*
***************************************************************************************************/

void PlayerBucket::start()
{
	first_pos = true;
	last_pos = false;
}

Player* PlayerBucket::next()
{
	Player *p = NULL;

	if ( last_pos ) return NULL;
	SDL_LockMutex(lock);

	/* move iterator */
	if ( first_pos )
	{
		it = dic.begin();
		first_pos = false;
	}

	if ( it != dic.end() )
	{
		p = it->second;
		it++;
	} else { last_pos = true; }

	SDL_UnlockMutex(lock);

	return p;
}

/***************************************************************************************************
*
* Copy
*
***************************************************************************************************/

/*
* void copyPlayers(list<Player*> *copy_list)
* - copy all the pointers fron the bucket in the given list
*/

void PlayerBucket::copyPlayers(list<Player*> *copy_list)
{
	/* empty the list */
	if ( copy_list == NULL ) return;
	copy_list->clear();

	/* lock bucket */
	SDL_LockMutex(lock);

	/* copy all elements from this bucket to the list */
	map<IPaddress,Player*,IpComparator>::iterator itb;
	for ( itb = dic.begin(); itb != dic.end(); itb++ )
		copy_list->insert(copy_list->end(), itb->second);

	/* unlock bucket */
	SDL_UnlockMutex(lock);
}
