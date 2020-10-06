
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
#include "../comm/MessageModule.h"
#include "PlayerAI.h"
#include "ClientActionModule.h"
#include "ClientRenderModule.h"

/***************************************************************************************************
*
* Client Threads
*
***************************************************************************************************/

int module_thread(void *data)
{
	Module *module = (Module*)data;

	try
	{
		module->run();
	} catch ( const char *err ) {
		printf("[ERROR]%s\n", err);
		exit(-1);
	}
	return 0;
}

/***************************************************************************************************
*
* Client inialization
*
***************************************************************************************************/

void init(int argc, char *argv[], ClientData &client_data)
{
	char *port_ptr;
	int i,j;

	/* initialize random number generator */
	srand((unsigned int)(time(NULL) + SDL_GetTicks() + SDL_ThreadID()));

	/* initial values for client parameters */
	/* (see description in client.h) */
	client_data.state = INITIAL;
	client_data.xpos = client_data.ypos = 0;
	client_data.life = client_data.attr = client_data.dir = 0;
	client_data.x1 = client_data.x2 = 0;
	client_data.y1 = client_data.y2 = 0;
	for ( i = 0; i < CLIENT_MATRIX_SIZE; i++ )
		for ( j = 0; j < CLIENT_MATRIX_SIZE; j++ )
			memset( &client_data.map[i][j], 0, sizeof(MapCell) );
	client_data.quest_active = false;
	client_data.term_cond = SDL_CreateCond();
	client_data.term_mutex = SDL_CreateMutex();
	if ( client_data.term_cond == NULL || client_data.term_mutex == NULL )
		throw "Cannot create mutex and condition variables";
	client_data.mapx = client_data.mapy = 0;
	client_data.terrain = NULL;
	client_data.has_GUI = false;
	client_data.resx = 512;
	client_data.resy = 384;
	client_data.bpp = 32;
	client_data.fps_average_param = 0.9;
	client_data.full_screen = false;
	client_data.debug_AI = false;
	client_data.purpose = 0;
	client_data.disconnect_timeout = DEFAULT_SERVER_TIMEOUT;
	client_data.migration_timeout = DEFAULT_MIGRATION_TIMEOUT;
	client_data.game_mutex = SDL_CreateMutex();
	if ( client_data.game_mutex == NULL )
		throw "Cannot create mutex for shared client data";
	client_data.average_update_interval = -1;
	client_data.dynamic_objects = new char *[CLIENT_MATRIX_SIZE];
	if ( client_data.dynamic_objects == NULL ) throw "Not enough memory";
	for ( i = 0; i < CLIENT_MATRIX_SIZE; i++ )
	{
		client_data.dynamic_objects[i] = new char[CLIENT_MATRIX_SIZE];
		if ( client_data.dynamic_objects[i] == NULL ) throw "Not enough memory";
		memset(client_data.dynamic_objects[i], 0, CLIENT_MATRIX_SIZE);
	}

	/* interpret command line arguments */
	if ( argc < 2 ) throw "Arguments are [options] <server_name:server_port>";

	/* get other parameters */
	for ( i = 1; i < argc-1; i++ )
	{
		if ( !strcmp(argv[i], "--gui") )
		{
			client_data.has_GUI = true;
			continue;
		}

		if ( !strcmp(argv[i], "--nogui") )
		{
			client_data.has_GUI = false;
			continue;
		}

		if ( !strncmp(argv[i], "--size=", 7) )
		{
			sscanf(argv[i]+7, "%dx%d", &client_data.resx, &client_data.resy);
			if ( client_data.resx < 120 || client_data.resy < 90  )
				throw "Invalid screen size";
			continue;
		}

		if ( !strcmp(argv[i], "--fullscreen") )
		{
			client_data.full_screen = true;
			continue;
		}

		if ( !strncmp(argv[i], "--bpp=", 6 ) )
		{
			sscanf(argv[i]+6, "%d", &client_data.bpp);
			if ( client_data.bpp != 1 &&   client_data.bpp != 8
				&&  client_data.bpp != 16 &&  client_data.bpp != 32 )
				throw "Invalid screen depth";
			continue;
		}

		if ( !strncmp(argv[i], "--disconnect_timeout=", 21 ) )
		{
			sscanf(argv[i]+21, "%d", &client_data.disconnect_timeout);
			if ( client_data.disconnect_timeout == 0 )
				throw "Invalid disconnect timeout (must be non-zero)";
			/* client imputs in seconds but variable holds miliseconds */
			client_data.disconnect_timeout *= 1000;
			continue;
		}

		if ( !strncmp(argv[i], "--migration_timeout=", 20 ) )
		{
			sscanf(argv[i]+20, "%d", &client_data.migration_timeout);
			if ( client_data.disconnect_timeout == 0 )
				throw "Invalid disconnect timeout (must be non-zero)";
			/* client imputs in seconds but variable holds miliseconds */
			client_data.migration_timeout *= 1000;
			continue;
		}

		if ( !strcmp(argv[i], "--debug-AI") )
		{
			client_data.debug_AI = true;
			continue;
		}

		if ( !strncmp(argv[i], "--fps_average_param=", 20 ) )
		{
			sscanf(argv[i]+20, "%f", &client_data.fps_average_param);
			continue;
		}

		printf("[WARNING] Unknown parameter '%s'\n", argv[i]);
	}

	/* get server name and port */
	client_data.server_name = strdup(argv[argc-1]);
	if ( client_data.server_name == NULL ) throw "Not enough memory";
	port_ptr = strchr(client_data.server_name, ':');
	if ( port_ptr == NULL ) throw "No port specified";
	*(port_ptr++) = 0;
	sscanf(port_ptr, "%d", &client_data.server_port);
	if ( client_data.server_port < 1 ) throw "Port number must be positive";
	printf("Connecting to server %s:%d\n", client_data.server_name, client_data.server_port);

	/* use DNS to get server ip address */
	SDLNet_ResolveHost(&client_data.server_address,
		client_data.server_name, client_data.server_port);
}

