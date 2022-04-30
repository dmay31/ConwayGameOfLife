// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/textfile.h>
#include "wx/toolbar.h"
#include <wx/slider.h>
#include <wx/mstream.h>

#include "config.h"
#include "GlPane.h"
#include "Conway.h"

#include "resources.h"

wxDEFINE_EVENT(RUN_EVENT_TYPE, wxCommandEvent);
wxDEFINE_EVENT(CLEAR_EVENT_TYPE, wxCommandEvent);
wxDEFINE_EVENT(PAUSE_EVENT_TYPE, wxCommandEvent);

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;
const int ID_TOOLBAR = 500;
const int ID_SLIDER = 100;

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	GlPane * glPane;
};
class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	int GetUpdateSpeed();

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnRun(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnPause(wxCommandEvent& event);

	wxMenuBar * m_pMenuBar;
	wxMenu  * m_pFileMenu;
	wxToolBar * m_pToolbar;
	wxSlider * SpeedSlider;

	wxDECLARE_EVENT_TABLE();
};
enum
{
	ID_Hello = 1
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
EVT_MENU(RUN_EVENT_TYPE, MyFrame::OnRun)
EVT_MENU(CLEAR_EVENT_TYPE, MyFrame::OnClear)
EVT_MENU(PAUSE_EVENT_TYPE, MyFrame::OnPause)
EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame("Conway's Game of Life", wxPoint(50, 50), wxSize(WIDTH, HEIGHT));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	glPane = new GlPane((wxFrame*)frame, args);
	sizer->Add(glPane, 1, wxEXPAND);

	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);
	frame->Show(true);

	return true;
}
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{

	m_pMenuBar = new wxMenuBar();
	m_pFileMenu = new wxMenu();
	wxToolBarBase *toolBar = GetToolBar();
	long style = TOOLBAR_STYLE;
	m_pToolbar = CreateToolBar(style, ID_TOOLBAR);

	wxInitAllImageHandlers();

	wxImage playImg, pauseImg;
	wxMemoryInputStream playStream(play_ico, play_ico_len);
	wxMemoryInputStream pauseStream(pause_ico, pause_ico_len);
	playImg.LoadFile(playStream, wxBITMAP_TYPE_ICO);
	pauseImg.LoadFile(pauseStream, wxBITMAP_TYPE_ICO);
	m_pToolbar->SetToolBitmapSize(wxSize(playImg.GetWidth(), playImg.GetHeight()));
	m_pToolbar->SetToolBitmapSize(wxSize(pauseImg.GetWidth(), pauseImg.GetHeight()));

	SpeedSlider = new wxSlider(m_pToolbar, ID_SLIDER, 0, 0, 100);

	m_pToolbar->AddTool(RUN_EVENT_TYPE, "",
		playImg, wxNullBitmap, wxITEM_NORMAL,
		"New file", "This is help for new file tool");

	m_pToolbar->AddTool(PAUSE_EVENT_TYPE, "",
		pauseImg, wxNullBitmap, wxITEM_NORMAL,
		"New file", "This is help for new file tool");

	m_pToolbar->AddSeparator();

	m_pToolbar->AddControl(SpeedSlider, wxString("Speed"));

	m_pToolbar->Realize();

	m_pFileMenu->Append(wxID_OPEN, _T("&Open"));
	m_pFileMenu->Append(RUN_EVENT_TYPE, _T("&Run"));
	m_pFileMenu->Append(CLEAR_EVENT_TYPE, _T("&Clear"));
	m_pMenuBar->Append(m_pFileMenu, _T("&File"));

	SetMenuBar(m_pMenuBar);

	Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(MyFrame::OnOpen));

}

void MyFrame::OnOpen(wxCommandEvent& event) {
	wxFileDialog
		openFileDialog(this, _("Open RLE file"), "", "",
			"RLE files (*.rle)|*.rle", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	wxTextFile file;
	std::list<Square> Squares;

	int xOffset = 20;
	int yOffset = 20;
	unsigned int x = xOffset;
	unsigned int y = yOffset;

	file.Open(openFileDialog.GetPath());
	for (auto i = 0; i < file.GetLineCount(); i++)
	{
		auto line = file.GetLine(i);

		if (line[0] == '#') {
			continue;
		}
		else if (line[0] == 'x') {
			continue;
		}
		else {
			unsigned int idx = 0;
			unsigned int anchor = 0;
			bool isNumber = false;
			unsigned int number = 1;

			
			while (idx < line.length()) {
				number = 1;
				anchor = idx;
				isNumber = false;
				while (line[idx] >= '0' && line[idx] <= '9') {
					isNumber = true;
					idx++;
				}
				if (isNumber) {
					char numString[4];
					snprintf(numString, idx - anchor + 1, "%s", line.mb_str().data()+anchor);
					number = atoi(numString);
					anchor = idx;
				}

				if (line[idx] == 'o') {
					for (auto n = 0; n < number; n++) {
						Squares.push_back(Square(x, y));
						x++;
					}
					idx++;
				}
				else if (line[idx] == 'b') {
					x += number;
					idx++;
				}
				else if (line[idx] == '$') {
					y++;
					x = xOffset;
					idx++;
				}
				else if (line[idx] == '!') {
					break;
				}
			}

		}
	}
	
	ConwayEngine * instance = ConwayEngine::getInstance();
	instance->addSquares(Squares);

}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a wxWidgets' Hello world sample",
		"About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnRun(wxCommandEvent& event) {
	ConwayEngine * instance = ConwayEngine::getInstance();
	instance->Run(true);
}

void MyFrame::OnClear(wxCommandEvent& event) {
	ConwayEngine * instance = ConwayEngine::getInstance();
	instance->Clear();
}

void MyFrame::OnClose(wxCloseEvent& event) {
	exit(0);
}

void MyFrame::OnPause(wxCommandEvent& event) {
	ConwayEngine * instance = ConwayEngine::getInstance();
	instance->Run(false);
}

int MyFrame::GetUpdateSpeed() {
	return SpeedSlider->GetValue();
}