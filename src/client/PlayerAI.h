
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

#ifndef __PLAYER_AI_H
#define __PLAYER_AI_H

#include "AStar/AStar.h"

/***************************************************************************************************
*
* Constants
*
***************************************************************************************************/

enum PlayerAI_Purpose
{
	BASIC = 0,
	EXPLORING,
	SEEKING_QUEST,
	SEEKING_FOOD,
	CHASING_PLAYER,
	RUNNING_AWAY
};

const char AI_state_names[6][20] = { "basic","exploring","quest","food","chasing","running away" };

enum PlayerAI_Actions
{
	NO_ACTION = 0,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	USE,
	ATTACK_RIGHT,
	ATTACK_LEFT,
	ATTACK_UP,
	ATTACK_DOWN
};

const char AI_actions[7][10] = { "none","up","down","left","right","eat","attack" };

/***************************************************************************************************
*
* AI Classs
*
***************************************************************************************************/

class PlayerAI
{
private:
	/* global data */
	ClientData *cd;

	/* AI specific */
	PlayerAI_Purpose purpose;	/* current client state */
	PointStack *path;		/* the current path the client is moving */
	AStar pathfind_alg;		/* class for the pathfindg algorithm */
	int debug_AI;			/* debugging flag */

	/* "should be" coordinates => where the player should be */
	int sbx,sby;
	int sb_wait_count;	/* number of times to wait until the server
				confirmes our "should be" position */

public:
	/* constructor */
	PlayerAI(ClientData *cd);

	/* condition check methods */
	bool questExists();
	bool questIsOutOfReach();
	bool foodIsVisible(int *x, int *y);
	bool lifeBelow(int x);
	bool weakPlayerNear(int *x, int *y);
	bool strongPlayerNear(int *x, int *y);
	bool findNearObject(int *x, int *y, int distance, int type, int min_value, int max_value);

	/* moving methods */
	void randomCoordinates(int *x, int *y);
	PlayerAI_Actions moveAlongThePath();

	/* action methods */
	bool checkFood(int amount);
	bool checkStrongPlayer();
	bool checkQuest();
	bool checkWeakPlayer();
	bool tryToExplore();

	/* main action taking method */
	PlayerAI_Actions takeAction();
};

#endif
