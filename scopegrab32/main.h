#ifndef __VwX_MyFrame_H__
#define __VwX_MyFrame_H__

// ---- Win32 includes ----

#ifdef __WIN32__
   #include <windows.h>
#else
   // TODO: add linux header includes
   #include "linuxport_typedefs.h"
#endif

// ---- wxWidgets includes ----

#include <wx/wxprec.h>

#ifdef __WIN32__
#include <wx/msw/winundef.h>
#endif
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
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
#include <wx/event.h>
#include <wx/thread.h>
#include <wx/tokenzr.h>
#include <wx/datetime.h>
#include <wx/config.h>
#include <wx/html/helpctrl.h>
#ifdef __WIN32__
#include <wx/msw/helpchm.h>
#endif

// ---- other headers ----

#include "CSerial.h"

// ---- GUI control IDs -----

#define ID_MENU_FILE            1
#define ID_MENU_HELP            2
#define ID_MENU_SETTINGS        3
#define ID_MENU_CHECK1          10
#define ID_MENU_EXIT            11
#define ID_MENU_ABOUT           12
#define ID_MENU_USRSGUIDE       13
#define ID_COMBO_BAUD           20
#define ID_COMBO_COM            21
#define ID_SCREENSHOT           30
#define ID_BTN_GETSCREENSHOT    40
#define ID_BTN_SAVEIMAGE        41
#define ID_BTN_CLIPBOARDIMAGE   42
#define ID_BTN_SAVEPOSTSCRIPT   43
#define ID_BTN_SEND             44
#define ID_BTN_RECONNECT        45
#define ID_BTN_WAVE             46
#define ID_TXTCTL_CONSOLECMD    60
#define ID_TASKTIMER            100
#define ID_RTSTIMER             101

#define WINDOW_WIDTH            655
#define WINDOW_HEIGHT           665

#define FLUKESCREEN_WIDTH       480
#define FLUKESCREEN_HEIGHT      480

#define EPSONSCREEN_SCALEFACTOR 2
#define EPSONSCREEN_WIDTH       240*EPSONSCREEN_SCALEFACTOR
#define EPSONSCREEN_HEIGHT      240*EPSONSCREEN_SCALEFACTOR
#define IMAGE_BITDEPTH          1

#define POSTTASK_NONE           0
#define POSTTASK_INITCONFIGS    1
#define POSTTASK_RECONNECT      2


// ---- main prog defs and vars ----

#define TIMER_TOGGLERATE 20      // 20ms intervals for RTS toggling
#define MAX_COMPORT_COUNT 32     // allow max 32 serial ports in the combo box
extern int combo_portIDs[MAX_COMPORT_COUNT]; // ID's of available ports
extern int fluke_baudrates[];

// model series of detected scopemeter
#define SCOPEMETER_NONE         0  // none
#define SCOPEMETER_120_SERIES 120  // 123,124
#define SCOPEMETER_190_SERIES 190  // 192,196,199
#define SCOPEMETER_90_SERIES   90  // 91,92,96
#define SCOPEMETER_97_SERIES   97  // 97,99
#define COMBISCOPE_PM33_SERIES 33  // 3370B,3380B,3390B,3384B,3394B

// format of the raw data the Scopemeter series returns (screenshot, waveforms)
#define GFXFORMAT_NONE        0
#define GFXFORMAT_POSTSCRIPT  1
#define GFXFORMAT_EPSONESC    2
#define GFXFORMAT_ADCSAMPLES  3
#define GFXFORMAT_PNG         4

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
      long style = wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxSYSTEM_MENU|wxWANTS_CHARS,
      const wxString& name = wxT("frame"));
   ~MyFrame();
   void VwXinit();

   void        OnCommEvent(const BYTE* rxbuf, const DWORD rxbytes);
   void        DoEvents(void);

   wxString    GetFlukeResponse(DWORD msTimeout);
   wxString    QueryFluke(wxString cmdString, bool bAsciiMode, DWORD msTimeout, bool* ResponseIsOK);

