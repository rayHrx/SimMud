
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

#include "ClientRenderModule.h"
#include "PlayerAI.h"
#include "../utils/IPUtils.h"

#ifdef __USE_3DS_GRAPHICS__
#include "../graphics/3ds/Ogl3dsObject.h"
#else
#include "../graphics/vrml/OglVrmlObject.h"
#endif

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

ClientRenderModule::ClientRenderModule(ClientData *cd)
{
	client_data = cd;
	frame_render_interval = 1;
	if ( client_data->has_GUI )
	{
		md1 = new MovementData[CLIENT_MATRIX_SIZE * CLIENT_MATRIX_SIZE];
		md2 = new MovementData[CLIENT_MATRIX_SIZE * CLIENT_MATRIX_SIZE];
		if ( md1 == NULL || md2 == NULL ) throw "Not enough memory to start the client";
	} else md1 = md2 = NULL;
	nmd = nmd2 = 0;
}

ClientRenderModule::~ClientRenderModule()
{
	if ( md1 == NULL ) delete md1;
	if ( md2 == NULL ) delete md2;
}

/***************************************************************************************************
*
* Graphics init/end
*
***************************************************************************************************/

void ClientRenderModule::initGraphics()
{
	if ( SDL_InitSubSystem(SDL_INIT_VIDEO) )
	{
		printf("[SDL] %s\n", SDL_GetError());
		throw "Cannot initialize SDL graphics";
	}

	/* set icon */
	SDL_Surface *icon = SDL_LoadBMP("data/icon32.bmp");
	if ( icon != NULL )
	{
		SDL_SetColorKey( icon, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB( icon->format, 255,255,255 ) );
		SDL_WM_SetIcon(icon, NULL);
	} else printf("[WARNING] Cannot find icon for this window\n");

	printf("Starting GUI %dx%d %s\n",
		client_data->resx, client_data->resy,
		client_data->full_screen ? "full screen":"");

	/* set up window */
	if ( client_data->full_screen )
		w.createFullScreen( client_data->resx, client_data->resy, client_data->bpp);
	else
		w.create( client_data->resx, client_data->resy, client_data->bpp);
	SDL_WM_SetCaption("Client", NULL);

	/* load models */
	#ifdef __USE_3DS_GRAPHICS__
	obstacle = new Ogl3dsObject("data/wall.3ds");
	resource = new Ogl3dsObject("data/food.3ds");
	wfloor = new Ogl3dsObject("data/floor.3ds");
	player[0] = new Ogl3dsObject("data/player.3ds");
	nplayer_models = 1;
	#else
	obstacle = new OglVrmlObject("data/wall.wrl");
	resource = new OglVrmlObject("data/food.wrl");
	wfloor = new OglVrmlObject("data/floor.wrl");
	player[0] = new OglVrmlObject("data/player.wrl");
	nplayer_models = 1;
	#endif

	/* verify that models were loaded */
	if ( obstacle == NULL || resource == NULL || wfloor == NULL )
		throw "Cannot load 3D models";
	for ( int i = 0; i < nplayer_models; i++ )
		if ( player[i] == NULL ) throw "Cannot load player 3D model";

	/* load font */
	font = new BitmapFont("data/mainfont.bmp");
	if ( font == NULL ) throw "Cannot load font";
}

