
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
#include <time.h>
#include <wx/wx.h>
#include "General.h"

#include "Constants.h"
#include "Structs.h"

#include "SlowReader.h"
#include "MainWindow.h"

//#include "RegionWindow.h"
//#include "ParsePacket.h"

#include "SliderDialog.h"

#include "../comm/MessageModuleIN.h"
#include "../comm/MessageModuleOUT.h"

/***************************************************************************************************
*
* Global variables
*
***************************************************************************************************/

SlowReader sr;
MainWindow *frame = NULL;
//RegionWindow *RW = NULL;
SliderDialog *sdiag = NULL;

MapData md;
MasterStatistics master_stats;
Uint32 timestamp;
ThreadStatistics ts[MAX_THREADS];
ServerStatistics ss;
RegionInfo ri[MAX_REGIONS];
int **layout;
int **players_per_region;
int quest_active,quest_x,quest_y;
wxMutex global_lock;

/***************************************************************************************************
*
* Application class
*
***************************************************************************************************/

class MiniApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

IMPLEMENT_APP(MiniApp)

bool MiniApp::OnInit()
{
	bool offline = false;
	wxChar *filename;
    char *server_name;
    char *server_port;
    int port;
    IPaddress server_address;
    //ParsePacket *parse = new ParsePacket();

	if ( argc < 2 )
	{
		wxMessageBox("GameMonitor requires location of game server",
		"Error", wxOK | wxCENTRE | wxICON_ERROR);
		printf("Error: GameMonitor requires location of game server\n");
		return false;
	}

    server_name = strdup(argv[1]);
    server_port = strchr(server_name, ':');
    *(server_port++) = 0;
    sscanf(server_port, "%d", &port);
    printf("Connecting to server %s:%d\n", server_name, port);
    SDLNet_ResolveHost(&server_address, server_name, port);

    UDPsocket udpsock = SDLNet_UDP_Open(0);
    int channel = SDLNet_UDP_Bind(udpsock, -1, &server_address);

    UDPpacket* packet = SDLNet_AllocPacket(sizeof(Uint32) + //timestamp
                                           sizeof(ThreadStatistics)*MAX_THREADS + //thread stats
                                           sizeof(RegionInfo)*MAX_REGIONS);

	//RW = new RegionWindow(wxT("RegionWindow"));

    //SetTopWindow(RW);
    while(1)
    {
        int result = 0;
        while( 0 == result )
        {
            //result = SDLNet_UDP_Recv(udpsock, packet);
        }
        if (result == -1)
        {
            printf("Error receiving packet\n");
        }
        else
        {
            //Parse packet data
            //parse->Parse(packet);
            //RW->Show(true);
        }
    }

	return true;
}

int MiniApp::OnExit()
{
	//sr.close();
	return 0;
}
