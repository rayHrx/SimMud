
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
#include <wx/wx.h>

#include "SDL_replace.h"
#include "Constants.h"
#include "Structs.h"

#include "Utils.h"
#include "SlowReader.h"
#include "MainWindow.h"
#include "SliderDialog.h"
#include "help.html.h"
#include "icon.xpm"

#define RIGHT_REGION_WIDTH	200

/***************************************************************************************************
*
* Global variables (extern)
*
***************************************************************************************************/

extern SliderDialog *sdiag;

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
int number_of_players;

int server_colors;
wxColor table_color[MAX_THREADS];

/***************************************************************************************************
*
* Constants
*
***************************************************************************************************/

#define TIMER_ID 1000

enum
{
	NOTEBOOK = 1,
	MENU_EXIT = wxID_EXIT,
	MENU_REFRESH = wxID_REFRESH,
	MENU_ABOUT = wxID_ABOUT,
	MENU_SAVE = wxID_SAVE,
	MENU_SAVE_REDUCED = 1000,
	MENU_SELECT_COLUMNS
};

/***************************************************************************************************
*
* Event table
*
***************************************************************************************************/

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(MENU_EXIT, MainWindow::OnExit)
	EVT_MENU(MENU_REFRESH, MainWindow::OnRefresh)
	EVT_MENU(MENU_SELECT_COLUMNS, MainWindow::OnSelectColumns)
	EVT_MENU(MENU_SAVE, MainWindow::OnSave)
	EVT_MENU(MENU_SAVE_REDUCED, MainWindow::OnSaveReduced)
	EVT_MENU(MENU_ABOUT, MainWindow::OnAbout)
	EVT_TIMER(TIMER_ID, MainWindow::OnTimer)
END_EVENT_TABLE()

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

MainWindow::MainWindow(bool offline) : wxFrame(NULL, -1, wxString("GameMonitor"),
	wxPoint(100,100), wxSize(640,480),
	wxDEFAULT_FRAME_STYLE )
{
	this->offline = offline;
	this->server_columns = 2;

	/* set size, icon and colors */
	SetMinSize(wxSize(400,320));
	SetIcon(wxIcon(icon_xpm));

	/* generate colors */
	randomColors(MAX_THREADS);

	/* set tabs with HTML controls */
	nb = new wxNotebook(this, NOTEBOOK);
	wh_general = new wxHtmlWindow(nb, -1);
	nb->AddPage(wh_general, _("General"));
	wh_servers = new wxHtmlWindow(nb, -1);
	nb->AddPage(wh_servers, _("Servers"));
	wh_regions = new wxHtmlWindow(nb, -1);
	nb->AddPage(wh_regions, _("Regions"));
	wh_load = new wxHtmlWindow(nb, -1);
	nb->AddPage(wh_load, _("Load"));
	wh_help = new wxHtmlWindow(nb, -1);
	nb->AddPage(wh_help, _("Help"));
	wh_help->SetPage(help_text);

	/* set menu */
	wxMenu *menu1 = new wxMenu();
	menu1->Append(MENU_REFRESH, _("&Refresh"));
	menu1->Append(MENU_SELECT_COLUMNS, _("Select number of &columns"));
	menu1->AppendSeparator();
	menu1->Append(MENU_EXIT, _("E&xit"));

	wxMenu *menu2 = new wxMenu();
	menu2->Append(MENU_SAVE, _("&Save CSV ..."));
	menu2->Append(MENU_SAVE_REDUCED, _("Save CSV &reduced ..."));

	wxMenu *menu3 = new wxMenu();
	menu3->Append(MENU_ABOUT, _("&About"));

	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menu1, _("&Menu"));
	if ( offline)
		menuBar->Append(menu2, _("&Save"));
	menuBar->Append(menu3, _("&Help"));
	SetMenuBar(menuBar);

	/* set update timer */
	wxTimer *timer = new wxTimer(this, TIMER_ID);
	if ( !offline )
		timer->Start(500, wxTIMER_CONTINUOUS);
}

/***************************************************************************************************
*
* Evemt handlers
*
***************************************************************************************************/

void MainWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exit(0);
}

void MainWindow::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
	UpdateHTMLControls();
}

