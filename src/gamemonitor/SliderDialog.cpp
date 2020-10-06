
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

#include <wx/wx.h>
#include <stdio.h>

#include "SDL_replace.h"
#include "Constants.h"
#include "Structs.h"

#include "SlowReader.h"
#include "SliderDialog.h"
#include "MainWindow.h"

/***************************************************************************************************
*
* Global variables (extern)
*
***************************************************************************************************/

extern MainWindow *frame;

extern Uint32 timestamp;
extern MapData map_data;
extern ThreadStatistics ts[MAX_THREADS];
extern ServerStatistics ss;
extern MasterStatistics master_stats;
extern int **layout;
extern int **players_per_region;
extern int quest_active,quest_x,quest_y;
extern wxMutex global_lock;

/***************************************************************************************************
*
* Constants
*
***************************************************************************************************/

#define ID_SLIDER	2222

/***************************************************************************************************
*
* Event table
*
***************************************************************************************************/

BEGIN_EVENT_TABLE(SliderDialog, wxFrame)
	EVT_COMMAND_SCROLL(ID_SLIDER, SliderDialog::OnSlider)
END_EVENT_TABLE()

/***************************************************************************************************
*
* Constructor
*
***************************************************************************************************/

SliderDialog::SliderDialog(wxWindow *parent, wxPoint pos, wxSize size, SlowReader *sr)
{
	/* create frame */
	Create(parent,-1, "Select time", pos, size,
		wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxCLIP_CHILDREN|wxFRAME_NO_TASKBAR);

	/* copy reader, read file size and reset reader */
	this->sr = sr;
	record_size =
		sizeof(Uint32) /* timestamp */
		+ sizeof(ThreadStatistics) * map_data.num_threads /* threads */
		+ sizeof(ServerStatistics) /* server */
        + 3 * sizeof(int) /* quest */
		+ sizeof(MasterStatistics);	/* MasterStatistics */
//		+ map_data.nregx * map_data.nregy * ( sizeof(int) + sizeof(int) ); /* regions */
	records = ( sr->size() - sizeof(MapData) ) / record_size;
	printf("Header: %d\n", (int)sizeof(MapData));
	printf("Records: %d\n", records);
	printf("Record size: %d\n", record_size);

	/* create slider and label */
	int x,y;
	GetSize(&x,&y);
	sl.Create(this,ID_SLIDER, 0,0, records-1, wxPoint(0,0),
		wxSize(x,y-20), wxSL_HORIZONTAL|wxSL_AUTOTICKS);
	loc.Create(this, -1, "", wxPoint(x-100,y-20), wxSize(90,20), wxALIGN_RIGHT);

	/* load first record */
	loadPosition(0);
	if ( frame != NULL ) frame->UpdateHTMLControls();
}

/***************************************************************************************************
*
* Events
*
***************************************************************************************************/

int SliderDialog::getPosition()
{
	return sl.GetValue();
}

void SliderDialog::setPosition(int pos)
{
	sl.SetValue(pos);
	loadPosition(pos);
}

int SliderDialog::getNumberOfRecords()
{
	return records;
}

void SliderDialog::loadPosition(int pos)
{
	int i,j;

	wxString label;
	label.Printf("%d/%d", pos,records);
	loc.SetLabel(label);

	sr->seek(sizeof(MapData) + pos * record_size);

	sr->read(&timestamp, sizeof(Uint32));
	sr->read(&ts[0], sizeof(ThreadStatistics) * map_data.num_threads);
	sr->read(&ss, sizeof(ServerStatistics));
    sr->read(&quest_active, sizeof(int));
	sr->read(&quest_x, sizeof(int));
	sr->read(&quest_y, sizeof(int));
	sr->read(&master_stats, sizeof(MasterStatistics));
/*	for ( i = 0; i < map_data.nregx; i++ )
		for ( j = 0; j < map_data.nregy; j++ )
		{
			sr->read(&layout[i][j], sizeof(int));
			sr->read(&players_per_region[i][j], sizeof(int));
		}*/
}

/***************************************************************************************************
*
* Events
*
***************************************************************************************************/

void SliderDialog::OnSlider(wxScrollEvent& event)
{
	loadPosition(event.GetPosition());
	if ( frame != NULL ) frame->UpdateHTMLControls();
}
