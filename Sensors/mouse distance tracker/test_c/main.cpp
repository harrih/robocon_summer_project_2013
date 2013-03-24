/*
 * hworld.cpp
 */

#include "wx/wx.h"

#include <unistd.h>

class MyApp: public wxApp
{
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	long int m_x, m_y;

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void mouseXY(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit = 1,
    ID_About,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit, MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)
    EVT_MOTION(MyFrame::mouseXY)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( _("Hello World"), wxPoint(50, 50),  wxSize(500, 500) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame( NULL, -1, title, pos, size )
{
    wxMenu *menuFile = new wxMenu;
	//this->CaptureMouse();
	//this->Release Mouse();
    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("&File") );

    SetMenuBar( menuBar );
	m_x = 0;
	m_y = 0;
    CreateStatusBar();
    SetStatusText( _("Welcome to wxWidgets!") );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( _("This is a wxWidgets Hello world sample"),
                  _("About Hello World"),
                  wxOK | wxICON_INFORMATION, this);
}

void MyFrame::mouseXY(wxMouseEvent& event)
{
	long t_x, t_y;
	event.GetPosition(&t_x, &t_y);
	wxString temp = _("Pos: ") + wxString::Format(wxT("%d"), (int)t_x) + _(", ") + wxString::Format(wxT("%d"), (int)t_y);
	m_x += t_x-250;
	m_y += t_y-250;
	temp += _("    TotPos: ") + wxString::Format(wxT("%d"), (int)m_x) + _(", ") + wxString::Format(wxT("%d"), (int)m_y);
	//event.GetLogicalPosition(&m_x, &m_y);
	//temp += _("    LogicalPos: ") + wxString::Format(wxT("%d"), (int)m_x) + _(", ") + wxString::Format(wxT("%d"), (int)m_y);
	if(event.Moving()) // it will always be moving ...
		temp += _("    Moving");
	//else
	//	temp += _("    Stationery");
	SetStatusText( temp );
	this->WarpPointer(250, 250);
	usleep(1000);
}