void MainWindow::OnSelectColumns(wxCommandEvent& WXUNUSED(event))
{
	/* get ratio */
	int res = (int)wxGetNumberFromUser( _T("Select the number of columns\n")
		_T("for the server tab."),
		_T("Columns: "), _T("Number of columns"),
		server_columns, 1, 10, this );
	if ( res == -1 ) return;
	server_columns = res;
	UpdateServers();
}

void MainWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
	int k;
	if ( sdiag == NULL ) return;

	/* get file name */
	wxFileDialog dialog(this,
		_T("Save CSV file"),
		wxEmptyString,
		_T("statistics.csv"),
		_T("Text CSV (*.csv)|*.csv|Text files (*.txt)|*.txt"),
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	dialog.SetFilterIndex(0);
	if (dialog.ShowModal() != wxID_OK) return;
	printf("%s (%d)\n", dialog.GetPath().c_str(), dialog.GetFilterIndex());

	/* get current position */
	int pos = sdiag->getPosition();

	/* open file */
	FILE *f = fopen(dialog.GetPath().c_str(), "w");
	if ( f == NULL )
	{
		printf("Cannot save file %s\n", dialog.GetPath().c_str());
		wxMessageBox("Cannot save file", "ERROR", wxICON_ERROR);
		return;
	}
	fprintf(f, "Timestamp;Number of players;Number of regions;Quest active;");
	for ( k = 0; k < map_data.num_threads; k++ )
		fprintf(f, "Players S%d;Regions S%d;Density S%d;Crowded %d;CPU S%d;MEM S%d (MB);PMEM S%d (MB);Update interval S%d (ms);"
			"TCP send S%d (kbps);TCP recv S%d (kbps);"
			"UDP send S%d (kbps);UDP recv S%d(kbps);",
			k,k,k,k,k,k,k,k,k,k,k,k);
	fprintf(f,"\n");

	/* save data */
	int n = sdiag->getNumberOfRecords();
	for ( int i = 0; i < n; i++ )
	{
		sdiag->loadPosition(i);
		number_of_players = 0;
		for ( int j = 0; j < map_data.num_threads; j++ )
			number_of_players += ts[j].number_of_players;
		fprintf(f, "%d;%d;%d;%d;",
			timestamp,number_of_players,
			(MAX_REGIONS),quest_active);
		for ( k = 0; k < map_data.num_threads; k++ )
			fprintf(f, "%d;%d;%.2f;%d;%d;%.2f;%.2f;%d;%.2f;%.2f;%.2f;%.2f;",
				ts[k].number_of_players,
				ts[k].number_of_regions,
				((ts[k].number_of_regions == 0 ) ? 0.0 :
					( (float)ts[k].number_of_players /
					(float)ts[k].number_of_regions ) ),
				ts[k].players_in_most_crowded_region,
				ss.machine_cpu_usage,
				ss.machine_mem_usage / 1024.0,
				ss.process_mem_usage / 1024.0,
				(int)ss.average_regular_update_interval,
				ss.bps_tcp_sent / 1000.0,
				ss.bps_tcp_recv / 1000.0,
				ss.bps_udp_sent / 1000.0,
				ss.bps_udp_recv / 1000.0);
		fprintf(f, "\n");
	}

	/* close file */
	fclose(f);

	/* restore last position */
	sdiag->setPosition(pos);
}

