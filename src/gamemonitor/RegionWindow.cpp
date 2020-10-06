#include "RegionWindow.h"

#include "Structs.h"
#include "Constants.h"

#define WINDOW_X 640
#define WINDOW_Y 480

extern MapData md;
    //num_threads
    //mapx, mapy ->scale
extern ServerStatistics ss;
    //machine_cpu_usage, etc
extern ThreadStatistics ts[MAX_THREADS];
    //number_of_regions, number_of_players
extern RegionInfo ri[MAX_REGIONS];
    //x, y, sizex, sizey, num_players, thread


RegionWindow::RegionWindow(const wxString& title)
            : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_X, WINDOW_Y))
{
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(RegionWindow::OnPaint));
    this->Centre();
}

void RegionWindow::OnPaint(wxPaintEvent & event)
{
    int region;
    int scale;

    if (WINDOW_X / md.mapx <= WINDOW_Y / md.mapy)
    {
        scale = WINDOW_X / md.mapx;
    }
    else
    {
        scale = WINDOW_Y / md.mapy;
    }

    wxPaintDC dc(this);
    wxColour gray, white, red, blue;
    wxColour orange, green, brown, black;
    wxColour c;

    gray.Set(wxT("#d4d4d4"));
    white.Set(wxT("#ffffff"));
    red.Set(wxT("#ff0000"));
    orange.Set(wxT("#fa8e00"));
    green.Set(wxT("#619e1b"));
    brown.Set(wxT("#715b33"));
    blue.Set(wxT("#0d0060"));
    black.Set(wxT("#000000"));

    int num_regions = 0;
    for (int i=0; i<ss.number_of_threads; i++)
    {
        num_regions += ts[i].number_of_players;
    }

    for (region = 0; region < num_regions; region++)
    {
        switch (ri[region].thread)
        {
            case 0: c = red; break;
            case 1: c = blue; break;
            case 2: c = orange; break;
            case 3: c = green; break;
            default: c = brown; break;
        }
        dc.SetBrush(wxBrush(c));
        dc.DrawRectangle(ri[region].x*scale, ri[region].y*scale, ri[region].sizex*scale, ri[region].sizey*scale);
        dc.SetBrush(wxBrush(black));
        //TODO: some sprintf("%d",ri[region].num_players); 
        dc.DrawText(wxT("SIZE LAWL"), (ri[region].x + ri[region].sizex/2)*scale, (ri[region].y+ri[region].sizey/2)*scale);
    }
}

