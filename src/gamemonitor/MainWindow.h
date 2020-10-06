
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

#ifndef _FERBAT_H
#define _FERBAT_H

#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>

/***************************************************************************************************
*
* MainWindow class
* - the main frame for the monitoring application
*
***************************************************************************************************/

class MainWindow : public wxFrame
{
private:
	bool offline;
	int server_columns;

	wxHtmlWindow *wh_general;
	wxHtmlWindow *wh_servers;
	wxHtmlWindow *wh_regions;
	wxHtmlWindow *wh_load;
	wxHtmlWindow *wh_help;
	wxNotebook *nb;

public:
	MainWindow(bool offline);
	void UpdateHTMLControls();

protected:
	DECLARE_EVENT_TABLE()
	void OnExit(wxCommandEvent& WXUNUSED(event));
	void OnRefresh(wxCommandEvent& WXUNUSED(event));
	void OnSave(wxCommandEvent& WXUNUSED(event));
	void OnSelectColumns(wxCommandEvent& WXUNUSED(event));
	void OnSaveReduced(wxCommandEvent& WXUNUSED(event));
	void OnAbout(wxCommandEvent& WXUNUSED(event));
	void OnTimer(wxTimerEvent& event);

	void randomColors(int n);
	wxString printAddress(IPaddress a);
	wxString printColor(wxColor c);
	wxString printDouble(double lf);

	void UpdateGeneral();
	void UpdateServers();
	void UpdateRegions();
	void UpdateLoad();
};

#endif