void MainWindow::OnSaveReduced(wxCommandEvent& WXUNUSED(event))
{
	int k;
	if ( sdiag == NULL ) return;

	/* get ratio */
	int res = (int)wxGetNumberFromUser( _T("Enter the ratio to reduce the data.\n")
		_T("from the original file."),
		_T("1 /"), _T("Save CSV reduced ..."),
		10, 1, 10000, this );
	if ( res == -1 ) return;

	/* get file name */
	wxFileDialog dialog(this,
		_T("Save CSV reduced file"),
		wxEmptyString,
		_T("statistics.csv"),
		_T("Text CSV (*.csv)|*.csv|Text files (*.txt)|*.txt"),
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	dialog.SetFilterIndex(0);
	if (dialog.ShowModal() != wxID_OK) return;
	printf("%s (%d) - ratio 1 / %d\n", dialog.GetPath().c_str(), dialog.GetFilterIndex(), res);

	/* get current position */
	int pos = sdiag->getPosition();

	/* open file */
	FILE *f = fopen(dialog.GetPath().c_str(), "w");
	if ( f == NULL )
	{
		printf("Cannot save file %s\n", dialog.GetPath().c_str());
		wxMessageBox("Cannot save file", "ERROR", wxICON_ERROR);
		return;
	}
	fprintf(f, "Timestamp;Number of players;Number of regions;Quest active;");
	for ( k = 0; k < map_data.num_threads; k++ )
		fprintf(f, "Players S%d;Regions S%d;CPU S%d;Update interval S%d (ms);"
			"TCP send S%d (kbps);TCP recv S%d (kbps);"
			"UDP send S%d (kbps);UDP recv S%d(kbps);",
			k,k,k,k,k,k,k,k);
	fprintf(f,"\n");

	/* save data */
	int n = sdiag->getNumberOfRecords();
	for ( int i = 0; i < n; i++ )
	{
		if ( i % res != 0 ) continue;
		sdiag->loadPosition(i);
		number_of_players = 0;
		for ( int j = 0; j < map_data.num_threads; j++ )
			number_of_players += ts[j].number_of_players;
		fprintf(f, "%d;%d;%d;%d;",
			timestamp,number_of_players,
			(MAX_REGIONS),quest_active);
		for ( k = 0; k < map_data.num_threads; k++ )
			fprintf(f, "%d;%d;%d;%d;%.2f;%.2f;%.2f;%.2f;",
				ts[k].number_of_players,
				ts[k].number_of_regions,
				ss.machine_cpu_usage,
				(int)ss.average_regular_update_interval,
				ss.bps_tcp_sent / 1000.0,
				ss.bps_tcp_recv / 1000.0,
				ss.bps_udp_sent / 1000.0,
				ss.bps_udp_recv / 1000.0);
		fprintf(f, "\n");
	}

	/* close file */
	fclose(f);

	/* restore last position */
	sdiag->setPosition(pos);
}

void MainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(_T("GameMonitor"));
	info.SetVersion(wxVERSION_NUM_DOT_STRING_T);
	info.SetDescription(_T("This application is used to monitor the game servers"));
	info.SetCopyright(_T("(C) 2007 Mihai Paslariu"));
	info.AddDeveloper(_T("Mihai Paslariu"));
	wxAboutBox(info);
}

void MainWindow::OnTimer(wxTimerEvent& event)
{
	UpdateHTMLControls();
}

/***************************************************************************************************
*
* Formatting methods
*
***************************************************************************************************/

void MainWindow::randomColors(int n)
{
	if ( n > MAX_THREADS ) n = MAX_THREADS;

		int base = rand() % 360;
	int dif = 360 / n;
	for ( int i = 0; i < n; i++ )
	{
		int rr,gg,bb;
		hsv2rgb( (base +  dif * i) % 360 , 110,255, &rr,&gg,&bb);
		table_color[i] = wxColor(rr,gg,bb);
	}
	server_colors = n;
}

wxString MainWindow::printAddress(IPaddress a)
{
	wxString s;
	Uint8 b1,b2,b3,b4;
	Uint16 port;

	port = ( ( a.port & 0xFF ) << 8 ) | ( a.port >> 8 );
	b1 = a.host & 0xFF;
	b2 = ( a.host >> 8 )& 0xFF;
	b3 = ( a.host >> 16 )& 0xFF;
	b4 = ( a.host >> 24 )& 0xFF;

	s.Printf("%u.%u.%u.%u:%d", b1,b2,b3,b4, port);
	return s;
}

wxString MainWindow::printColor(wxColor c)
{
	wxString s;
	s.Printf("#%X%X%X%X%X%X",
		c.Red() >> 4, c.Red() & 0xF,
		c.Green() >> 4, c.Green() & 0xF,
		c.Blue() >> 4, c.Blue() & 0xF);
	return s;
}

wxString MainWindow::printDouble(double lf)
{
	wxString s;
	s.Printf("%.2lf", lf);
	return s;
}

/***************************************************************************************************
*
* Update Methods
*
***************************************************************************************************/

void MainWindow::UpdateHTMLControls()
{
	global_lock.Lock();

	if ( map_data.num_threads < server_colors )
		randomColors(map_data.num_threads);

	UpdateGeneral();
	UpdateServers();
	UpdateRegions();
	UpdateLoad();

	global_lock.Unlock();
}