void ClientRenderModule::endGraphics()
{
	for ( int i = 0; i < nplayer_models; i++ )
		delete player[i];
	delete obstacle;
	delete resource;
	delete wfloor;
	delete font;
	w.destroy();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/***************************************************************************************************
*
* Rendering
*
***************************************************************************************************/

void ClientRenderModule::updatePosition(MovementData *md, int px, int py)
{
	if ( px != md->arrivalx || py != md->arrivaly )
	{
		/* current time and position */
		md->lastx = md->arrivalx;
		md->lasty = md->arrivaly;
		md->last_time = SDL_GetTicks();
		/* new time and position */
		md->arrivalx = px;
		md->arrivaly = py;
		md->arrival_time = md->last_time + MOVEMENT_CONSTANT;
		/* the player should reach the destination in CLIENT_AI_DELAY miliseconds */
	}
	Uint32 current_time = SDL_GetTicks();
	if ( current_time >= md->arrival_time )
	{
		/* if the time interval expired */
		md->pfx = px;
		md->pfy = py;
	} else {
		/* the player is on his way to the arrival coordinates */
		float r = (float)( current_time - md->last_time ) / (float)MOVEMENT_CONSTANT;
		md->pfx = md->lastx + r * ( md->arrivalx - md->lastx );
		md->pfy = md->lasty + r * ( md->arrivaly - md->lasty );
	}
}

void ClientRenderModule::updatePositionForPlayer(IPaddress id, int px, int py, float *pfx, float *pfy)
{

	*pfx = px;
	*pfy = py;

	MovementData *md = NULL;

	/* get movement data */
	for ( int i = 0; i < nmd; i++ )
		if ( equalIP(id, md1[i].id) )
		{
			md = md1 + i;
			break;
		}

	/* add new entry in vector if this player is not present */
	if ( md == NULL )
	{
		md = md2 + nmd2;
		md->id.host = id.host;
		md->id.port = id.port;
		*pfx = md->pfx = md->lastx = md->arrivalx = px;
		*pfy = md->pfy = md->lasty = md->arrivaly = py;
		md->arrival_time = md->last_time = SDL_GetTicks();
		nmd2++;
		return;
	}

	/* store movement data in new vector */
	memcpy(md2 + nmd2, md, sizeof(MovementData));
	md = md2 + nmd2;
	nmd2++;

	/* update movement data */
	updatePosition(md, px,py);
	*pfx = md->pfx;
	*pfy = md->pfy;
}

void ClientRenderModule::updateMovementDataVectors()
{
	/* interchange vectors */
	MovementData *md_aux;
	md_aux = md1;
	md1 = md2;
	md2 = md_aux;

	/* reset size */
	nmd = nmd2;
	nmd2 = 0;
}

/***************************************************************************************************
*
* Rendering
*
***************************************************************************************************/

void ClientRenderModule::renderSingleFrame()
{
	int i,j,ii,jj;
	MapCell *m;

	/* local copy for share data */
	int px,py;		/* player position */
	int dir,life,attr;	/* player attributes */
	int x1,x2,y1,y2;	/* map coordinates */
	float pfx,pfy;		/* interpolated player position (for main player) */
	float pfx2,pfy2;	/* interpolated player position (for another player) */
	MapCell map[CLIENT_MATRIX_SIZE][CLIENT_MATRIX_SIZE];
	OglObject *player_model;/* type of player */

	/* create local copy for shared data */
	SDL_LockMutex(client_data->game_mutex);
	px = client_data->xpos;
	py = client_data->ypos;
	x1 = client_data->x1;
	x2 = client_data->x2;
	y1 = client_data->y1;
	y2 = client_data->y2;
	dir = client_data->dir;
	life = client_data->life;
	attr = client_data->attr;
	for ( i = 0; i < CLIENT_MATRIX_SIZE; i++ )
		for ( j = 0; j < CLIENT_MATRIX_SIZE; j++ )
			memcpy( &map[i][j], &client_data->map[i][j], sizeof(MapCell) );
	SDL_UnlockMutex(client_data->game_mutex);

	/* begin rendering */
	w.beginRender();

	/* get new player position */
	updatePosition(&pmd, px,py);
	pfx = pmd.pfx;
	pfy = pmd.pfy;

	/* set camera and lights */
	gluLookAt(pfx-4.0,6.0,pfy-4.0, 	/* position */
		pfx-0.5,0.0,pfy-0.5,	/* look at */
		0,1,0);		/* up direction */

	/* draw floor */
	for ( i = max( 0, px - MAX_CLIENT_VIEW ); i <= min( client_data->mapx, px + MAX_CLIENT_VIEW ); i++ )
		for ( j = max( 0, py - MAX_CLIENT_VIEW ); j <= min( client_data->mapy, py + MAX_CLIENT_VIEW ); j++ )
	//for ( i = x1; i < x2; i++ )
	//	for ( j = y1; j < y2; j++ )
			wfloor->render(i,j);

	/* draw map */
	for ( i = x1; i < x2; i++ )
		for ( j = y1; j < y2; j++ )
		{
			ii = MAX_CLIENT_VIEW - ( px - i );
			jj = MAX_CLIENT_VIEW - ( py - j );
			m = &map[ii][jj];
			if ( m->terrain != 0 ) obstacle->render( i,j );
			if ( m->type == CELL_OBJECT ) resource->render( i,j, 0.3 + 0.1 * m->quantity );
			if ( m->type == CELL_PLAYER )
			{
				player_model = player[ m->p_attr % nplayer_models ];
				updatePositionForPlayer( m->id, i,j, &pfx2, &pfy2);
				if ( m->dir >= 0 )
					player_model->render( pfx2,pfy2, 0.5 + 0.7 * m->life / 100, m->dir*90 );
				else
					player_model->render( pfx2,pfy2, 0.5 + 0.7 * m->life / 100,
						-m->dir*90, -90);
			}
			if ( m->type == CELL_OBJ_PLAYER )
			{
				resource->render( i,j, 0.3 + 0.1 * m->quantity );
				 
				player_model = player[ m->p_attr % nplayer_models ];
				updatePositionForPlayer( m->id, i,j, &pfx2, &pfy2);
				if ( m->dir >= 0 )
					player_model->render( pfx2,pfy2, 0.5 + 0.7 * m->life / 100, m->dir*90 );
				else
					player_model->render( pfx2,pfy2, 0.5 + 0.7 * m->life / 100,
						-m->dir*90, -90);
			}
		}

	/* draw player */
	player_model = player[ attr % nplayer_models ];
	if ( dir >= 0 )
		player_model->render( pfx,pfy, 0.5 + 0.7 * life / 100 , dir * 90 );
	else
		player_model->render( pfx,pfy, 0.5 + 0.7 * life / 100, -dir * 90, -90);

	/* get coordinates for the status area */
	SDL_Surface *screen = w.getScreen();
	SDL_Rect rect;
	rect.w = client_data->resx;
	rect.h = 40;
	rect.x = 0;
	rect.y = client_data->resy - rect.h;

	/* display status */
	SDL_FillRect( screen, &rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
	SDL_UpdateRect(screen, rect.x,rect.y, rect.w,rect.h);

	char sbuffer[256];
	SDL_WM_SetCaption( client_data->name, NULL);
	if ( client_data->state != PLAYING )
		sprintf(sbuffer, "%s", client_states[client_data->state]);
	else
		sprintf(sbuffer, "playing -> %s ( coord = %d,%d life = %d )",
			AI_state_names[client_data->purpose],
			px,py, life);
	font->render(rect.x + 9, rect.y + 2, sbuffer, screen);
	sprintf(sbuffer, "update interval: average = %.1lfms, last = %.1lfms, fps = %.1f",
		client_data->average_update_interval, client_data->last_update_interval,
		1000.0 / frame_render_interval );
	font->render(rect.x + 9, rect.y + 21, sbuffer, screen);
	SDL_UpdateRect(screen, rect.x,rect.y, rect.w,rect.h);

	/* finish frame rendering */
	updateMovementDataVectors();
	w.endRender();
}

/***************************************************************************************************
*
* Main loop
*
***************************************************************************************************/

void ClientRenderModule::run()
{
	Uint32 start,stop,interval;

	printf("ClientRenderModule started\n");

	if ( client_data->has_GUI ) initGraphics();

	while ( client_data->state != GONE )
	{
		if ( client_data->has_GUI )
		{
			/* render frame */
			start = SDL_GetTicks();
			renderSingleFrame();

			/* handle user actions */
			if ( w.checkMessages() )
			{
				client_data->state = WAITING_LEAVE;
				break;
			}

			/* limit frames per second */
			stop = SDL_GetTicks();
			interval = stop - start;
			if ( interval < RENDER_DELAY ) SDL_Delay( RENDER_DELAY - interval );

			/* compute frames per second average */
			interval = SDL_GetTicks() - start;
			frame_render_interval = interval * ( 1 - client_data->fps_average_param )
				+ frame_render_interval * client_data->fps_average_param;

		} else SDL_Delay(RENDER_DELAY);
	}

	/* wait for client to reach GONE state */
	SDL_LockMutex(client_data->term_mutex);
	while ( client_data->state != GONE )
		SDL_CondWait(client_data->term_cond, client_data->term_mutex);
	SDL_UnlockMutex(client_data->term_mutex);

	if ( client_data->has_GUI ) endGraphics();
}
