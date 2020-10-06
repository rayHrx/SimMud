
/***************************************************************************************************
*
* SUBJECT:
*    A Benckmark for Massive Multiplayer Online Games
*    Game Server and Client
*
* AUTHOR:
*    Mihai Paslariu
*    Politehnica University of Bucharest, Romania
*    mihplaesu@yahoo.com
*
* TIME AND PLACE:
*    University of Toronto, Toronto, Canada
*    March - August 2007
*
***************************************************************************************************/

#ifndef __CONSTANTS_H
#define __CONSTANTS_H

/* UDP / TCP communication parameters */
/***********************************************************************/

/* size for udp packets (default 16k) */
#define MAX_UDP_PACKET_SIZE (16 * 1024)

/* timeout in miliseconds ( -1 means forever )*/
#define UDP_TIMEOUT -1

/* buffer size for receiving TCP messages */
#define TCP_BUFFER_SIZE 	4096

/* Server constants */
/***********************************************************************/

/* the maximum number of servers */
#define MAX_THREADS		128
#define GENERAL_MSG_ID  0

/* The max number of players to a thread */
#define MAX_PLAYERS_PER_THREAD 20

/* the maximum number of players in a region */
#define MAX_PLAYERS_PER_REGION (MAX_PLAYERS_PER_THREAD/2)

/* the maximum tree depth - avoid tree degeneration */
#define MAX_TREE_DEPTH 10 

/* the max number of players to merge regions*/
#define MAX_PLAYERS_TO_MERGE (MAX_PLAYERS_PER_THREAD/5)

/* number of thread is server */
#define MAX_REGULAR_UPDATE_THREADS	64
#define MAX_WORLD_UPDATE_THREADS	32

/* Other constants */
/***********************************************************************/

/* size of the buffer for reading from a file */
#define MAX_FILE_READ_BUFFER	1024

/* client name */
#define MAX_PLAYER_NAME		32

/* the distance at which the client views (in meters/cells) */
/* (the client is in the middle of a square of size 2*MAX_CLIENT_VIEW+1) */
#define MAX_CLIENT_VIEW		8
#define CLIENT_MATRIX_SIZE	(2 * MAX_CLIENT_VIEW + 1)

/* minimum time between rendering two frames (in miliseconds) */
#define RENDER_DELAY		25

/* client delay in miliseconds (interval between client actions) */
/* (how fast can a human player take an action) */
#define CLIENT_AI_DELAY		200

/* how much should a client wait for its action to take effect */
#define AI_RETRY_COUNT		3

/* the time the player needs to move to the next cell - move 1 meter */
/* (used for rendering) */
#define MOVEMENT_CONSTANT 	( CLIENT_AI_DELAY )

/* the number to retry some operations */
#define RETRY_COUNT		100

/* the interval in miliseconds the master checks if is hast to modify the
layout because overloading (in miliseconds) */
#define MASTER_CHECK_INTERVAL	1000

/* timeout if server is not responding (in miliseconds) */
#define DEFAULT_SERVER_TIMEOUT	10000

/* timeout if migration is taking too long (in miliseconds) */
#define DEFAULT_MIGRATION_TIMEOUT	10000

/* Map constants */
/***********************************************************************/

#define CELL_NONE		((char)0)
#define CELL_EMPTY		((char)1)
#define CELL_OBJECT		((char)2)
#define CELL_PLAYER		((char)3)
#define CELL_OBJ_PLAYER	((char)4)

/* Other constants */
/***********************************************************************/

/* Usualy defined somewhere else, but just in case */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#endif