void MainWindow::UpdateGeneral()
{
	wxString c;
	c << "<html><head><title>Game Monitor</title></head><body>";
	c << "<b>Last timestamp: </b>" << timestamp << "<br />";
	c << "<b>Number of servers: </b>" << map_data.num_threads << "<br />";
	number_of_players = 0;
	for ( int i = 0; i < map_data.num_threads; i++ )
		number_of_players += ts[i].number_of_players;
	c << "<b>Number of players: </b>" << number_of_players << "<br />";
	c << "<b>Regular update interval (SLA): </b>"
		<< map_data.regular_update_interval << "<br />";
	c << "<b>Partitioning algorithm: </b>"
		<< (char*)map_data.algorithm_name << "<br />";
	c << "<br />";

	c << "<b>Map size: </b>" << map_data.mapx << " x "
		<< map_data.mapy << "<br />";
	c << "<b>Region min size: </b>" << map_data.regminx << " x "
		<< map_data.regminy << "<br />";
	c << "<br />";


	c << "<b>Min/Max quest interval: </b>" << map_data.quest_min << "/"
		<< map_data.quest_max << " seconds<br />";
	c << "<b>Maximum interval between quests: </b>" << map_data.quest_between << " seconds<br />";
	if ( quest_active )
		c << "<b>Current quest:</b> " << quest_x << "," << quest_y << "<br />";
	else
		c << "<b>Current quest:</b> no quest active<br />";
	c << "<b>Quest bonus: </b>" << map_data.quest_bonus << "<br />";
	c << "<br />";

	c << "<b>Number of player migrations: </b>" << master_stats.player_migrations << "<br />";
	c << "<b>Number of region migrations: </b>" << master_stats.region_migrations << "<br />";
	c << "<br />";

	c << "</body></html>";
	wh_general->SetPage(c);
}

void MainWindow::UpdateServers()
{
	wxString c;
	double x;
	c << "<html><head><title>Game Monitor</title></head><body>";

	c << "<table border=\"0\" width=\"100%\">";
	for ( int i = 0; i < map_data.num_threads; i++ )
	{
		if ( i % server_columns == 0 ) c << "<tr>";
		c << "<td bgcolor=\"" << printColor(table_color[i]) << "\">";
		c << "<h5><b>Server " << i << " - ";
		//c << "<font color=\"#0000ff\"> " << printAddress(ss[i].udp_connection) << "</font>";
		c << "<font color=\"#0000ff\"> " << i << "</font>";
		c << "</b></h5><br/><br />";
		c << "<font size=\"2\">";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Number of regions: </b>" <<
			ts[i].number_of_regions << "<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Number of players: </b>" <<
			ts[i].number_of_players << "<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Player density: </b>" <<
			(( ts[i].number_of_regions == 0 ) ? 0 :
			( ts[i].number_of_players / ts[i].number_of_regions ))
			<< "<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Players in most crowded region: </b>" <<
			ts[i].players_in_most_crowded_region << "<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>CPU usage (machine): </b>" <<
			ss.machine_cpu_usage << "%<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Memory usage (machine): </b>"
			<< (ss.machine_mem_usage/1024) << "MB<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>CPU usage (process): </b>" <<
			ss.process_cpu_usage << "%<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Memory usage (process): </b>"
			<< printDouble(ss.process_mem_usage/1024.0) << "MB<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Number of threads: </b>" <<
			ss.number_of_threads << "<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Average regular update interval: </b>"
			<< (int)ss.average_regular_update_interval << " ms<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Real last regular update interval: </b>"
			<< (int)ss.average_real_regular_update_interval << " ms<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Master TCP connection (sent): </b>"
			<< printDouble(ss.bps_tcp_sent / 1000.0 ) << " kb/s<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Master TCP connection (recv): </b>"
			<< printDouble(ss.bps_tcp_recv / 1000.0 ) << " kb/s<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Clients UDP connection (sent): </b>"
			<< printDouble(ss.bps_udp_sent / 1000.0 ) << " kb/s<br />";
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Clients UDP connection (recv): </b>"
			<< printDouble(ss.bps_udp_recv / 1000.0 ) << " kb/s<br />";
		x = ( ss.number_of_statistics == 0 ) ? 0 :
			( ss.tcp_total / ss.number_of_statistics );
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Average TCP transfer: </b>"
			<< printDouble( x / 1000.0 ) << " kb/s<br />";
		x = ( ss.number_of_statistics == 0 ) ? 0 :
			( ss.udp_total / ss.number_of_statistics );
		c << "&nbsp;&nbsp;&nbsp;&nbsp;<b>Average UDP transfer: </b>"
			<< printDouble( x / 1000.0 ) << " kb/s<br />";
		c << "</font>";
		c << "</td>";
		if ( i % server_columns == server_columns - 1 ) c << "</tr>";
	}
	c << "</table>";
	c << "</body></html>";
	wh_servers->SetPage(c);
}

