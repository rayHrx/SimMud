
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
#include "Player.h"
#include <math.h>

Player::Player(IPaddress adr)
{
	int i;
	memset(name, 0, MAX_PLAYER_NAME);
	for ( i = 0; i < 8; i++ ) name[i] = 'a' + rand() % ('z'-'a');
	sprintf( name + i, "_%X_%d", adr.host, adr.port);
	memset(&address, 0, sizeof(address));
	address.host = adr.host;
	address.port = adr.port;

	pos.x = pos.y = 0;
	life = 0;
	attr = 0;
	dir = 0;
	count = 0;
	mutex = SDL_CreateMutex();
}

Player::Player(char *data)
{
	char *ptr;
	mutex = SDL_CreateMutex();
	count = 0;

    printf("create player struct from data\n");

	/* copy player name */
	memset(name, 0, MAX_PLAYER_NAME);
	memcpy(name, data, MAX_PLAYER_NAME);

	/* copy player address */
	ptr = data + MAX_PLAYER_NAME;
	memset(&address, 0, sizeof(address));
	address.host = ((IPaddress*)ptr)->host;
	address.port = ((IPaddress*)ptr)->port;

	/* copy attributes */
	ptr += sizeof(IPaddress);
	pos.x    = *( (int*)ptr + 0 );
	pos.y    = *( (int*)ptr + 1 );
	life	 = *( (int*)ptr + 2 );
	attr	 = *( (int*)ptr + 3 );
	dir	     = *( (int*)ptr + 4 );
}

Player::~Player()
{
	if ( mutex != NULL ) SDL_DestroyMutex(mutex);
}

int Player::playerDataSize()
{
	return MAX_PLAYER_NAME + sizeof(IPaddress) + sizeof(int)*5;
}

char *Player::playerData()
{
	char *buffer,*ptr;

    printf("create data from player struct\n");

	/* alocate buffer */
	buffer = new char[playerDataSize()];
	if ( buffer == NULL ) return NULL;

	/* copy data into the buffer */
	memcpy(buffer, name, MAX_PLAYER_NAME);
	ptr = buffer + MAX_PLAYER_NAME;
	memcpy( (IPaddress*)ptr, &address, sizeof(IPaddress) );
	ptr += sizeof(IPaddress);
	*( (int*)ptr + 0 ) = pos.x;
	*( (int*)ptr + 1 ) = pos.y;
	*( (int*)ptr + 2 ) = life;
	*( (int*)ptr + 3 ) = attr;
	*( (int*)ptr + 4 ) = dir;

	return buffer;
}

void Player::setName(char *name)
{
	strncpy(this->name, name, MAX_PLAYER_NAME);
}

double Player::getDistance(Player* pl)
{
	return sqrt((pos.x - pl->pos.x)*(pos.x - pl->pos.x) + (pos.y - pl->pos.y) * (pos.y - pl->pos.y));	
}

void Player::useObject(GameObject* o)
{
	SDL_LockMutex(mutex);
	if( life < 100 )		o->quantity --, life++;
	SDL_UnlockMutex(mutex);
}

void Player::attackPlayer(Player* p2)
{
	if( *(int*)this < *(int*)p2 ){		SDL_LockMutex(this->mutex);	SDL_LockMutex(p2->mutex);	}
	else						 {		SDL_LockMutex(p2->mutex);SDL_LockMutex(this->mutex);	}
	
	/* update players */
	if ( p2->life > 1 )		p2->life--, p2->dir = - dir;
	if ( life  < 100 )		life++;
	
	if( *(int*)this < *(int*)p2 ){		SDL_UnlockMutex(this->mutex);SDL_UnlockMutex(p2->mutex);	}
	else						 {		SDL_UnlockMutex(p2->mutex);SDL_UnlockMutex(this->mutex);	}
	
}

