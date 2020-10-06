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

#ifndef __STRUCTS_H
#define __STRUCTS_H


/* set alignment to 8 bytes so it will work well on both x86 and x86_64 */
/* more details at http://en.wikipedia.org/wiki/Byte_alignment */
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 8 byte boundary */

typedef struct
{
	int x, y;
} Vector2D;


inline
int is_inside( Vector2D point, Vector2D box_origin, Vector2D box_size )
{
	if( point.x >= box_origin.x && point.x < box_origin.x + box_size.x &&
		point.y >= box_origin.y && point.y < box_origin.y + box_size.y )
		return 1;
	else
		return 0;
		
}

/***************************************************************************************************
*
* ThreadStatistics
* - information about the load on each thread
*
***************************************************************************************************/

struct ThreadStatistics
{
	int number_of_regions;
	int number_of_players;
	int players_in_most_crowded_region;
};

/***************************************************************************************************
*
* ServerStatistics
* - information about the server
*
***************************************************************************************************/

struct ServerStatistics
{
	int machine_cpu_usage;
	int machine_mem_usage;
	int process_cpu_usage;
	int process_mem_usage;
	int number_of_threads;
	double average_regular_update_interval;
	double average_real_regular_update_interval;
	double bps_tcp_recv,bps_tcp_sent;
	double bps_udp_recv,bps_udp_sent;
	double tcp_total,udp_total;
	int number_of_statistics;
};

/***************************************************************************************************
*
* MasterStatistics
* - information about player and region migration
*
***************************************************************************************************/

struct MasterStatistics
{
	int player_migrations;
	int region_migrations;
};

/***************************************************************************************************
*
* ConnectedServerInfo
* - used by the master to hold information about servers
*
***************************************************************************************************/

/*struct ConnectedServerInfo
{
	IPaddress tcp_connection;
	IPaddress udp_connection;
	ThreadStatistics statistics;
};*/


/***************************************************************************************************
*
* QuestPoint
* - used by the master to hold a list of quests
*
***************************************************************************************************/

struct QuestPoint
{
	int x,y;
	int duration;
};



#pragma pack(pop)   /* restore original alignment from stack */

#endif