void MainWindow::UpdateRegions()
{
/*	wxString c;
	int i,j,np;

	np = 0;


	c << "<html><head><title>Game Monitor</title></head><body>";
	c << "<font size=\"2\">";

	c << "<table border=\"0\"><tr>";
	for ( int i = 0; i < map_data.num_threads; i++ )
	{
		c << "<td bgcolor=\"" << printColor(table_color[i]) << "\">";
		c << "<b>S" << i << "</b>: " << ts[i].number_of_players;
		np += ts[i].number_of_players;
		c << "</td>";
	}
	c << "<td bgcolor=\"#FFFFFF\"><b>Total</b>: " << number_of_players << "</td>";
	c << "<td bgcolor=\"#FFFFFF\">(from map: " << np << ")</td>";
	c << "</tr></table>";

	c << "<table width=\"100%\" border=\"0\">";
	for ( i = 0; i < map_data.nregx; i++ )
	{
		c << "<tr valign=\"center\">";
		for ( j = 0; j < map_data.nregy; j++ )
		{
			c << "<td align=\"center\" bgcolor=\"";
			if ( layout[i][j] >= 0 && layout[i][j] < MAX_THREADS )
				c << printColor(table_color[layout[i][j]]) << "\" >";
			else
				c << "white" << "\" >";
			c << "<b>S" << layout[i][j] << " </b> ";
			c << "(" << i << "," << j << ") ";
			if ( quest_x / map_data.regx == i && quest_y / map_data.regy == j
				&& quest_active )
				c << "<font color=\"red\"><b>Q</b></font>";
			c << "<br />";
			c << players_per_region[i][j] << " players<br />";
			//c << (i * map_data.regx) << "," << (j * map_data.regy);
			c << "</td>";
		}
		c << "</tr>";
	}
	c << "</table>";

	c << "</font>";
	c << "</body></html>";
	wh_regions->SetPage(c);*/
}

void MainWindow::UpdateLoad()
{
/*	wxString c;
	wxColor color;
	double rv;
	int v;

	c << "<html><head><title>Game Monitor</title></head><body>";
	c << "<font size=\"2\">";
	c << "<table width=\"100%\" border=\"0\">";
	for ( int i = 0; i < map_data.nregx; i++ )
	{
		c << "<tr valign=\"center\">";
		for ( int j = 0; j < map_data.nregy; j++ )
		{
			// get color 
			if ( number_of_players == 0 )
			{
				color = wxColor(0,255,0);
			} else {
				rv = players_per_region[i][j] / (double)number_of_players;
				v = (int)(255 * rv);
				color = wxColor(128 + v/2,255-v,0);
			}

			// display page 
			c << "<td align=\"center\" bgcolor=\"";
			if ( layout[i][j] >= 0 && layout[i][j] < MAX_THREADS )
				c << printColor(color) << "\" >";
			else
				c << "white" << "\" >";
			c << "<b>S" << layout[i][j] << " </b> ";
			c << "(" << i << "," << j << ") ";
			if ( quest_x / map_data.regx == i && quest_y / map_data.regy == j
				&& quest_active )
				c << "<font color=\"red\"><b>Q</b></font>";
			c << "<br />";
			c << players_per_region[i][j] << " players<br />";
			c << (i * map_data.regx) << "," << (j * map_data.regy);
			c << "</td>";
		}
		c << "</tr>";
	}
	c << "</table>";
	c << "</font>";
	c << "</body></html>";
	wh_load->SetPage(c);*/
}
