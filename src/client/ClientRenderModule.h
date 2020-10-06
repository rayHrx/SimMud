
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

#ifndef __CLIENT_RENDER_MODULE
#define __CLIENT_RENDER_MODULE

#include "Client.h"

#define MAX_PLAYER_MODELS 16

struct MovementData
{
	IPaddress id;			/* IP address of the current player (used for id) */
	float lastx,lasty;		/* last position */
	float pfx,pfy;			/* current position (interpolated) */
	float arrivalx,arrivaly;	/* next position */
	Uint32 arrival_time,last_time;	/* time for last and next position */
};

class ClientRenderModule : public Module
{
protected:
	ClientData *client_data;
	OglWorld w;
	OglObject *player[MAX_PLAYER_MODELS];
	int nplayer_models;
	OglObject *obstacle,*resource,*wfloor;
	BitmapFont *font;

	MovementData pmd;	/* movement data for main player */
	MovementData *md1,*md2;	/* movement data for other players (two vectors) */
	int nmd,nmd2;		/* number of players with movement data (+ for second vector) */

	float frame_render_interval;

public:
	/* constructor / destructor */
	ClientRenderModule(ClientData *cd);
	~ClientRenderModule();

	/* graphics setup */
	void initGraphics();
	void endGraphics();

protected:
	/* player movement */
	void updatePosition(MovementData *md, int px, int py);
	void updatePositionForPlayer(IPaddress id, int px, int py, float *pfx, float *pfy);
	void updateMovementDataVectors();

public:
	/* rendering */
	void renderSingleFrame();

	/* main loop */
	void run();
};

#endif

