
/***************************************************************************************************
*
* SUBJECT:
*    A monitoring application for the Massive Multiplayer Online Game benchmark
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

#include <stdio.h>
#include <string.h>
#include <wx/wx.h>

#include "SDL_replace.h"
#include "Constants.h"
#include "Structs.h"

#include "MainWindow.h"
#include "SlowReader.h"
#include "ReadThread.h"

/***************************************************************************************************
*
* Global variables (extern)
*
***************************************************************************************************/

extern Uint32 timestamp;
extern MapData map_data;
extern MasterStatistics master_stats;
extern ThreadStatistics ts[MAX_THREADS];
extern ServerStatistics ss;
extern int **layout;
extern int **players_per_region;
extern RegionInfo ri[MAX_REGIONS];
extern int quest_active,quest_x,quest_y;
extern wxMutex global_lock;

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

ReadThread::ReadThread(SlowReader *sr) : wxThread()
{
	this->sr = sr;
	Create();
}

/***************************************************************************************************
*
* Thread entry point
*
***************************************************************************************************/

void *ReadThread::Entry()
{
	int i,j;
	Uint32 timestamp2;
	ThreadStatistics ts2[MAX_THREADS];
    ServerStatistics ss2;
	int quest_active2,quest_x2,quest_y2;

	//layout2 = new_int_matrix(map_data.nregx, map_data.nregy);
	//players_per_region2 = new_int_matrix(map_data.nregx, map_data.nregy);

    RegionInfo ri2[MAX_REGIONS];

	while ( true )
	{
		/* read data */
		sr->read(&timestamp2, sizeof(Uint32));
		sr->read(&ts2[0], sizeof(ThreadStatistics) * map_data.num_threads);
		//sr->read(&ss2, sizeof(ServerStatistics));
        //sr->read(&quest_active2, sizeof(int));
		//sr->read(&quest_x2, sizeof(int));
		//sr->read(&quest_y2, sizeof(int));
		//sr->read(&master_stats, sizeof(MasterStatistics));
		//for ( i = 0; i < map_data.nregx; i++ )
		//	for ( j = 0; j < map_data.nregy; j++ )
		//	{
		//		sr->read(&layout2[i][j], sizeof(int));
		//		sr->read(&players_per_region2[i][j], sizeof(int));
		//	}
        
        /* read one byte, -1 for done, make data structure for region info */
        for (i=0; i<MAX_REGIONS; i++)
        {
            sr->read(&ri2[i], sizeof(RegionInfo));
            if (ri2[i].x == -1) break;
        }

		/* update global variables */
		global_lock.Lock();
		timestamp = timestamp2;
		memcpy(&ts[0], &ts2[0], sizeof(ThreadStatistics) * map_data.num_threads);
		memcpy(&ss, &ss2, sizeof(ServerStatistics));
		quest_active = quest_active2;
		quest_x = quest_x2;
		quest_y = quest_y2;
		/*for ( i = 0; i < map_data.nregx; i++ )
			for ( j = 0; j < map_data.nregy; j++ )
			{
				layout[i][j] = layout2[i][j];
				players_per_region[i][j] = players_per_region2[i][j];
			}*/
		global_lock.Unlock();
	}
}

/***************************************************************************************************
*
* Utility method
*
***************************************************************************************************/

int **ReadThread::new_int_matrix(int m, int n)
{
	int i;
	int **a;

	a = new int*[m];
	for ( i = 0; i < m; i++ ) a[i] = new int[n];
	return a;
}
