
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

#include "Client.h"
#include "PlayerAI.h"

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

PlayerAI::PlayerAI(ClientData *cd)
{
	this->cd = cd;
	purpose = BASIC;
	if ( cd == NULL ) throw "Cannot initialize AI";
	debug_AI = cd->debug_AI;
}

/***************************************************************************************************
*
* Condition check methods
*
***************************************************************************************************/

bool PlayerAI::questExists()
{
	return cd->quest_active;
}

bool PlayerAI::questIsOutOfReach()
{
	return ( abs(cd->xpos - cd->questx) > MAX_CLIENT_VIEW )
		||
		( abs(cd->ypos - cd->questy) > MAX_CLIENT_VIEW );
}

bool PlayerAI::foodIsVisible(int *x, int *y)
{
	return findNearObject(x,y, MAX_CLIENT_VIEW, CELL_OBJECT, -1, 0x7FFFFFFF);
}

bool PlayerAI::lifeBelow(int x)
{
	return cd->life < x;
}

bool PlayerAI::weakPlayerNear(int *x, int *y)
{
	return findNearObject(x,y, MAX_CLIENT_VIEW, CELL_PLAYER, 10,cd->life-1);
}

bool PlayerAI::strongPlayerNear(int *x, int *y)
{
	return findNearObject(x,y, MAX_CLIENT_VIEW, CELL_PLAYER, cd->life+1,0xFFFF);
}

bool PlayerAI::findNearObject(int *rx, int *ry, int distance, int type, int min_value, int max_value)
{
	int i,j,d;
	int px,py,x,y;

	px = cd->xpos;
	py = cd->ypos;

	for ( d = 1; d < distance; d++ )
	{
		for ( x = px - d; x <= px + d; x++ )
		{
			y = py - d;
			i = x - px + distance;
			j = y - py + distance;
			if ( cd->map[i][j].type == type
				&& cd->map[i][j].life >= min_value && cd->map[i][j].life <= max_value )
			{
				*rx = x, *ry = y;
				return true;
			}

			y = py + d;
			i = x - px + distance;
			j = y - py + distance;
			if ( cd->map[i][j].type == type
				&& cd->map[i][j].life >= min_value && cd->map[i][j].life <= max_value )
			{
				*rx = x, *ry = y;
				return true;
			}
		}

		for ( y = py - d + 1; y <= py + d - 1; y++ )
		{
			x = px - d;
			i = x - px + distance;
			j = y - py + distance;
			if ( cd->map[i][j].type == type
				&& cd->map[i][j].life >= min_value && cd->map[i][j].life <= max_value )
			{
				*rx = x, *ry = y;
				return true;
			}

			x = px + d;
			i = x - px + distance;
			j = y - py + distance;
			if ( cd->map[i][j].type == type
				&& cd->map[i][j].life >= min_value && cd->map[i][j].life <= max_value )
			{
				*rx = x, *ry = y;
				return true;
			}
		}
	}
	return false;
}

/***************************************************************************************************
*
* Movement methods
*
***************************************************************************************************/

void PlayerAI::randomCoordinates(int *x, int *y)
{
	do
	{
		*x = rand() % cd->mapx;
		*y = rand() % cd->mapy;
	} while ( cd->terrain[*x][*y] != 0 );
}

PlayerAI_Actions PlayerAI::moveAlongThePath()
{
	/* get next coordinates */
	if ( path->empty() ) return NO_ACTION;
	int x = path->getX();
	int y = path->getY();
	if ( debug_AI ) printf("Player wants at %d,%d\n", x,y);
	if ( x < 0 || x >= cd->mapx
		|| y < 0 || y >= cd->mapy
		|| cd->terrain[x][y] != 0 )
	{
		purpose = BASIC;
		return NO_ACTION;	/* rethink this action */
	}
	if ( debug_AI ) printf("Move to %d,%d (%d)\n", x,y, path->size());

#define empty_cell(x,y) ( cd->map[MAX_CLIENT_VIEW + x][MAX_CLIENT_VIEW + y].type != CELL_PLAYER )

	/* select move action */
	PlayerAI_Actions action = NO_ACTION;
	if ( x < cd->xpos && empty_cell(-1,0) ) action = MOVE_LEFT;
	if ( y < cd->ypos && empty_cell(0,-1) ) action = MOVE_UP;
	if ( x > cd->xpos && empty_cell(1,0) ) action = MOVE_RIGHT;
	if ( y > cd->ypos && empty_cell(0,1) ) action = MOVE_DOWN;
	path->pop();

	/* update 'should be' position */
	sb_wait_count = 0;
	sbx = x;
	sby = y;

	return action;
}

/***************************************************************************************************
*
* Action methods
*
***************************************************************************************************/

bool PlayerAI::checkFood(int amount)
{
	int x,y;

	if ( lifeBelow(amount)
		&& cd->map[MAX_CLIENT_VIEW][MAX_CLIENT_VIEW].type == CELL_OBJECT
		&& cd->map[MAX_CLIENT_VIEW][MAX_CLIENT_VIEW].quantity > 0 )
	{
		path->clear();
		purpose = SEEKING_FOOD;
		if ( debug_AI ) printf("[AI]Want food at %d,%d (same position as player)\n",
			cd->xpos,cd->ypos);
		return true;
	}
	if ( lifeBelow(amount) && foodIsVisible(&x,&y) )
	{
		path = pathfind_alg.findPath(cd->xpos,cd->ypos, x,y);
		if ( path->empty() ) return false;
		purpose = SEEKING_FOOD;
		if ( debug_AI ) printf("[AI]Want food at %d,%d\n", x,y);
		return true;
	}

	return false;
}

