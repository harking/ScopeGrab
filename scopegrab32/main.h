#ifndef __VwX_MyFrame_H__
#define __VwX_MyFrame_H__

// ---- Win32 includes ----

#ifdef __WIN32__
  #include <windows.h>
#else
  // TODO: add linux header includes
#endif

// ---- wxWidgets includes ----

#include <wx/wxprec.h>

#include <wx/msw/winundef.h>
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/clipbrd.h>

// ---- other headers ----

#include "CSerial.h"

// ---- GUI control IDs -----

#define ID_MENU_2               1
#define ID_MENU_EXIT            2
#define ID_MENU_ABOUT           3
#define ID_COMBO_BAUD           4
#define ID_COMBO_COM            5
#define ID_SCREENSHOT           6
#define ID_BTN_GETSCREENSHOT    7
#define ID_BTN_SAVEIMAGE        8
#define ID_BTN_CLIPBOARDIMAGE   9
#define ID_BTN_SAVEPOSTSCRIPT   10
#define ID_BTN_SEND             11
#define ID_BTN_RECONNECT        12
#define ID_RTSTIMER             13
#define ID_TXTCTL_CONSOLECMD    14

#define WINDOW_WIDTH            655
#define WINDOW_HEIGHT           665

#define FLUKESCREEN_WIDTH       480 // 320
#define FLUKESCREEN_HEIGHT      480 // 240
#define IMAGE_BITDEPTH          1

// ---- main prog defs and vars ----

#define TIMER_TOGGLERATE 20      // 20ms intervals for RTS toggling
#define MAX_COMPORT_COUNT 32     // allow max 32 serial ports in the combo box
extern int combo_portIDs[MAX_COMPORT_COUNT]; // ID's of available ports
extern int fluke_baudrates[];

#define SCOPEMETER_NONE         0  // none
#define SCOPEMETER_120_SERIES 120  // 123,124
#define SCOPEMETER_190_SERIES 190  // 192,196,199
#define SCOPEMETER_90_SERIES   90  // 91,92,96
#define SCOPEMETER_97_SERIES   97  // 97,99

// ---- wxWidgets application class ----

class MyApp : public wxApp {
public:
	// override base class virtuals
	// ----------------------------
	// this one is called on application startup and is a good place for the app
	// initialization (doing it here and not in the ctor allows to have an error
	// return: if OnInit() returns false, the application terminates)
	bool OnInit();
	MyFrame* frameMain;
};


// ---- wxWidgets main GUI window ----

class MyFrame : public wxFrame
{
public:

    MyFrame(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxSYSTEM_MENU,
        const wxString& name = wxT("frame"));
    ~MyFrame();
    void VwXinit();

    void        OnCommEvent(const BYTE* rxbuf, const DWORD rxbytes);
    void        DoEvents(void);

    wxString    GetFlukeResponse(DWORD msTimeout);
    wxString    QueryFluke(wxString cmdString, BOOL asciiMode, DWORD msTimeout, BOOL* ResponseIsOK);

private:

    // -- GUI safety funcs
    void        GUI_Down();
    void        GUI_Up();

    // -- internal vars init
    void        initBefore();
 
    // -- event handlers
    // menu
    void        OnMenuExit(wxMenuEvent& event);
    void        OnMenuAbout(wxMenuEvent& event);
    // serial port selection
    void        evtChangeComPort(wxCommandEvent& event);
    void        evtReconnect(wxCommandEvent& event);
    void        ChangeComPort();
    // button clicks
    void        evtSendCommand(wxCommandEvent& event);
    void        evtGetScreenshot(wxCommandEvent& event);
    void        evtClipboardImage(wxCommandEvent& event);
    void        evtSaveImage(wxCommandEvent& event);
    void        evtSavePostscript(wxCommandEvent& event);
    // timer event, toggles the RTS pin to generate negative rail
    void        evtRtsTimer(wxTimerEvent& event);

    // -- GUI components
    wxMenuBar   *m_menuBar;
    wxMenu      *m_menuFile;
    wxMenu      *m_menuHelp;
    wxStatusBar *statusBar;
 
    wxComboBox  *comboCOM;
    wxComboBox  *comboBaud;
 
    wxNotebook  *nbNote;;
    wxPanel     *pn_1;
    wxPanel     *pn_2;
    wxPanel     *pn_3;

    wxTextCtrl  *txtSerialTrace;
    wxTextCtrl  *txtCommandToSend;
    wxButton    *btnSendCommand;

    wxStaticText *st_1;
    wxStaticText *st_2;
    wxStaticText *st_3;
    wxStaticText *stFlukeID;

    wxButton       *btnGetScreenshot;
    wxButton       *btnCopyScreenshot;
    wxButton       *btnSaveScreenshot;
    wxButton       *btnSavePostscript;
    wxButton       *btnReconnect;

    wxStaticBitmap *sbmpScreenshot;    // image painted to the display
    wxImage        *imgScreenshot;     // actual image content
    wxString       strPostscript;      // postscript version of the image
    wxString       strPrevSavePath;    // previous path used when saving file
    int            mScopemeterType;    // detected scopemeter, default: SCOPEMETER_NONE
    BOOL           bFlukeDetected;

    
    // -- serial communications
    CSerial        *mySerial;
    wxArrayString  ReceivedStrings;
    wxString       CurrRxString;
    volatile BOOL  bRxReceiverActive;
    BOOL           bRxAsciiMode;
    DWORD          RxErrorCounter;
    wxTimer*       tmrToggleRTS;

    // -- event table
    // see start of main.cpp for the event mappings
    DECLARE_EVENT_TABLE()
    #define VwX_INIT_OBJECTS_MyFrame

};


#endif

