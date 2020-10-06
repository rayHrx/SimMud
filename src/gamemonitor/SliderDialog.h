
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

#ifndef __SLIDER_DIALOG_H
#define __SLIDER_DIALOG_H

class SliderDialog : public wxFrame
{
private:
	wxSlider sl;
	wxStaticText loc;

	SlowReader *sr;
	int records;
	int record_size;

public:
	SliderDialog(wxWindow *parent, wxPoint pos, wxSize size, SlowReader *sr);

	int getPosition();
	void setPosition(int pos);
	int getNumberOfRecords();
	void loadPosition(int pos);

protected:
	DECLARE_EVENT_TABLE()
	void OnSlider(wxScrollEvent& event);
};

#endif
