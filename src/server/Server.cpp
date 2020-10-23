
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
string serializeTime(const std::chrono::system_clock::time_point& time, const std::string& format){
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    //std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );
    return ss.str();
}

void logPerformanceMetrics(WorldUpdateModule **wu_modules,const ServerData* sd ){
	auto stamp = std::chrono::system_clock::now();

	int total_players = 0;
	for(int i = 0; i < sd->num_threads; ++ i){
		total_players += sd->wm.players[i].size();	
	}	
	string quest_setting = (sd->quest_between/1000 >= 1000) ? "noquest" : "quest";

	string test_run_name = serializeTime(stamp, "UTC_%Y-%m-%d-%H_%M_%S");
	string dir_name = string("metrics/" + test_run_name);
	if(mkdir(dir_name.c_str(), 0777) == -1){
		printf(strerror(errno));	
	}

	ofstream labelFile;
	labelFile.open(dir_name + "/label.txt");
	labelFile << string(sd->algorithm_name) + "," + quest_setting + "," + to_string(total_players);
	labelFile.close();

	for(int i = 0; i < sd->num_threads; i++ ){
		ofstream logFile;
  		logFile.open(dir_name + "/" + to_string(i) + ".csv");	
		
		int headerRow = 0;
		auto module = wu_modules[i];		

		auto t1 = module->requests_number_tracker;
		auto t2 = module->requests_time_tracker;
		auto t3 = module->updates_number_tracker;
		auto t4 = module->updates_time_tracker;

		auto t1Sample = t1->getCalculatedAverages();
		auto t2Sample = t2->getCalculatedAverages();
		auto t3Sample = t3->getCalculatedAverages();
		auto t4Sample = t4->getCalculatedAverages();

		int iterations = max(t1Sample.size(), max(t2Sample.size(), max(t3Sample.size(), t4Sample.size())));

		vector<string> rows(iterations + 1, "");
		rows[headerRow] += t1->getName() + " " + t2->getName() + " " + t3->getName() + " " + t4->getName();
	
		for(int i = 0 ; i < t1Sample.size(); ++ i){
			rows[i + 1] += to_string(t1Sample[i]);
		}
		
		for(int i = 0 ; i < t2Sample.size(); ++ i){
			rows[i + 1] += "," + to_string(t2Sample[i]);
		}
			
		for(int i = 0 ; i < t3Sample.size(); ++ i){
			rows[i + 1] += "," + to_string(t3Sample[i]);
		}
			
		for(int i = 0 ; i < t4Sample.size(); ++ i){
			rows[i + 1] += "," + to_string(t4Sample[i]);
		}
			

		for(auto row : rows){
			logFile << row << "\n";		
		}
		logFile.close();
	}
}

/***************************************************************************************************
*
* Main
*
***************************************************************************************************/
WorldUpdateModule **wu_module;
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
			if ( !strcmp(cmd, "exit") || !strcmp(cmd, "quit") || !strcmp(cmd, "q") ){
				logPerformanceMetrics(wu_module, sd);
				exit(0);
			}	
		}		
		logPerformanceMetrics(wu_module, sd);
		finish();

	} catch ( const char *err ) {
		printf("[ERROR] %s\n", err);
		exit(-1);
	}

	return 0;
}