private:

   // -- GUI safety funcs
   void        GUI_Down();
   void        GUI_Up();
   void        GUI_State(bool on);

   // -- internal vars init
   void        initBefore();
   void        ResetModeldependendGUI();

   // -- event handlers
   // menu
   void        OnMenuExit(wxMenuEvent& event);
   void        OnMenuAbout(wxMenuEvent& event);
   void        OnMenuGuide(wxMenuEvent& event);
   // serial port selection
   void        evtChangeComPort(wxCommandEvent& event);
   void        evtChangeBaudrate(wxCommandEvent& event);
   void        evtReconnect(wxCommandEvent& event);
   void        ChangeComPort(bool bNewPortSelected);
   // button clicks
   void        evtSendCommand(wxCommandEvent& event);
   void        evtGetScreenshot(wxCommandEvent& event);
   void        evtClipboardImage(wxCommandEvent& event);
   void        evtSaveImage(wxCommandEvent& event);
   void        evtSavePostscript(wxCommandEvent& event);
   void        evtGetWaveform(wxCommandEvent& event);
   // timer events
   void        evtRtsTimer(wxTimerEvent& event);  // toggles the RTS pin to generate negative rail
   void        evtTaskTimer(wxTimerEvent& event); // executes tasks after window creation
   // key press
   void        evtKeyDown(wxKeyEvent& event);

   // -- GUI components
   wxMenuBar   *m_menuBar;
   wxMenu      *m_menuFile;
   wxMenu      *m_menuSettings;
   wxMenu      *m_menuHelp;
   wxStatusBar *statusBar;
   wxMenuItem  *m_menuNoBaudWarn;

   wxComboBox  *comboCOM;
   wxComboBox  *comboBaud;

   wxNotebook  *nbNote;;
   wxPanel     *pnlCapture;
   wxPanel     *pnlConsole;
   wxPanel     *pnlTools;

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

   wxStaticText   *st_4;
   wxComboBox     *comboWaveforms;
   wxButton       *btnGetWaveform;
   wxTextCtrl     *txtWavestring;

   wxStaticBitmap *sbmpScreenshot;    // image painted to the display
   wxImage        *imgScreenshot;     // actual image content
   bool           bGotScreenshot;     // TRUE when image contains a screenshot
   volatile bool  bEscKey;            // TRUE after user presses 'ESC'
   wxString       strPostscript;      // postscript version of the image
   wxString       strPrevSavePath;    // previous path used when saving file
   int            mScopemeterType;    // detected scopemeter, default: SCOPEMETER_NONE
   wxString       strScopemeterID;    // the ID string returned by the SM
   bool           bFlukeDetected;


   // -- serial communications
   CSerial        *mySerial;               // serial port class
   wxArrayString  ReceivedStrings;         // array of received ascii test lines
   wxString       CurrRxString;            // local rx buffer
   volatile bool  bRxReceiverActive;       // activity flag for timing out rx operations
   bool           bRxAsciiMode;            // how to handle incoming data
   bool           bRxBinarymodeAfterACK;   // receive ack in ASCII, further data in binary
   DWORD          RxErrorCounter;          // count for all rx errors
   DWORD          RxTotalBytecount;        // for debug or statistics
   wxTimer*       tmrToggleRTS;            // timer for generating optical cable circuit voltage supplies
   wxCriticalSection csRxBuf;

   // -- program config file
   wxConfig*      mConfigs;                // for storing/loading user selections

   wxTimer*       tmrPostTasks;            // for scheduling tasks after window startup
   int            posttask;                // type of -"- task

   #ifdef __WIN32__
   wxCHMHelpController *m_helpFile;        // program help file, CHM in Windows
   #else
   wxHtmlHelpController *m_helpFile;       // program help file, hhp in Linux
   #endif

   // -- event table
   // see start of main.cpp for the event mappings
   DECLARE_EVENT_TABLE()
   #define VwX_INIT_OBJECTS_MyFrame

};


#endif
