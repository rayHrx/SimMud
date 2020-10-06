
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

#ifndef __CLIENT_H
#define __CLIENT_H

#include "General.h"
#include "../game/Region.h"
#include "../game/Quest.h"

#include "../graphics/Graphics.h"
#include "../graphics/OglObject.h"
#include "../graphics/OglWorld.h"
#include "../graphics/font/BitmapFont.h"

/***************************************************************************************************
*
* A map cell seen by the client
*
***************************************************************************************************/

struct MapCell
{
	char terrain;		/* terrain type (0/1 free/blocked) */
	char type;		/* type: NONE,EMPTY,OBJECT,PLAYER (NONE means not known) */
	int life,p_attr,dir;			/* player/object attributes */
	int quantity, o_attr;		/* object attributes */
	IPaddress id;		/* players are identified by their IPaddress */
};

/***************************************************************************************************
*
* Data shared by all client threads
*
***************************************************************************************************/

struct ClientData
{
	/* server */
	char *server_name;	/* game server coordinates */
	int server_port;
	IPaddress server_address;
	int server_target;
    Uint32 disconnect_timeout;	/* time to wait for a server update (in ms) */
	Uint32 migration_timeout;	/* time to wait for a player migration (in ms) */
	double last_update_interval;	/* update interval */
	double average_update_interval;	/* 95% previos average, 5% last update */

	/* player state */
	char name[MAX_PLAYER_NAME];	/* player name */
	int state;			/* the state of the client (finite state machine) */
	int xpos,ypos;		/* players position */
	int life,attr,dir;		/* attributes */

	/* all map (client remembers the terrain in the explored region) - used for pathfinding */
	int mapx,mapy;
	char **terrain;

	/* game state */
	int x1,x2,y1,y2;	/* location of the region seen by the player */
	/* player and object information */
	MapCell map[CLIENT_MATRIX_SIZE][CLIENT_MATRIX_SIZE];
	char **dynamic_objects;
	bool quest_active;		/* indicates the presence of a quest */
	int questx,questy;		/* location of the current quest */
	SDL_mutex *game_mutex;

	/* graphical interface */
	bool has_GUI;
	int resx,resy,bpp;	/* screen size and bits per pixel */
	bool full_screen;

	/* terminate condition */
	SDL_mutex *term_mutex;
	SDL_cond *term_cond;

	/* debug */
	int purpose;
	bool debug_AI;
	float fps_average_param;	/* used for weighted average when computing frames per second */
};

/***************************************************************************************************
*
* Client states (client is a finite state machine)
*
***************************************************************************************************/

enum ClientStates
{
	INITIAL = 0,
	WAITING_JOIN,
	PLAYING,
	WAITING_LEAVE,
	MIGRATING,
	GONE
};

const char client_states[6][24] = {"Initial","Waiting join ...","Playing",
	"Waiting leave ...","Migrating ...","Gone"};

#endif