void finish(ClientData &client_data)
{
	/* free client_data */
	SDL_DestroyMutex(client_data.term_mutex);
	SDL_DestroyCond(client_data.term_cond);
}

int run_client(int argc, char *argv[])
{
	ClientData client_data;

	try
	{
		init(argc, argv, client_data);

		/* create server modules */

		MessageModule *comm_module = new MessageModule(0, 1, 0);	assert( comm_module );
				
		/* CLIENT ACTIONS */
		ClientActionModule *action_module = new ClientActionModule(&client_data, comm_module);
		if ( action_module == NULL ) throw "Cannot create action module";
		SDL_Thread *action_thread = SDL_CreateThread(module_thread, (void*)action_module);
		if ( action_thread == NULL ) throw "Cannot create action thread";

		/* RENDER */
		ClientRenderModule *render_module = new ClientRenderModule(&client_data);
		if ( render_module == NULL ) throw "Cannot create input module";

		render_module->run();

		/* terminate */
		finish(client_data);

	} catch ( const char *err ) {
		printf("[ERROR]%s\n", err);
		return -1;
	}

	printf("Client terminated without errors\n");
	return 0;
}

/***************************************************************************************************
*
* Client MAIN
*
***************************************************************************************************/

int main(int argc, char *argv[])
{
	int result = 0;

	/* initialize SDL */
	if ( SDL_Init( SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE ) < 0 )
	{
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		printf("Failed to initialize SDL (SDL_Init)");
		return -1;
	}
	if ( SDLNet_Init() < 0 )
	{
		printf("Failed to initialize SDL_net (SDLNet_Init)");
		return -1;
	}

	result = run_client(argc, argv);

	/* finish SDL */
	SDLNet_Quit();
	SDL_Quit();

	return result;
}

