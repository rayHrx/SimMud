
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

#ifndef __SERVERDATA_H
#define __SERVERDATA_H

#include "../General.h"
#include "../utils/Configurator.h"

#include "WorldMap.h"


struct ServerData
{
public:	
	WorldMap	wm;	
	
	char *log_file;

	/* server */
	int num_threads;				/* number of threads */
	int regular_update_interval;	/* maximum time between two consecutive client updates */

	/* load-balance */
	char algorithm_name[64];	/* name of the load balancing algorithm */
	double overloaded_level;	/* overloaded and light server level */
	double light_level;
	Uint32 load_balance_limit;

    /* stats */
    int stats_interval;

	/* players */
	/* (values are between 1 and 100, exept for max_life which is between 41 and 100) */
	int player_min_life;
	int player_max_life;
	int player_min_attr;
	int player_max_attr;

	/* quests */
	int quest_bonus;		/* the bonus given for quests */
	int quest_first;		/* time until the first quest occurs */
	int quest_between;		/* maximum time between quests in seconds */
	int quest_min, quest_max;	/* the minimum and maximum duration of quests in seconds */
	
	int send_start_quest, send_end_quest;
	Vector2D quest_pos;	

	/* messages to display */
	int display_all_warnings;	/* to display all warning messages */
	int display_quests;
	int display_actions;
	int display_user_on_off;
	int display_migrations;	


	ServerData( char* conf_file );
	
private:
	void dataFromConfigurator( Configurator &conf );
	
};

extern ServerData *sd;


#endif
