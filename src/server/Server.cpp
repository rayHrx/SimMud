
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

#include "../General.h"
#include "../utils/Configurator.h"
#include "../utils/SDL_barrier.h"

#include "../comm/MessageModule.h"

#include "ServerData.h"
#include "WorldUpdateModule.h"
//#include "PeriodicEventsModule.h"
//#include "StatisticsModule.h"

int local_port = 0;
ServerData	*sd = NULL;

/***************************************************************************************************
*
* Module thread
*
***************************************************************************************************/

int module_thread(void *data)
{
	Module *module = (Module*)data;

	try{							module->run();							} 
	catch( const char *err ){		printf("[ERROR] %s\n", err);exit(-1);	}

	return 0;
}

/***************************************************************************************************
*
* Initialization
*
***************************************************************************************************/

void init(int argc, char *argv[])
{
	/* interpret command line arguments */
	if ( argc < 3 )		throw "Usage: server <config_file> <port> [<log_file>]";
	
	
	/* local port */
	sscanf( argv[2], "%d", &local_port);
	if ( local_port < 1 )			throw "The port must be an integer larger than 0";
	printf( "Starting server on port %d\n", local_port );
	
	    
    srand( (unsigned int)time(NULL) );
    /*
	if ( SDL_Init( SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE ) < 0 ) // |SDL_INIT_VIDEO
	{
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		throw "Failed to initialize SDL (SDL_Init)";
	}*/
	if ( SDLNet_Init() < 0 )		throw "Failed to initialize SDL_net (SDLNet_Init)";

	/* initialize server data */
	sd = new ServerData( argv[1] );	assert( sd );
	sd->log_file = ( argc >= 4 ) ? argv[3] : NULL;
	sd->wm.generate();
}

void finish()
{
	/* finish SDL */
	SDLNet_Quit();
	//SDL_Quit();

	/* free memory */
	delete sd;
}

/***************************************************************************************************
*
* Main
*
***************************************************************************************************/

int main(int argc, char *argv[])
{
	int i;
    
	try
	{
		#ifdef __COMPRESSED_MESSAGES__
		printf("Starting server with compressed messages\n");
		#endif

		/* initialize */
		init(argc, argv);
		printf("Number of Threads @ main: %d\n",  sd->num_threads);
        
		/* create server modules */
       	MessageModule *comm_module = new MessageModule( local_port, sd->num_threads, 0 );	assert( comm_module );
		
		//* WorldUpdateModule
        SDL_barrier *wu_barrier = SDL_CreateBarrier( sd->num_threads );						assert( wu_barrier ); 
		WorldUpdateModule **wu_module = new WorldUpdateModule*[ sd->num_threads ];			assert( wu_module );			
		for ( i = 0; i < sd->num_threads; i++ )
		{
			wu_module[i] = new WorldUpdateModule( i, comm_module, wu_barrier );				assert( wu_module[i] );
		}
		
		
		//* User input loop (type 'quit' to exit)
		char cmd[256]; 
		while ( true )
		{			
			scanf("%s", cmd);
			if ( !strcmp(cmd, "exit") || !strcmp(cmd, "quit") || !strcmp(cmd, "q") )	exit(0);
		}		

		finish();

	} catch ( const char *err ) {
		printf("[ERROR] %s\n", err);
		exit(-1);
	}

	return 0;
}