bool PlayerAI::checkStrongPlayer()
{
	int x,y;
	if ( !strongPlayerNear(&x,&y) ) return false;

	/* run in the oposite direction */
	x = cd->xpos - ( x - cd->xpos );
	x = ( x >= 0 ) ? ( ( x < cd->mapx ) ? x : ( cd->mapx - 1 ) ) : 0;
	y = cd->ypos - ( y - cd->ypos );
	y = ( y >= 0 ) ? ( ( y < cd->mapy ) ? y : ( cd->mapy - 1 ) ) : 0;

	/* get random coodinates is the previos coodinates were not valid */
	if ( cd->terrain[x][y] != 0 ) randomCoordinates(&x,&y);

	/* set path */
	path = pathfind_alg.findPathNear( cd->xpos,cd->ypos, x,y );
	if ( path->empty() ) return false;
	purpose = RUNNING_AWAY;

	return true;
}

bool PlayerAI::checkQuest()
{
	if ( questExists() && questIsOutOfReach() )
	{
		path = pathfind_alg.findPathNear( cd->xpos,cd->ypos, cd->questx,cd->questy );
		if ( path->empty() )
		{
			if ( debug_AI )
				printf("[AI]Cannot find path to quest (%d,%d -> %d,%d)\n",
					cd->xpos,cd->ypos, cd->questx,cd->questy);
			return false;
		}
		purpose = SEEKING_QUEST;
		if ( debug_AI )
			printf("[AI]Seeking quest at %d,%d\n", cd->questx,cd->questy);
		return true;
	}
	return false;
}

bool PlayerAI::checkWeakPlayer()
{
	int x,y;
	if ( weakPlayerNear(&x,&y) )
	{
		path = pathfind_alg.findPathNear( cd->xpos,cd->ypos, x,y );
		if ( path->empty() ) return false;
		purpose = CHASING_PLAYER;
		if ( debug_AI ) printf("[AI]Chasing player at %d,%d (%d moves)\n",
			x,y, path->size());
		return true;
	}
	return false;
}

bool PlayerAI::tryToExplore()
{
	int x,y;
	randomCoordinates(&x,&y);
	path = pathfind_alg.findPathNear(cd->xpos,cd->ypos, x,y);
	if ( path->empty() ) return false;
	if ( debug_AI ) printf("[AI]Exploring at %d,%d\n", x,y);
	purpose = EXPLORING;
	return true;
}

/***************************************************************************************************
*
* Main action taking method
*
***************************************************************************************************/

PlayerAI_Actions PlayerAI::takeAction()
{
	int px,py;
	PlayerAI_Actions action;

	/* player coodinates */
	px = cd->xpos;
	py = cd->ypos;

	/* set map for pathfinding algorithm */
	pathfind_alg.setMatrix(cd->mapx,cd->mapy, cd->terrain);
	pathfind_alg.setSecondMatrix(CLIENT_MATRIX_SIZE, CLIENT_MATRIX_SIZE, cd->dynamic_objects,
		cd->xpos - MAX_CLIENT_VIEW, cd->ypos - MAX_CLIENT_VIEW);

	/* if node update was received wait some more */
	/* if no update is received for a long time take another action */
	if ( purpose != BASIC )
		if ( sbx != px || sby != py )
		{
			sb_wait_count++;
			if ( sb_wait_count == AI_RETRY_COUNT )
			{
				if ( debug_AI ) printf("[AI]Wait limit reached "
					"(now:%d,%d -> sb: %d,%d) \n", px,py, sbx,sby);
				purpose = BASIC;
				path->clear();
			} else return NO_ACTION;
		}
	if ( debug_AI ) printf("[AI][%d,%d]Purpose: %s\n", px,py,AI_state_names[purpose]);
	cd->purpose = purpose;

	/* select a new action */
	switch ( purpose )
	{
		case BASIC:
			checkFood(100)
				|| checkQuest() || checkStrongPlayer()
				|| checkWeakPlayer() || tryToExplore();
			break;
		case SEEKING_QUEST:
			if ( !questExists() ) purpose = BASIC;
			checkFood(20);
			break;
		case CHASING_PLAYER:
			checkFood(40) || checkQuest() || checkWeakPlayer();
			break;
		case EXPLORING:
			checkFood(90) || checkQuest()
				|| checkStrongPlayer() || checkWeakPlayer();
			break;
		default: break;
	}

	/* move player */
	action = moveAlongThePath();
	if ( action != NO_ACTION ) return action;

	if ( path->size() <= 1 && purpose == CHASING_PLAYER )
	{
		if ( debug_AI ) printf("[AI]Attack\n");
		purpose = BASIC;
		
		if( path->size() == 1 )
		{
			int opponent_x = path->getX();
			int opponent_y = path->getY();
		
			if( opponent_x == px + 1 )		return ATTACK_RIGHT;
			if( opponent_x == px - 1 )		return ATTACK_LEFT;
			if( opponent_y == py + 1 )		return ATTACK_DOWN;
			if( opponent_y == py - 1 )		return ATTACK_UP;
		}
		
		return ATTACK_UP;
	}

	/* check if last purpose is over */
	if ( path->empty() && purpose != BASIC )
	{
		if ( purpose == SEEKING_FOOD
			&& cd->map[MAX_CLIENT_VIEW][MAX_CLIENT_VIEW].type == CELL_OBJECT
			&& cd->map[MAX_CLIENT_VIEW][MAX_CLIENT_VIEW].quantity > 0 )
		{
			if ( debug_AI ) printf("[AI]Eat\n");
			purpose = BASIC;
			return USE;
		}
		purpose = BASIC;
	}

	return NO_ACTION;
}
