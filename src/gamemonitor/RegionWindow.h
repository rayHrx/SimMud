#include <wx/wx.h>

class RegionWindow : public wxFrame
{
public:
    RegionWindow( const wxString & title);

    void OnPaint(wxPaintEvent & event);
};
