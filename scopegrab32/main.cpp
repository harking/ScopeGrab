/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

// ************************************************************************
// ***                                                                  ***
// ***  ScopeGrab32 - A tool for the Fluke ScopeMeter series            ***
// ***  (C) 2004 Jan Wagner                                             ***
// ***                                                                  ***
// ***  Version: 2.2.5 alpha (Windows and Linux/Unix support)           ***
// ***                                                                  ***
// ***  Licence: GNU GPL                                                ***
// ***                                                                  ***
// ***  THANKS                                                          ***
// ***  a big thanks for their very helpful feedback, infos, and        ***
// ***  generous testing and creation of serial comm traces go to:      ***
// ***    Jens F.,  Ethan W.,  Stuart P.,  Wayne L.,  Kristijan B.,     ***
// ***    Leif K.                                                       ***
// ***                                                                  ***
// ***  PROGRAM DESCRIPTION                                             ***
// ***  This program was inspipired by Steven J. Merrifields similarly  ***
// ***  named ScopeGrab "Fluke ScopeMeter Screen Capture" tool          ***
// ***  for unix/linux. You can find the source for his program at      ***
// ***    http://www.labyrinth.net.au/~steve/fluke/scopegrab.html       ***
// ***                                                                  ***
// ***  ScopeGrab32 runs under Windows 95/98/.../FX.                    ***
// ***                                                                  ***
// ***  The program requires an optical cable for the Fluke             ***
// ***  ScopeMeter. Fluke is selling a good quality cable kit, but      ***
// ***  it has its (high) price, too. It is possible to build your      ***
// ***  own cable. For schematics and infos, see e.g. my page at        ***
// ***    http://www.hut.fi/~jwagner/fluke/                             ***
// ***                                                                  ***
// ***  For now, the program supports monochrome screenshots from both  ***
// ***  190 and 190C series. Color screenshots of the 190C are PNG      ***
// ***  format, which is not yet handled by this program (hey, now you  ***
// ***  have the source code, add it yourself!:)                        ***
// ***                                                                  ***
// ***  The current 2.1.0 alpha version should have working support     ***
// ***  for ScopeMeters 123,124, 91,92,96 and 97,99.                    ***
// ***                                                                  ***
// ***                                                                  ***
// ***  USING THE PROGRAM                                               ***
// ***  Plug in the serial port cable into the PC serial port and       ***
// ***  the Fluke optical port, switch on the Fluke, and start          ***
// ***  ScopeGrab32. It will automatically check COM1 for the           ***
// ***  presence of a ScopeMeter. You may also select a different       ***
// ***  serial port or baud rate (1200 baud is the default for Fluke).  ***
// ***                                                                  ***
// ***  When a ScopeMeter is detected, the name is displayed below      ***
// ***  the serial port combo boxes, for example "FLUKE 196".           ***
// ***                                                                  ***
// ***  To make a screenshot of the current screen visible on your      ***
// ***  Fluke, click the Get Screen button. The image builds up while   ***
// ***  it is being downloaded.                                         ***
// ***                                                                  ***
// ***  Once downloaded, "To Clipboard" button can be clicked to copy   ***
// ***  the picture to the clipboard. After this you can for example    ***
// ***  paste it into your project report document, etc.                ***
// ***                                                                  ***
// ***  "Save Image" allows saving the picture to a Windows Bitmap      ***
// ***  *.bmp file.                                                     ***
// ***                                                                  ***
// ***  I also added a "Save Postscript" button, though I don't really  ***
// ***  know why! ;-) This will save the original postscript data       ***
// ***  that the ScopeMeter transmitted. However, at least with my      ***
// ***  Fluke 196 and firmware 4.08, the postscript is faulty and the   ***
// ***  left side of the image is not visible. The image may be garbled ***
// ***  a bit, too. Maybe in Fluke 190C series this is already fixed(?) ***
// ***                                                                  ***
// ***                                                                  ***
// ***  COMPILING                                                       ***
// ***  The program compiles only for Windows systems, as it calls      ***
// ***  the Win32 API serial port functions. Porting to linux should    ***
// ***  work, if you rewrite the CSerial class.                         ***
// ***                                                                  ***
// ***  This source was developed and compiled in WinXP using:          ***
// ***                                                                  ***
// ***   a) Dev-C++ 5.0 beta 9                                          ***
// **       http://www.bloodshed.net/dev/devcpp.html                    ***
// ***                                                                  ***
// ***   b) wxWindows (wxWidgets) 2.4.2, DevPak for Dev-C++             ***
// ***        http://michel.weinachter.free.fr/                         ***
// ***        or www.google.com "wxwindows 2.4.2 devpak"                ***
// ***      (note: a newer wxWindows DevPak may already be included     ***
// ***       with the DevC++ setup program)                             ***
// ***                                                                  ***
// ***   The source may compile also with a newer wxWindows DevPak.     ***
// ***   If not, use the DevC++ Tools=>Packet Manager to remove the     ***
// ***   new DevPak and then install the 2.4.2 one.                     ***
// ***                                                                  ***
// ***  The Linux and Unix release:                                     ***
// ***                                                                  ***
// ***   Use wxGTK (the GTK+ version of wxWidgets) for which you        ***
// ***   need to download the sources from http://www.wxwidgets.org     ***
// ***   Unpack wxGTK and configure with ./configure --disable-shared   ***
// ***   To install, just 'make' and 'make install'.                    ***
// ***   After wxGTK is installed, compile ScopeGrab32 in its source    ***
// ***   code folder by running 'make'.                                 ***
// ***                                                                  ***
// ***   This ScopeGrab32 code has only been tested with 2.4.2 and      ***
// ***   2.6.2 of wxGTK. wxX11 and wxUniv is known to not work          ***
// ***   correctly.                                                     ***
// ***                                                                  ***
// ***                                                                  ***
// ***   Editor settings: spaces for tabs, tabsize 3                    ***
// ***                                                                  ***
// ***                                                                  ***
// ************************************************************************

#include "main.h"
#include "Waveforms.h"
#include "ScopeGrab32_private.h"

// instantiate the wxWidgets application MyAp (similar to WinMain())
IMPLEMENT_APP(MyApp)



// ----------------------------------------------------
// -------------      GUI EVENT MAP       -------------
// ----------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame,wxFrame)

    EVT_MENU(ID_MENU_ABOUT,MyFrame::OnMenuAbout)
    EVT_MENU(ID_MENU_USRSGUIDE,MyFrame::OnMenuGuide)
    EVT_MENU(ID_MENU_EXIT,MyFrame::OnMenuExit)

    EVT_COMBOBOX(ID_COMBO_COM,MyFrame::evtChangeComPort)
    EVT_COMBOBOX(ID_COMBO_BAUD,MyFrame::evtChangeComPort)
  
    EVT_BUTTON(ID_BTN_SEND, MyFrame::evtSendCommand)
    EVT_BUTTON(ID_BTN_RECONNECT, MyFrame::evtReconnect)
    EVT_TEXT_ENTER(ID_TXTCTL_CONSOLECMD, MyFrame::evtSendCommand)
    EVT_BUTTON(ID_BTN_GETSCREENSHOT, MyFrame::evtGetScreenshot)
    EVT_BUTTON(ID_BTN_CLIPBOARDIMAGE, MyFrame::evtClipboardImage)
    EVT_BUTTON(ID_BTN_SAVEIMAGE, MyFrame::evtSaveImage)
    EVT_BUTTON(ID_BTN_SAVEPOSTSCRIPT, MyFrame::evtSavePostscript)
    EVT_BUTTON(ID_BTN_WAVE, MyFrame::evtGetWaveform)
    
    EVT_TIMER(ID_RTSTIMER, MyFrame::evtRtsTimer)
    EVT_TIMER(ID_TASKTIMER, MyFrame::evtTaskTimer)    
    
    EVT_CHAR(MyFrame::evtKeyDown)
    
END_EVENT_TABLE()



// ----------------------------------------------------
// -------------        CONSTANTS         -------------
// ----------------------------------------------------

// list of baud rates that are supported by Fluke ScopeMeters
int fluke_baudrates[] = { 19200, 9600, 4800, 1200 };

// array for storing serial port ID's
int combo_portIDs[MAX_COMPORT_COUNT];

// waveform querying
wxString fluke90wfQueries[FLUKE90_WF_QUERY_NUM] = {
        "QW 101", "QW 102", "QW 103", "QW 104", "QW 105", "QW 106",
        "QW 107", "QW 108", "QW 109", "QW 110", "QW 111"
    };
wxString fluke90wfNames[FLUKE90_WF_QUERY_NUM] = {
        "Channel A", "Channel B", "Channel maths", "Temp 1", "Temp 2",
        "Temp 3", "Memory 4", "Memory 5", "Memory 6", "Memory 7",
        "Memory 8"
    };
wxString fluke190wfQueries[FLUKE190_WF_QUERY_NUM] = {
        "QW 10", "QW 11", "QW 12", "QW 13", "QW 20", "QW 21", "QW 22", 
        "QW 23", "QW 30"        
    };
wxString fluke190wfNames[FLUKE190_WF_QUERY_NUM] = {
        "Channel A", "Trendplot 1", 
        "Channel A env (190C)", "Channel A ref (190C)",
        "Channel B", "Trendplot 2",
        "Channel B env (190C)", "Channel B ref (190C)",
        "Channel maths"        
    };
wxString fluke190wfUnits[FLUKE190_WF_UNITSCOUNT] = {
        " ", "V", "A", "Ohm", "W", "F", "K", "s",
        "h", "d", "Hz", "deg", "degC", "degF", "%", "dBm 50 Ohm",
        "dBm 600 Ohm", "dB V", "dB A", "dB W", "VAR", "VA"
    };


// ----------------------------------------------------
// -------------     HELPER FUNCTIONS     -------------
// ----------------------------------------------------

void MyFrame::DoEvents() {
   #ifdef __WIN32__
   MSG msg;
   // allow win32 messages through
   while ( TRUE==::PeekMessage((LPMSG)&msg, NULL, 0, 0, PM_REMOVE) ) {
      ::TranslateMessage(&msg); ::DispatchMessage(&msg);
   }
   #else
   // linux Yield() ?
   wxYield();
   #endif
}

// ----------------------------------------------------
// ---------   PROGRAM ENTRY POINT AND INIT   ---------
// ----------------------------------------------------

bool MyApp::OnInit()
{
    // create the main application window, resized in the constructor
	frameMain = new MyFrame(NULL, -1, wxT("ScopeGrab32 for the Fluke ScopeMeter series"));
	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frameMain->Center(wxBOTH);
	wxIcon progIcon = wxIcon("A", wxBITMAP_TYPE_ICO_RESOURCE);
	frameMain->SetIcon(progIcon);
	frameMain->Show(true);
	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned 'false' here, the
	// application would exit immediately.
	return true;
}


//
// Main application window/frame constructor
//
MyFrame::MyFrame(wxWindow* parent,wxWindowID id,const wxString& title,const wxPoint& pos,const wxSize& size,long style,const wxString& name)
VwX_INIT_OBJECTS_MyFrame
 {
   // init window
   Create(parent,id,title,pos,size,style,name);
   if( (pos==wxDefaultPosition) && (size==wxDefaultSize) ){
      SetSize(1,1,WINDOW_WIDTH,WINDOW_HEIGHT);
   }
   if( (pos!=wxDefaultPosition) && (size==wxDefaultSize) ){
      SetSize(WINDOW_WIDTH,WINDOW_HEIGHT);
   }
   this->SetThemeEnabled(false);

   // init window elements
   initBefore();
   VwXinit();
   
   return;
}

//
// Main window destructor
//
MyFrame::~MyFrame()
{
  return;
}


//
// Program internal vars init
//
void MyFrame::initBefore()
{
    CurrRxString = "";
    strPrevSavePath = "";
    strPostscript = "";
    ReceivedStrings.Empty();
    bRxAsciiMode = true;
    bRxBinarymodeAfterACK = false;
    bRxReceiverActive = false;
    bFlukeDetected = false;
    bGotScreenshot = false;
    bEscKey = false;
    RxErrorCounter = 0;
    RxTotalBytecount = 0;
    mScopemeterType = SCOPEMETER_NONE;
    strScopemeterID = "";
    mySerial = NULL;
    mConfigs = NULL;
}





// ----------------------------------------------------
// -------------   MAIN WINDOW GUI INIT   -------------
// ----------------------------------------------------

void MyFrame::VwXinit()
{
    // create a serial RX/TX class and mutex, and RTS toggler
    mySerial = new CSerial(this);
    tmrToggleRTS = new wxTimer(this, ID_RTSTIMER);
    
    // -- add GUI components

    Show(false);
    SetBackgroundColour(wxSystemSettings::GetColour((wxSystemColour)wxSYS_COLOUR_3DLIGHT));

    wxFont fntSmall = wxFont(8,wxDEFAULT,wxNORMAL,wxNORMAL,false,CONSOLE_FONT,wxFONTENCODING_SYSTEM);
    
    // -- menu and status bar
    
    statusBar=new wxStatusBar(this,-1,0,"statusBar");
    int statusWidths[] = { -1, -3 };
        statusBar->SetFieldsCount(2, statusWidths);
        statusBar->SetStatusText("  ",0);
        statusBar->SetStatusText("  ",1);        
    SetStatusBar(statusBar);

    wxMenuItem *menuitem;
    m_menuBar=new wxMenuBar();

    m_menuFile=new wxMenu(); // File -> Settings | Help | Exit
    
        m_menuSettings=new wxMenu();
        m_menuNoBaudWarn = new wxMenuItem(NULL,ID_MENU_CHECK1,wxT("Disable low baud warning"),
            wxT(""),wxITEM_CHECK, NULL);
        m_menuSettings->Append(m_menuNoBaudWarn);
        m_menuFile->Append(ID_MENU_SETTINGS,wxT("&Settings"),m_menuSettings);

        m_menuFile->AppendSeparator();
    
        m_menuHelp=new wxMenu();
        menuitem = new wxMenuItem(NULL,ID_MENU_USRSGUIDE,wxT("Users &guide"));
        m_menuHelp->Append(menuitem);        
        menuitem = new wxMenuItem(NULL,ID_MENU_ABOUT,wxT("&About"));
        m_menuHelp->Append(menuitem);
        m_menuFile->Append(ID_MENU_HELP,"&Help",m_menuHelp);

        m_menuFile->AppendSeparator();

        menuitem = new wxMenuItem(NULL,ID_MENU_EXIT,wxT("E&xit"));
        m_menuFile->Append(menuitem);
    
    m_menuBar->Append(m_menuFile,wxT("&File"));
    SetMenuBar(m_menuBar);
  
    // -- fluke serial comm
    st_1=new wxStaticText(this,-1,wxT(""),wxPoint(11,11+MNU_OFFSET),wxSize(60,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_1->SetLabel(wxT("Serial port setup:"));
    comboCOM=new wxComboBox(this,ID_COMBO_COM,wxT(""),
                  wxPoint(140,6),wxSize(CB_COM_WIDTH,21),
                  0,NULL,wxCB_READONLY);
    comboBaud=new wxComboBox(this,ID_COMBO_BAUD,wxT(""),
                  wxPoint(150+CB_COM_WIDTH,6),wxSize(CB_BAUD_WIDTH,21),
                  0,NULL,wxCB_READONLY);
    #ifdef __WIN32__
    comboCOM->SetToolTip("The serial port to which the optical cable is connected");
    comboBaud->SetToolTip("Highest baud rate to use which works reliably with the optical cable");
    #endif
    btnReconnect=new wxButton(this,ID_BTN_RECONNECT,wxT(""),
        wxPoint(150+CB_COM_WIDTH+CB_BAUD_WIDTH,6+MNU_OFFSET),wxSize(65,21));
        btnReconnect->SetLabel(wxT("Connect"));
        btnReconnect->SetTitle(wxT("Connect"));

    st_2=new wxStaticText(this,-1,wxT(""),wxPoint(11,35+MNU_OFFSET),wxSize(30,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_2->SetLabel(wxT("Fluke ScopeMeter:"));
    stFlukeID=new wxStaticText(this,-1,wxT(""),wxPoint(140,35+MNU_OFFSET),wxSize(130,19),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        stFlukeID->SetLabel(wxT("<not detected>"));

    nbNote=new wxNotebook(this,-1,wxPoint(6,60+MNU_OFFSET),
        wxSize((145+FLUKESCREEN_WIDTH),(45+FLUKESCREEN_HEIGHT)));
        nbNote->SetTitle(wxT("State"));

    // -- screen capture
    pnlCapture=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(20+FLUKESCREEN_HEIGHT)),wxNO_BORDER|wxCLIP_CHILDREN);
        nbNote->AddPage(pnlCapture,"Screen capture",false);
        pnlCapture->SetTitle(wxT("Screen capture"));
    imgScreenshot=new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
    imgScreenshot->SetMask(false);
    imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
    #ifdef __WIN32__
    sbmpScreenshot=new wxStaticBitmap(pnlCapture,ID_SCREENSHOT,
            wxBitmap(imgScreenshot,IMAGE_BITDEPTH),
            wxPoint(5,5),wxSize(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT),
            wxTHICK_FRAME,"sbmpScreenshot");
    #else
    sbmpScreenshot=new wxStaticPicture(pnlCapture,ID_SCREENSHOT,
            wxBitmap(),
            wxPoint(5,5),wxSize(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT),
            wxTHICK_FRAME,wxT("sbmpScreenshot"));
    sbmpScreenshot->SetBitmap(wxBitmap(imgScreenshot,IMAGE_BITDEPTH));
    #endif
    btnGetScreenshot=new wxButton(pnlCapture,ID_BTN_GETSCREENSHOT,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10),wxSize(90,25));
        btnGetScreenshot->SetLabel(wxT("Get Screen"));
        btnGetScreenshot->SetTitle(wxT("Get Screen"));
    btnCopyScreenshot=new wxButton(pnlCapture,ID_BTN_CLIPBOARDIMAGE,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+30),wxSize(90,25));
        btnCopyScreenshot->SetLabel(wxT("To Clipboard"));
        btnCopyScreenshot->SetTitle(wxT("To Clipboard"));
    btnSaveScreenshot=new wxButton(pnlCapture,ID_BTN_SAVEIMAGE,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+2*30),wxSize(90,25));
        btnSaveScreenshot->SetLabel(wxT("Save Image"));
        btnSaveScreenshot->SetTitle(wxT("Save Image"));
    btnSavePostscript=new wxButton(pnlCapture,ID_BTN_SAVEPOSTSCRIPT,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+3*30),wxSize(90,25));
        btnSavePostscript->SetLabel(wxT("Save Postscript"));
        btnSavePostscript->SetTitle(wxT("Save Postscript"));

    // -- miscellaneous tools 
    pnlTools=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(20+FLUKESCREEN_HEIGHT)),wxNO_BORDER);
        nbNote->AddPage(pnlTools,"Waves and tools",false);
        pnlTools->SetTitle(wxT("Waves and tools"));
    st_4=new wxStaticText(pnlTools,-1,wxT(""),wxPoint(11,11+4),wxSize(120,13),
        wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
    st_4->SetLabel(wxT("Download the waveform:"));
    comboWaveforms=new wxComboBox(pnlTools,-1,wxT(""),wxPoint(160,11),
        wxSize(120,22),0,NULL,wxCB_READONLY);
    btnGetWaveform=new wxButton(pnlTools,ID_BTN_WAVE,wxT(""),
        wxPoint(290,11),wxSize(70,21));
        btnGetWaveform->SetLabel(wxT("Download"));
        btnGetWaveform->SetTitle(wxT("Download")); 
    txtWavestring=new wxTextCtrl(pnlTools,-1,wxT(""),
        wxPoint(11,40),wxSize(FLUKESCREEN_WIDTH,22),wxTE_READONLY|wxTE_NOHIDESEL);
        txtWavestring->SetFont(fntSmall);
        txtWavestring->SetForegroundColour(*wxLIGHT_GREY);
        txtWavestring->SetValue("<Matlab vector of downloaded waveform data>");
            
    // -- serial trace/console
    pnlConsole=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(20+FLUKESCREEN_HEIGHT)),wxNO_BORDER);
        nbNote->AddPage(pnlConsole,"Serial console",false);
        pnlConsole->SetTitle(wxT("Serial console"));
    txtSerialTrace=new wxTextCtrl(pnlConsole,-1,wxT(""),
        wxPoint(1,1),wxSize(FLUKESCREEN_WIDTH+135,FLUKESCREEN_HEIGHT-30),
        wxTE_RICH|wxTE_MULTILINE);
        txtSerialTrace->SetFont(fntSmall);
    st_3=new wxStaticText(pnlConsole,-1,wxT(""),
        wxPoint(1,FLUKESCREEN_HEIGHT-10),wxSize(50,13),
        wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_3->SetLabel(wxT("Command:"));
    txtCommandToSend=new wxTextCtrl(pnlConsole,ID_TXTCTL_CONSOLECMD,wxT(""),
        wxPoint(65,FLUKESCREEN_HEIGHT-12),
        wxSize(320,18),wxTE_DONTWRAP|wxTE_PROCESS_ENTER);
    btnSendCommand=new wxButton(pnlConsole,ID_BTN_SEND,wxT(""),
        wxPoint(390,FLUKESCREEN_HEIGHT-12),wxSize(60,18));
        btnSendCommand->SetLabel(wxT("send"));
        btnSendCommand->SetTitle(wxT("Send"));

    // correct the font sizes for wxGTK
    #ifndef __WIN32__
    #ifdef __WXGTK12__ // GTK+ 1.2 or higher   
        wxFont fntNormal = wxFont(GENERAL_FONT_SIZE, wxDEFAULT, wxNORMAL, wxNORMAL, 
                    false, GENERAL_FONT, wxFONTENCODING_SYSTEM);
        #define TOUPDATE_CNT 22
        wxWindow* toUpdate[TOUPDATE_CNT] = { 
            // startup view
            (wxWindow*)st_1, (wxWindow*)st_2, (wxWindow*)stFlukeID,
            (wxWindow*)comboCOM, (wxWindow*)comboBaud, (wxWindow*)m_menuBar,
            (wxWindow*)btnReconnect,
            // panel headings
            (wxWindow*)nbNote,
            (wxWindow*)pnlCapture, (wxWindow*)pnlTools, (wxWindow*)pnlConsole,
            // screen capture panel
            (wxWindow*)btnGetScreenshot, (wxWindow*)btnCopyScreenshot,
            (wxWindow*)btnSaveScreenshot, (wxWindow*)btnSavePostscript,
            // tools panel (waveform, ...)
            (wxWindow*)st_4, (wxWindow*)comboWaveforms, (wxWindow*)btnGetWaveform,
            // serial console panel
            (wxWindow*)st_3, (wxWindow*)txtCommandToSend, (wxWindow*)btnSendCommand,
            // status bar
            (wxWindow*)statusBar
        };
        for(unsigned int i=0; i<TOUPDATE_CNT; ++i) {
            toUpdate[i]->SetFont(fntNormal);
        }
    #endif
    #endif

    // -- populate the serial ports and baud list

    #ifdef __WIN32__
    // (direct copy from MSDN example code)
    // serial ports first:
    DWORD dwNeeded = 0, dwReturned = 0;
    PPORT_INFO_2 pStart, pInfo = NULL;
    EnumPorts(NULL, 2, (LPBYTE) pInfo, 0, &dwNeeded, &dwReturned);
    pInfo = (PPORT_INFO_2) malloc(dwNeeded);
    EnumPorts(NULL, 2, (LPBYTE) pInfo, dwNeeded, &dwNeeded, &dwReturned);
    pStart = pInfo;
    wxString strRest;
    long portID;
    int portCnt = 0;
    for (DWORD j = 0; j < dwReturned && portCnt<MAX_COMPORT_COUNT; j++) {
        wxString wxtmp = wxString(pInfo->pPortName);
        if (wxtmp.StartsWith("COM", &strRest)) {
            strRest.ToLong(&portID, 10);
		    combo_portIDs[portCnt] = (int)portID;
            comboCOM->Append( wxtmp.Left(wxtmp.Length() - 1).c_str(), &combo_portIDs[portCnt]);
            portCnt++;
        }
        pInfo++;
    }
    if (pStart) {
        free(pStart);
    }
    #else
    // serial port listing for linux
    for (int i=0; i<8 && i<MAX_COMPORT_COUNT; ++i) {
        combo_portIDs[i] = i;
        comboCOM->Append(wxString::Format("/dev/ttyS%d",i).c_str(), &combo_portIDs[i]);
    }
    for (int i=8; i<16 && i<MAX_COMPORT_COUNT; ++i) {
        combo_portIDs[i] = i;
        comboCOM->Append(wxString::Format("/dev/ttyUSB%d",(i-8)).c_str(), &combo_portIDs[i]);
    }
    #endif

    // next, add ScopeMeter supported baud rates
    for(unsigned int i=0;i<(sizeof(fluke_baudrates)/sizeof(int));++i) {
        comboBaud->Append(
            wxString::Format("%d", fluke_baudrates[i]), &fluke_baudrates[i]
        );
    }

    // helpfile
    #ifdef __WIN32__
    m_helpFile = new wxCHMHelpController();
    m_helpFile->Initialize(".\\help\\scopegrab32.chm");
    #else
    m_helpFile = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);
    m_helpFile->AddBook(wxFileName("./help/scopegrab32.hhp"),false);
    #endif
    
    // schedule config loader after window is visible
    this->mConfigs = new wxConfig("ScopeGrab32");
    GUI_Down();
    posttask = POSTTASK_INITCONFIGS;
    tmrPostTasks = new wxTimer(this, ID_TASKTIMER);
    tmrPostTasks->Start(1000, true); // 1 second, single shot
        
    return;
}

//
// Task timer event
//
void MyFrame::evtTaskTimer(wxTimerEvent& event)
{
    signed long sel;
    switch(this->posttask) {
        case POSTTASK_NONE:
            break;
        // -- after window is visible, load user settings and try
        //    to connect to the scopemeter
        case POSTTASK_INITCONFIGS:
            if(this->mConfigs!=NULL) {
                mConfigs->SetPath(wxGetCwd());    
                mConfigs->SetRecordDefaults(true);
                // get and apply previous user settings now 
                // (Read() defaults: 0=COM1, 0=19200baud)
                sel = mConfigs->Read("ComPort",0L);
                if(sel<0 || sel>=comboCOM->GetCount()) { sel = 0; }
                comboCOM->SetSelection(sel);
                sel = mConfigs->Read("UseBaud",0L);
                if(sel<0 || sel>=comboBaud->GetCount()) { sel = 0; }
                comboBaud->SetSelection(sel);
                bool state;
                mConfigs->Read("DisableBaudWarn",&state,false);
                m_menuNoBaudWarn->Check(state);
                mConfigs->Flush();
            } else {
                // use defaults (0=COM1, 0=19200baud)
                comboCOM->SetSelection(0);
                comboBaud->SetSelection(0);        
            }
            this->ChangeComPort(); // apply
            GUI_Up();
            break;
        default:
            break;
    }
    return;
}



//
// Reset GUI entries (combo boxes etc) with ScopeMeter
// model dependend infos
//
void MyFrame::ResetModeldependendGUI()
{
    if ( false==bFlukeDetected ) return;

    // update the waveform download combo    
    comboWaveforms->Clear();
    switch(mScopemeterType) {
        case SCOPEMETER_190_SERIES:
        case SCOPEMETER_120_SERIES:
            for ( int i=0; i<FLUKE190_WF_QUERY_NUM; ++i ) {
                comboWaveforms->Append(fluke190wfNames[i], &fluke190wfQueries[i]);
            }
            comboWaveforms->SetSelection(0);
            break;
        case SCOPEMETER_97_SERIES:
        case SCOPEMETER_90_SERIES:
            for ( int i=0; i<FLUKE90_WF_QUERY_NUM; ++i ) {
                comboWaveforms->Append(fluke90wfNames[i], &fluke90wfQueries[i]);
            }
            comboWaveforms->SetSelection(0);            
            break;
        default:
            comboWaveforms->SetValue("not supported");    
            break;
    }
    
    // (add other processing here)
    
    return;
}


//
// Safe GUI handling, enable or disable the input controls
//
void MyFrame::GUI_State(bool on)
{
    comboBaud->Enable(on); comboCOM->Enable(on);
    m_menuBar->EnableTop(0,on);
    btnGetScreenshot->Enable(on && bFlukeDetected);
    btnSaveScreenshot->Enable(on && bFlukeDetected);
    btnCopyScreenshot->Enable(on && bFlukeDetected);
    btnSavePostscript->Enable(on && (SCOPEMETER_190_SERIES==mScopemeterType)); // only 190/190C does postscript
    btnReconnect->Enable(on);
    btnSendCommand->Enable(on);
    if(comboWaveforms->GetCount()>0 || !on) { 
        comboWaveforms->Enable(on);
        btnGetWaveform->Enable(on);   
    }
    return;
}

void MyFrame::GUI_Up()
{
    GUI_State(true);
    return;
}

void MyFrame::GUI_Down()
{
    GUI_State(false);
    return;
}

//
// Keypress: catch 'ESC' key presses (this actually works
//           only if the main frame is selected, not one
//           of the contained panels or textboxes... No 
//           workaround yet...)
//
void MyFrame::evtKeyDown(wxKeyEvent& event)
{
    if ( WXK_ESCAPE==event.GetKeyCode() ) { 
        if ( NULL!=this->txtSerialTrace ) {
            txtSerialTrace->AppendText("<user hit the ESC key>\r\n");
        }
        this->bEscKey = true; 
    }
    event.Skip();   
}


//
// Button click: try to connect to the Fluke again, with the current
//               serial port selections (port nr and baud rate)
//
void MyFrame::evtReconnect(wxCommandEvent& event)
{
    this->ChangeComPort();
    return;
}


//
// Combo box selection: user changed the serial baud or port
//
void MyFrame::evtChangeComPort(wxCommandEvent& event)
{
    #ifdef __WIN32__
    // call ChangeComPort() to apply the settings, but skip
    // duplicate events
    static bool firstCall = true;
    if ( true==firstCall ) { this->ChangeComPort(); }
    firstCall = !firstCall;
    #else
    // duplicates skipping not required in linux/X11
    this->ChangeComPort();
    #endif
}

//
// Apply new port settings
// - try to connect at the Fluke-default 1200 baud rate, then switch 
//   to the selected baud rate.
// - if no response at 1200 baud, try the selected baud rate
//

void MyFrame::ChangeComPort()
{
    bool ret;
    bool respOk;
    wxString response, str;
    int  idx, port, baud, user_baud, prev_baud;

    // starting baud rate (1200 or previous user baud rate)
    if(NULL==mySerial) {
        baud = 1200;
    } else {
        if(!mySerial->isOpen()) { baud = 1200; }
        else { baud = mySerial->getBaudrate(); }
    }
    prev_baud = baud;
    
    // reset current Fluke infos    
    bFlukeDetected = false;
    mScopemeterType = SCOPEMETER_NONE;

    // get the current/new port number
    idx = comboCOM->GetSelection();
    if ( NULL==comboCOM->GetClientData(idx) ) return;
    port = *(int*)comboCOM->GetClientData(idx);
    #ifndef __WIN32__
    wxString portstr = comboCOM->GetString(idx);
    #endif
    
    // also get the desired baud rate
    idx = comboBaud->GetSelection();
    if ( NULL==comboBaud->GetClientData(idx) ) return;
    user_baud = *(int*)comboBaud->GetClientData(idx);

    // show hourglass
    wxBusyCursor wait;
    GUI_Down();
    
    while(!bFlukeDetected) {
        
        // (re)open the port - fluke comm is always 8 databit, 1 stopbit, 
        // no parity, start with 1200 baud (or previous user baud)
        if(NULL==mySerial) { mySerial = new CSerial(this); }
        tmrToggleRTS->Stop();
        CurrRxString.Clear();
        ReceivedStrings.Clear();
        
        #ifdef __WIN32__
        ret = mySerial->openPort(port, baud, 8, ONESTOPBIT, 'N', 0, NULL);
        str = wxString::Format("COM%d @ %d baud",port, baud);
        statusBar->SetStatusText(str,0);
        #else
        ret = mySerial->openPort(0xFF, baud, 8, ONESTOPBIT, 'N', 0, portstr.c_str());
        str = wxString::Format(portstr+" @ %d",baud);
        statusBar->SetStatusText(str,0);
        #endif

        if(false==ret) {
            // port open failed
            statusBar->SetStatusText(wxString::Format("system error 0x%04lX",mySerial->getLastError()),1);
            txtSerialTrace->AppendText(wxString::Format(str + " open: system error 0x%04lX\r\n",mySerial->getLastError()));
            break;  
        } 

        txtSerialTrace->AppendText(wxString::Format("PC set to %d baud\r\n", baud));
        statusBar->SetStatusText("checking...",1);

        // init circuit supply voltage 'generator'
        tmrToggleRTS->Start(TIMER_TOGGLERATE, false); // given rate, not single-shot
        DoEvents();
        #ifdef __WIN32__
        SleepEx(400,TRUE);
        #else
        // linux task_sleep()
        usleep(400000L);
        #endif
        
        DoEvents();

        response = QueryFluke("  ",true,1000,&respOk); // send a few cr/lf's

        // try to get identification from the Fluke : <acknowledge><cr><infos><cr>
        response = QueryFluke("ID",true,1000,&respOk); // just the <ack>
        if(true==respOk) {
            response = GetFlukeResponse(250); // <infos> string
        }
        
        wxString infoStr = "";
        if(response.Freq(';')>1) {
            bFlukeDetected = true;
            strScopemeterID = response;
            // show ID in text label
            stFlukeID->SetLabel(response.BeforeFirst(';'));
            // ... and in the status bar
            infoStr = "ident=" + response;
            // next, try to extract the scopemeter series info
            response = response.MakeUpper();
            response = response.BeforeFirst(';');
            if(1!=response.Contains("SCOPE") && 1!=response.Contains("FLUKE")) {
                // no Fluke ScopeMeter
                mScopemeterType = SCOPEMETER_NONE;
                bFlukeDetected = false;
                infoStr = "(unsupported) " + infoStr;
                // different ScopeMeter series:
            } else if(1==(response.Contains(" 192") || response.Contains(" 196") || response.Contains(" 199"))) {
                mScopemeterType = SCOPEMETER_190_SERIES;
            } else if(1==(response.Contains(" 91") || response.Contains(" 92") || response.Contains(" 96"))) {
                mScopemeterType = SCOPEMETER_90_SERIES;
            } else if(1==(response.Contains(" 97") || response.Contains(" 99"))) {
                mScopemeterType = SCOPEMETER_97_SERIES;
            } else if(1==(response.Contains(" 123") || response.Contains(" 124")
                    || response.Contains(" 105"))) {
                mScopemeterType = SCOPEMETER_120_SERIES;
            } else {
                // unsupported model
                mScopemeterType = SCOPEMETER_NONE;
                bFlukeDetected = false;
                infoStr = "(unsupported) " + infoStr;
            }
        } else {
            bFlukeDetected = false;
            // update text label and status bar
            stFlukeID->SetLabel("<not detected>");
            infoStr = "no ScopeMeter";
        }
        statusBar->SetStatusText(infoStr,1);

        // try to switch baud rate
        if(bFlukeDetected) {

            txtSerialTrace->AppendText("Got Fluke : " + infoStr + "\r\n");
            
            if(user_baud==baud) break; // connected at target baud, finished

            prev_baud = baud;
            baud = user_baud;
            txtSerialTrace->AppendText(wxString::Format("Setting Fluke baudrate to %d...\r\n",baud));

            wxString command = "";
            switch(mScopemeterType) {
                case SCOPEMETER_190_SERIES:
                case SCOPEMETER_120_SERIES:
                    command = wxString::Format("PC %d", baud);
                    break;
                case SCOPEMETER_90_SERIES:
                    command = wxString::Format("PC%d,N,8,1,XONXOFF", baud);
                break;
                case SCOPEMETER_97_SERIES:
                    command = wxString::Format("PC %d,N,8,1,XONXOFF", baud);
                break;
            }
            response = QueryFluke(command,true,1000,&respOk); // <ack><cr>
            if ( true==respOk ) {
                txtSerialTrace->AppendText(wxString::Format("Ok, setting PC baudrate to %d...\r\n",baud));
                tmrToggleRTS->Stop();
                if(!mySerial->setBaudrate(baud)) {
                    txtSerialTrace->AppendText("PC baud rate set failed!!\r\n");
                }
                tmrToggleRTS->Start(TIMER_TOGGLERATE, false);
                #ifdef __WIN32__
                statusBar->SetStatusText(wxString::Format("COM%d @ %d baud",port, baud),0);
                #else
                statusBar->SetStatusText(wxString::Format(portstr+" @ %d",baud));
                #endif
                break;
            } else {
                txtSerialTrace->AppendText(wxString::Format("Fluke didn't ACK baud rate change. Staying at %d.\r\n",prev_baud));
                response = QueryFluke("  ",true,1000,&respOk); // send a few cr/lf's in case of errors
                break;
            }
            
        } else { // !bFlukeDetected
        
            if(user_baud==baud) break; // finished, no Fluke

            txtSerialTrace->AppendText("No Fluke found, trying user baud rate.\r\n");

            // try at user specified baud
            baud = user_baud;
            
        }

    }
            
    ResetModeldependendGUI();
    
    GUI_Up();
    return;
}


//
// Button click: send user query string to the ScopeMeter
//
void MyFrame::evtSendCommand(wxCommandEvent& event)
{
    
    if ( NULL==mySerial || false==mySerial->isOpen() ) {
        txtSerialTrace->AppendText("Error: port not opened, can't send the command.\r\n");
        txtCommandToSend->SetValue("");
        return;
    }
    
    // send the command and dump all response data (can be multiple lines)
    bool respOk;
    GUI_Down();

    wxString sendStr = txtCommandToSend->GetValue();
    wxString response = QueryFluke(sendStr,true,1000,&respOk); // <ack>
    while ( response.Length() > 0 ) {
        response = GetFlukeResponse(1000); // <possible command info data>
    }
    GUI_Up();
    return;
}


//
// Button click: Get a screenshot from the scopemeter
//
void MyFrame::evtGetScreenshot(wxCommandEvent& event)
{
    bool gotImage = false;
    bool imageStart = false;
    long rxbytesremaining = 0;
    unsigned char currbyte = 0;
    unsigned int y_counter = 0, x_counter = 0;
    unsigned int bitval = 0;
    signed int bitnr = 0;
    BYTE color = 0;
    unsigned int GraphicsFormat = GFXFORMAT_NONE;
    wxString oldSBstr;
    unsigned int idx;

    #define LINES_PER_UPDATE 20    // reduce flicker, only repaint after n new lines
    unsigned int skipcounter = 0;

    wxString command="", response = "", strOldBaud="";
    bool respOk=false;

    // any Fluke there?
    if ( false==bFlukeDetected ) {
        statusBar->SetStatusText("screenshot: no ScopeMeter detected",1);
        return;
    }

    // check that baudrate is set high
    if ( (mySerial->getBaudrate()<19200) && !m_menuNoBaudWarn->IsChecked() ) {
        idx = wxMessageBox("You have selected a baud rate lower than 19200.\r\n"
            "Downloading the screenshot may take a considerable amount of time!\r\n\r\n"
            "Choose Yes if you want to continue anyway.\r\n\r\n"
            "Note: you can disable this warning via the menu File->Settings.",
            "Warning: low baud rate - slow!",
            wxYES_NO | wxICON_EXCLAMATION, this);
        if ( idx!=wxYES ) return;
    }    

    GUI_Down();
    
    wxBusyCursor wait; // displays hourglass, reverts back to normal automatically
        
    // -- request screenshot

    bGotScreenshot = false;
    RxTotalBytecount = 0;
    
    switch(mScopemeterType) {
        case SCOPEMETER_190_SERIES:
        case SCOPEMETER_120_SERIES:
            // Query Print screen 0 and postscript (3) format
            command = "QP 0,3";
            GraphicsFormat = GFXFORMAT_POSTSCRIPT;
            statusBar->SetStatusText("screenshot: requesting postscript image",1);
            break;
        case SCOPEMETER_90_SERIES:
            // Query Print live screen and Epson ESC sequence format
            command = "QP"; // ???correct??
            GraphicsFormat = GFXFORMAT_EPSONESC;
            statusBar->SetStatusText("screenshot: requesting screenshot",1);
            break;
        case SCOPEMETER_97_SERIES:
            // Query Graphics live screen and Epson ESC sequence format
            command = "QP"; // QP seems to work too (was "QG129")
            GraphicsFormat = GFXFORMAT_EPSONESC;
            statusBar->SetStatusText("screenshot: requesting screen graphics",1);
            break;
    }

    // -- wait for graphics data to arrive
    this->bEscKey = false;
    if ( GFXFORMAT_POSTSCRIPT==GraphicsFormat ) {
        //
        // Receive postscript data in ASCII mode
        //
        oldSBstr = statusBar->GetStatusText(0);
        this->strPostscript = "";
        response = QueryFluke(command,true,1000,&respOk); // first just the <ack>
        while ( (response.Length()>0) && (true==respOk) ) {

            // get next line
            response = GetFlukeResponse(1000); // <printer or image data><cr>
            if(response.Length()<=0) break;
            
            // interrupt if user wants
            if ( true==this->bEscKey ) {
                statusBar->SetStatusText("screenshot: user cancelled (ESC)",1);                
                wxMessageBox("Postscript download cancelled.\r\nPlease switch off your "
                             "ScopeMeter first, so it stops transmitting data.\r\nThen switch on and reconnect at 1200 baud.",
                             "User cancelled", wxOK | wxICON_INFORMATION, this);
                gotImage = false;
                this->bEscKey = false;
                break;    
            }
            
            // store to postscript temp var
            this->strPostscript.Append(response);
            this->strPostscript.Append("\r\n");
        
            // scan up to the
            //   "... { currentfile 60 string readstring pop } image" line
            // in the response - the image data starts on the next line
            if ( false==imageStart ) {
                if ( response.Find("image") >= 0 ) {
                    imageStart = true;
                    if(this->imgScreenshot!=NULL) { delete imgScreenshot; }
                    this->imgScreenshot = new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                    this->imgScreenshot->Create(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                    this->imgScreenshot->SetMask(false);
                    this->imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
                    statusBar->SetStatusText("screenshot: receiving image...",1);
                }
                continue;
            }

            statusBar->SetStatusText(wxString::Format("%ld bytes",(long)strPostscript.Length()),0);

            // a line with "showpage" indicates the end of the image data
            if ( response.Find("showpage") >= 0 ) {
                gotImage = true;
                break;
            }

            // process the current line, all upper case HEX data strings
            response = response.MakeUpper();

            for ( idx=0; idx<response.Length(); ++idx ) {
                // check hex format
                currbyte = response.GetChar(idx);
                if ( !(currbyte>='0' && currbyte<='9') &&
                     !(currbyte>='A' && currbyte<='F') )      
                { continue; }
                // hex to dec
                if(currbyte>='0' && currbyte<='9') { currbyte -= '0'; }
                else { currbyte = 10+(currbyte-'A'); }
                // one character (0..F) marks 4 pixels (bits), with MSb on the left
                bitval=0x08;
                for ( bitnr=3; bitnr>=0; --bitnr, bitval/=2 ) {
                    color = 0xFF;
                    if ( 0==(currbyte&bitval) ) { color = 0x00; } // bit cleared: black
                    this->imgScreenshot->SetRGB(x_counter, y_counter, color, color, color);
                    ++x_counter;
                }
            }

            // update image and advance to next line
            // parse the hex data in the current line
            if ( x_counter>=FLUKESCREEN_WIDTH ) {
                x_counter = 0;
                ++y_counter;
            }

            // refresh the bitmap display after each N new lines
            if ( 0==((++skipcounter) % LINES_PER_UPDATE) ) {
               wxBitmap tempBitmap(this->imgScreenshot, IMAGE_BITDEPTH);
               sbmpScreenshot->SetBitmap(tempBitmap);
               #ifndef __WIN32__
               sbmpScreenshot->Refresh(TRUE, NULL);
               #endif
            }
         
        }

        // done receiving postscript image
        if ( true==imageStart ) { bGotScreenshot = true; }
        statusBar->SetStatusText(oldSBstr,0);
        
    } else if ( GFXFORMAT_EPSONESC==GraphicsFormat ) {
        //
        // Receive Epson ESC code data in binary mode
        //
        // Before the binary graphics data starts, there's still the
        // ack code and then the count of bytes to receive
        // e.g. "0<cr>7454,<epson esc binary data><cr><more data><cr>...
        //
        rxbytesremaining = 0;
        bool dataStart = false;
        wxString newstr = "";
        const char STR_GFXSTART[] = { 0x1B, 0x2A, 0x04, '\0' };
        const int  LEN_STR_GRFXSTART = 3;
        unsigned int data_len = 0;
        unsigned int imageblock_retries = 0;
        
        response = QueryFluke(command,false,1000,&respOk); // false=>binary mode, <ack> still rx'ed in ASCII mode
        response = "";
        
        while ( (true==respOk) && (false==gotImage) ) {

            DoEvents();
            
            // get more data
            if ( false==dataStart || response.Length()<=256 || imageblock_retries>0 ) {
                txtSerialTrace->AppendText(
                   wxString::Format("Epson ESC: waiting for more binary data... (try %d)\r\n", imageblock_retries) );
                newstr = GetFlukeResponse(5000); // (scopemeter can take up to 5 secs before starting response...)
            } else {
                newstr = "";
            }
            
            if ( newstr.Length()>0 ) { response.Append(newstr); }
            
            // check for timeouts or too many reattempts
            if ( (newstr.Length()<=0)  // timeout
                 && ( response.Length()<=0 || imageblock_retries>=3 ) // no other data
               ) 
            {
                gotImage = true; // finished receiving the screenshot                
                if ( (false==imageStart) || (false==dataStart) ) {
                    txtSerialTrace->AppendText("Epson ESC: Fluke didn't send data, timeout error.");
                    respOk = false;
                    gotImage = false;
                } else if ( rxbytesremaining>240 || imageblock_retries>=3 ) {
                    txtSerialTrace->AppendText("Timeout while waiting for more data. Image may be incomplete.\r\n");
                    gotImage = false;
                }
                break;
            }

            // -- interrupt now if user wants
            if ( true==this->bEscKey ) {
                statusBar->SetStatusText("screenshot: user cancelled (ESC)",1);                
                wxMessageBox("Epson ESC download cancelled.\r\nPlease switch off your "
                             "ScopeMeter first, so it stops transmitting data.\r\nThen switch on and reconnect at 1200 baud.",
                             "User cancelled", wxOK | wxICON_INFORMATION, this);
                gotImage = false;
                this->bEscKey = false;
                break;    
            }
            
            // -- start of data? read the byte count e.g. "7453,<data>"
            if ( false==dataStart ) {
                if ( response.Length()<5 ) continue;
                size_t commaidx = response.Index(',');
                if ( (size_t)wxNOT_FOUND!=commaidx ) {
                    long ltmp = 0;
                    wxString lenStr = response.Left(commaidx);
                    lenStr.Replace("\r", "", true);
                    lenStr.Replace("\n", "", true);
                    if ( true==lenStr.ToLong(&ltmp,10) ) {
                        rxbytesremaining = ltmp + 1; // fluke sends x+1 bytes...
                        txtSerialTrace->AppendText("Epson ESC byte count string: '"+lenStr+"'\r\n");
                        if(this->imgScreenshot!=NULL) { delete imgScreenshot; }
                        this->imgScreenshot = new wxImage(EPSONSCREEN_WIDTH,EPSONSCREEN_HEIGHT);
                        this->imgScreenshot->Create(EPSONSCREEN_WIDTH,EPSONSCREEN_HEIGHT);
                        this->imgScreenshot->SetMask(false);
                        this->imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
                        statusBar->SetStatusText("screenshot: receiving Epson ESC...",1);
                        dataStart = true; // actual data starts after the comma
                    } else {
                        statusBar->SetStatusText("screenshot: response format error",1);
                        txtSerialTrace->AppendText("Error, unexpected Epson ESC byte count string: '"+lenStr+"'\r\n");
                        respOk = false; // quit
                    }
                    // remove byte-count string from response
                    if( response.Length()>(commaidx+1) ) { //(wxString crashes if crop len <=0)
                        response = response.Right(response.Length()-(commaidx+1));
                    } else {
                        response = "";
                    }
                    //response.Shrink();
                }
                continue;
            }

            // -- interpret data ESC sequences
            imageblock_retries++;
            size_t escIndex = response.Index(STR_GFXSTART); // 0x1B 0x2A 0x04 start of one new gfx row
            if ( (size_t)wxNOT_FOUND==escIndex ) {
                if ( (response.Length()>256) && (false==imageStart) ) {
                   txtSerialTrace->AppendText("Epson ESC: no graphics in first 256 bytes of response, aborting rx\r\n");
                   respOk = false;
                } else {
                    continue;
                }
            } else { 
                txtSerialTrace->AppendText(wxString::Format("Epson ESC: got start sequence, line %d\r\n", y_counter));
                imageStart = true; 
            }

            if ( response.Length() <= (escIndex+1+LEN_STR_GRFXSTART+2) )
                { continue; } // wait until 2 data-length-bytes avaible

            data_len = (unsigned char)response.GetChar(escIndex+LEN_STR_GRFXSTART)
                + 256L*((unsigned char)response.GetChar(escIndex+LEN_STR_GRFXSTART+1));
            txtSerialTrace->AppendText(wxString::Format("Epson ESC: data len of line %d is %d bytes, %ld remaining, currently got %d\r\n", y_counter, data_len, rxbytesremaining, response.Length()));
                
            if ( response.Length() <= (escIndex+1+LEN_STR_GRFXSTART+2+data_len) )
                { continue; } // wait until full data available for one row

            // get the row data
            wxString dataStr = response.Mid(escIndex+1+LEN_STR_GRFXSTART+2 - 1, data_len);
            
            // truncate response buffer string
            response = response.Mid(escIndex+1+LEN_STR_GRFXSTART+2+data_len, wxSTRING_MAXLEN);
            //response.Shrink();
            rxbytesremaining -= escIndex+1; // stuff before data block
            rxbytesremaining -= LEN_STR_GRFXSTART+2+data_len; // head and data block
            if ( rxbytesremaining<0 ) {
                txtSerialTrace->AppendText("Warning: received more graphics bytes than expected...\r\n");
                rxbytesremaining = 0;
            }

            // parse the data into the wxwidgets bitmap
            for ( x_counter=0;
                  (x_counter<data_len) && (x_counter<=EPSONSCREEN_WIDTH);
                  ++x_counter )
            {
                currbyte = dataStr.GetChar(x_counter);
                bitval=1;
                for ( bitnr=0; bitnr<8; ++bitnr,bitval*=2 ) {
                    color = 0x00; 
                    if ( 0==(currbyte&bitval) )
                        { color = 0xFF; } // if bit cleared: white
                    // highest bit is current row, lowest bit lowest row
                    for ( unsigned int xtmp=0; xtmp<EPSONSCREEN_SCALEFACTOR; ++xtmp )
                    for ( unsigned int ytmp=0; ytmp<EPSONSCREEN_SCALEFACTOR; ++ytmp ) {
                        this->imgScreenshot->SetRGB(
                                 x_counter*EPSONSCREEN_SCALEFACTOR+xtmp,
                                 y_counter+EPSONSCREEN_SCALEFACTOR*(8-bitnr)-1-ytmp,
                                 color, color, color);
                    }
                }
            }
            y_counter += 8*EPSONSCREEN_SCALEFACTOR;
            if ( y_counter>EPSONSCREEN_HEIGHT ) {
                txtSerialTrace->AppendText(
                  wxString::Format("Warning: Epson ESC image height exceeded %d pixels.\r\n",EPSONSCREEN_WIDTH)
                );
                gotImage = true; // stop drawing if outside 260x260
            }
            imageblock_retries = 0; // reset, until next block
            
            // refresh the bitmap display after each N new lines
            if ( 0==((++skipcounter) % LINES_PER_UPDATE) ) {
               wxBitmap tempBitmap(imgScreenshot, IMAGE_BITDEPTH);
               sbmpScreenshot->SetBitmap(tempBitmap);
               #ifndef __WIN32__
               sbmpScreenshot->Refresh(TRUE, NULL);
               #endif
                //sbmpScreenshot->Refresh(false,
               //                    new wxRect(0,y_counter-8*EPSONSCREEN_SCALEFACTOR,
               //                    EPSONSCREEN_WIDTH*EPSONSCREEN_SCALEFACTOR,y_counter) );
            }

        }//while(rx)

        // done receiving epson esc sequence image
        if ( true==dataStart ) { bGotScreenshot = true; }
    }

    txtSerialTrace->AppendText(wxString::Format("debug: received %ld bytes in total.\r\n", RxTotalBytecount));

    // -- show the final result of the operation
    wxBitmap outBitmap(imgScreenshot, IMAGE_BITDEPTH);
    sbmpScreenshot->SetBitmap(outBitmap);
    if ( true==gotImage ) {
        statusBar->SetStatusText("screenshot: complete image received",1);
        txtSerialTrace->AppendText("Screenshot complete.\r\n");
    } else {
        if ( false==imageStart ) {
            txtSerialTrace->AppendText("Screenshot error: the response did not contain any image\r\n");
            statusBar->SetStatusText("screenshot: no image received",1);
        } else {
            txtSerialTrace->AppendText("Screenshot error: ran out of data before image end\r\n");
            statusBar->SetStatusText("screenshot: incomplete image received",1);
        }
    }

    GUI_Up();
    return;
}


//
// Button click: Save the current image to a BMP
//
void MyFrame::evtSaveImage(wxCommandEvent& event)
{
    wxString filePath;
    // screenshot available?
    if ( false==bGotScreenshot ) {
        statusBar->SetStatusText("save image: error, no screenshot available!",1);
        return;
    }
    // check if a previous saving path is remembered
    if ( strPrevSavePath.Length()<1 ) {
        strPrevSavePath = wxGetCwd(); // default to current directory
    }
    // open a Save File dialog for *.BMP
    wxFileDialog* saveDialog = new wxFileDialog (
        (wxWindow *)this, "Save screenshot to image file",
        strPrevSavePath, "", "Windows Bitmap (*.bmp)|*.bmp",
        wxSAVE|wxOVERWRITE_PROMPT
      );
    if ( wxID_CANCEL==saveDialog->ShowModal() ) { return; }
    filePath = saveDialog->GetPath();
    saveDialog->Destroy();
    // try to save the .BMP file
    if ( false==this->imgScreenshot->SaveFile (filePath,wxBITMAP_TYPE_BMP ) ) {
        statusBar->SetStatusText("save BMP: error during saving!",1);
    } else {
        statusBar->SetStatusText("save BMP: done.",1);
    }
    // store the path name for the next Save File dialog
    strPrevSavePath = filePath.BeforeLast('\\');
    return;
}


//
// Button click: Copy the current image to the clipboard
//
void MyFrame::evtClipboardImage(wxCommandEvent& event)
{
    // screenshot available?
    if ( false==bGotScreenshot ) {
        statusBar->SetStatusText("copy to clipbrd: error, no screenshot available!",1);
        return;
    }
    // try to place bitmap on the clipboard
    if( true==wxTheClipboard->Open() ) {
        wxTheClipboard->SetData ( new wxBitmapDataObject(sbmpScreenshot->GetBitmap()) );
        wxTheClipboard->Flush(); // keeps image on clipbrd even after this app exits
        wxTheClipboard->Close();
        statusBar->SetStatusText("copy to clipbrd: done.",1);
    } else {
        statusBar->SetStatusText("copy to clipbrd: error, couldn't open clipboard!",1);
    }
    return;
}


//
// Button click: Save to postscript file
//
void MyFrame::evtSavePostscript(wxCommandEvent& event)
{
    wxString filePath;
    // screenshot available?
    if ( strPostscript.Length()<128 ) {
        statusBar->SetStatusText("save postscript: error, no screenshot available!",1);
        return;
    }
    // check if a previous saving path is remembered
    if ( strPrevSavePath.Length()<1 ) {
        strPrevSavePath = wxGetCwd(); // default to current directory
    }
    // open a Save File dialog for *.BMP
    wxFileDialog* saveDialog = new wxFileDialog (
        (wxWindow *)this, "Save screenshot to Postscript file",
        strPrevSavePath, "", "Postscript (*.ps)|*.ps",
        wxSAVE|wxOVERWRITE_PROMPT
      );
    if ( wxID_CANCEL==saveDialog->ShowModal() ) { return; }
    filePath = saveDialog->GetPath();
    saveDialog->Destroy();
    // try to save the .PS file
    wxFile outputFile(filePath,wxFile::write);
    if ( true==outputFile.IsOpened() ) {
        outputFile.Seek(0, wxFromStart);
        if ( true==outputFile.Write(this->strPostscript,wxConvLibc) ) {
            statusBar->SetStatusText("save postscript: done",1);
        } else {
            statusBar->SetStatusText("save postscript: error, file write failed",1);
        }
        outputFile.Flush();
        outputFile.Close();
    } else {
        statusBar->SetStatusText("save postscript: error, couldn't create file!",1);
    }
    // store the path name for the next Save File dialog
    strPrevSavePath = filePath.BeforeLast('\\');
    return;
}

//
// Button click: download the selected waveform and store
//               it into the textbox as well as a file
// 
void MyFrame::evtGetWaveform(wxCommandEvent& event)
{
    int         wave=0;
    void        *queryPtr=NULL;
    wxString    query="", response="";
    bool        respOk=false, gotFullWF=false;
    wxString    matlabStr="", csvStr="", str="", preStr="";
    double      x_offset=0, y_offset=0, delta_x=0, delta_y=0;
    wxString    strXUnit="", strYUnit="";
    long        wavlen=0, idx;
    unsigned char wbyte=0;
    struct      _fluke190wfTraceAdmin* ptrTraceAdmin = NULL;

    // need to be connected first
    if ( false==bFlukeDetected ) {
        statusBar->SetStatusText("waveforms: no ScopeMeter detected",1);
        return;
    }

    // -- get the query string (written in ResetModeldependendGUI())
    
    wave = comboWaveforms->GetSelection();
    if ( wave<0 ) return;
    queryPtr = comboWaveforms->GetClientData(wave);
    if ( NULL==queryPtr ) return;
    query = *((wxString *)queryPtr);    

    GUI_Down(); 
    wxBusyCursor wait; // displays hourglass, reverts back to normal automatically 
    
    // -- execute the query, data is in binary (not ASCII)
    
    gotFullWF = false;
    response = QueryFluke(query,false,1000,&respOk); // <ack><cr>
    if ( !respOk ) {
        
        txtSerialTrace->AppendText("Waveform: query failed.\r\n");
        statusBar->SetStatusText("waveforms: download request failed",1);
        
    } else {

        statusBar->SetStatusText("waveforms: downloading",1);
        response = GetFlukeResponse(5000); // <binary data string>
        if ( response.Length()<=0 ) {
            txtSerialTrace->AppendText("Waveform: Timed out waiting for data.\r\n");
            gotFullWF = false;
        } else {        
            statusBar->SetStatusText("waveforms: got waveform",1);
            txtSerialTrace->AppendText("Waveform data:" + response + "\r\n");        
            gotFullWF = true;            
        }
    }
    
    // -- decode the waveform data into Matlab and CSV format (e.g. Excel importing)
    
    csvStr = ""; matlabStr = ""; preStr = "";
    idx = 0;
    
    if ( gotFullWF ) {
        
        wxStringTokenizer tkz(response, ",");
        if ( tkz.CountTokens()<10 ) {
            txtSerialTrace->AppendText("Waveform: SM response too short! Decoding anyway...\r\n");
        }

        switch(mScopemeterType) {
        case SCOPEMETER_190_SERIES:
        case SCOPEMETER_120_SERIES:
            // Response format (at least for the 190 series):
            // <trace_admin>,<trace_samples>
            //   where
            // <trace_admin> = #0<block_header><block_length><trace_result>
            //                 <y_unit><x_unit><y_divisions><x_divisions>
            //                 <y_scale><x_scale><y-step><x_step><y_zero>
            //                 <x_zero><y_resolution><x_resolution><y_at_0>
            //                 <x_at_0><date_stamp><time_stamp><check_sum>
            // <trace_samples>= #0<block_header><block_length><sample_format>
            //                 <overload><underload><invalid><nbr_of_samples>
            //                 <samples><check_sum><cr>
            if ( response.Length()<sizeof(_fluke190wfTraceAdmin) ) {
                txtSerialTrace->AppendText("Waveform: the response was too short, no valid data!\r\n");
                break;
            }
            ptrTraceAdmin = (_fluke190wfTraceAdmin*)response.GetData();
            if ( NULL==ptrTraceAdmin ) {
                txtSerialTrace->AppendText("Waveform: invalid string data pointer (NULL)\r\n");
                break;
            }
            // TODO: decode!
            // ptrTraceAdmin->block_length ...
            matlabStr = "hex: ";
            for ( idx=0; idx<(long)response.Length(); ++idx ) {
                matlabStr += wxString::Format("%02X ", (unsigned char)response.GetChar(idx));
            }
            csvStr = matlabStr;
            
            break;
            
        case SCOPEMETER_90_SERIES:
        case SCOPEMETER_97_SERIES:
            // A guess at the response format (undocumented?):
            // <description string>,<y axis unit e.g. Vols>,<x axis unit>,
            // <y offset value>,<x offset value>,<delta y>,<delta x>,
            // <ydivs (255 for 8-bit scopemeters)>,<xdivs>,
            // <binary data as 'xdivs' nr of bytes><one checksum(?) byte>
            // -- decode the data descriptors
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                csvStr = str + "\r\n\r\n";
                matlabStr = "title('" + str + "'), ";
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                matlabStr += "ylabel('" + str + "'), ";
                strYUnit = str;
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                matlabStr += "xlabel('" + str + "');\r\n";
                strXUnit = str;
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                // if str=>float conversion successful, change string
                if(str.ToDouble(&y_offset)) { str = wxString::Format("%f",y_offset); }
                else { y_offset=0; } // conversion error, use 0 in calcs
                csvStr += strYUnit + " offset:;" + str + ";";
                preStr += "yoffset=" + str + ", ";
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                if(str.ToDouble(&x_offset)) { str = wxString::Format("%f",x_offset); }
                else { x_offset=0; }
                csvStr += strXUnit + " offset:;" + str + ";";
                preStr += "xoffset=" + str + ", ";
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                if(str.ToDouble(&delta_y)) { str = wxString::Format("%f",delta_y); }
                else { delta_y=1; }
                csvStr += "\r\n";
                csvStr += "delta " + strYUnit + ":;" + str + ";";
                preStr += "deltay=" + str + ", ";
            str = tkz.GetNextToken(); idx += str.Length() + 1;
                if(str.ToDouble(&delta_x)) { str = wxString::Format("%f",delta_x); }
                else { delta_x=1; }
                csvStr += "delta " + strXUnit + ":;" + str + "\r\n";
                preStr += "deltax=" + str + ";\r\n";
            str = tkz.GetNextToken(); // ydivs (always 255?)
                csvStr += strYUnit + " divs:;" + str + ";";
                idx += str.Length() + 1;
            str = tkz.GetNextToken(); // xdivs = nr of wave bytes
                csvStr += strXUnit + " divs:;" + str + "\r\n";
                idx += str.Length() + 1;
            if (!str.ToLong(&wavlen, 10)) {
                str = response.Mid(idx, wxSTRING_MAXLEN); // actual data + chksum
                wavlen = str.Length()-1;                  // (don't include chksum)
            } else {
                str = response.Mid(idx, wavlen);          // actual data, no chksum
                if ( wavlen!=(long)(str.Length()) ) {
                    txtSerialTrace->AppendText(
                        wxString::Format("Waveform: waveform data length mismatch! ("
                                         "should be %ld, but got %d)\r\n",
                                         wavlen, str.Length()));
                }
            }

            // -- decode data and prepare CSV file and matlab vectors
            // csv data 
            csvStr += wxString::Format("\r\n"+strYUnit+" range;min:;%f;max:;%f\r\n",
                y_offset-((float)0x80)*delta_y, y_offset+((float)0x80)*delta_y);
            csvStr += wxString::Format(strXUnit+" range;min:;%f;max:;%f\r\n",
                x_offset, x_offset+delta_x*(wavlen-1));
            csvStr += "\r\n" + strXUnit + ";" + strYUnit + "\r\n";
            // matlab x vector
            preStr += "x = xoffset:deltax:(xoffset+deltax*";
            preStr += wxString::Format("%d);\r\n",(int)(wavlen-1));
            preStr += "y = [ ";
            for ( long i=0; i<wavlen && i<(long)(str.Length()); ++i ) {
                wbyte = (unsigned char)str.GetChar(i);
                // csv
                csvStr += wxString::Format("%f;%f\r\n",
                               i*delta_x+x_offset,     // X
                               ((double)wbyte-0x80)*delta_y+y_offset ); // Y
                // matlab vector
                preStr += wxString::Format("%f ", ((double)wbyte-0x80)*delta_y+y_offset);
            }
            // matlab
            preStr += " ]; \r\n";
            preStr += "figure(1), plot(x,y), ";
            preStr += wxString::Format("axis([%f %f %f %f]), ",
                        x_offset, x_offset+delta_x*(wavlen-1),
                        y_offset-((float)0x80)*delta_y, y_offset+((float)0x80)*delta_y);
            matlabStr = preStr + matlabStr;
            break;
        }//(switch)
        
        // -- store the Matlab code snippet in the textbox and on the clipboard
        txtWavestring->SetValue(matlabStr);
        if( true==wxTheClipboard->Open() ) {
            wxTheClipboard->SetData ( new wxTextDataObject(matlabStr) );
            wxTheClipboard->Flush();
            wxTheClipboard->Close();
            statusBar->SetStatusText("waveforms: got waveform - Matlab code on clipboard",1);
        }

        // -- save a .CSV file
        if(SCOPEMETER_97_SERIES==mScopemeterType || SCOPEMETER_90_SERIES==mScopemeterType) {

            if ( strPrevSavePath.Length()<1 ) {
                strPrevSavePath = wxGetCwd(); // default to current directory
            }
            // select a new file
            wxFileDialog* saveDialog = new wxFileDialog (
                (wxWindow *)this, "Save waveform data to CSV text file",
                strPrevSavePath, "", "Character Separated Values file (*.csv)|*.csv",
                wxSAVE|wxOVERWRITE_PROMPT
            );
            if ( wxID_CANCEL!=saveDialog->ShowModal() ) { 
                str = saveDialog->GetPath();
                // write info and waveform string to CSV file
                wxFile *outfile = new wxFile(str.c_str(), wxFile::write);
                if ( (NULL!=outfile) && (true==outfile->IsOpened()) ) {
                    wxDateTime now = wxDateTime::Now();
                    wxString modelStr = this->strScopemeterID;
                    modelStr.Replace(";"," ",true);
                    wxString infoStr = "## Fluke scopemeter waveform capture - generated with "
                        + wxString(PRODUCT_NAME)+wxString(" ")+wxString(FILE_VERSION)+ "\r\n"
                        + "## wave downloaded " + now.FormatISODate() + " " + now.FormatISOTime() + "\r\n"
                        + "## SM model: " +  modelStr + "\r\n"
                        + "## data in decimal notation (use find/replace . to , if necessary)\r\n"
                        + "\r\n";
                    if ( !outfile->Write(infoStr) || !outfile->Write(csvStr) ) {
                        txtSerialTrace->AppendText("Waveform: could not write data to CSV file!\r\n");
                    } else {
                        txtSerialTrace->AppendText("Waveform: stored to CSV file\r\n");
                        statusBar->SetStatusText("waveforms: Matlab code on clipboard, data in CSV file",1);
                    }
                    outfile->Close();
                }
                // store the path name for the next Save File dialog
                strPrevSavePath = str.BeforeLast('\\');
            }
            saveDialog->Destroy();

        } else {
            wxMessageBox(
                "Waveform data decoding is currently supported only for ScopeMeter 9x series.\r\n"
                "The raw hex waveform data has been placed on the clipboard.",
                "Note",
                wxOK | wxICON_INFORMATION, this);
        }         
    }

    GUI_Up();
    
    return;    
}

//
// Menu: display the About dialog
//
void MyFrame::OnMenuAbout(SG32_MENU_EVT_TYPE event)
{
    // Show prog and version info (cf. ScopeGrab32_private.h and/or project settings)
    wxMessageBox(wxString(PRODUCT_NAME)+wxString(" ")+wxString(FILE_VERSION)+
        "\r\n\rAn open source tool for communicating\r\n"
        "with a Fluke Scopemeter and also do a\r\nscreen capture.\r\n\r\n"
        "Supported models: 190 and 190C series\r\n\r\n"
        "Beta support: 123,124,91,92,96 and 97,99\r\n\r\n"
        "(C) 2005 Jan Wagner\r\nRequests and bugs to: jan.wagner@iki.fi\r\n\r\n"
        "Thanks go to: Jens F., Ethan W., Stuart P., \r\n"
        "Wayne L., Kristijan B., Leif K.",
        "About ScopeGrab32",
        wxOK | wxICON_INFORMATION, this);
    return;
}

//
// Menu: display the help file
//
void MyFrame::OnMenuGuide(SG32_MENU_EVT_TYPE event)
{
    m_helpFile->DisplayContents();
}

//
// Menu: exit the program
//
void MyFrame::OnMenuExit(SG32_MENU_EVT_TYPE event)
{
    // close serial comms
    tmrToggleRTS->Stop();
    if(NULL!=mySerial) {
        mySerial->closePort();
        delete mySerial;
    }
    // store settings
    if(this->mConfigs!=NULL) {
        mConfigs->Write("ComPort",(long)comboCOM->GetSelection());
        mConfigs->Write("UseBaud",(long)comboBaud->GetSelection());
        mConfigs->Write("DisableBaudWarn",m_menuNoBaudWarn->IsChecked());
        mConfigs->Flush();
        delete mConfigs;
    }
    // exit the app
    Close();
}



// ----------------------------------------------------
// -------------    SERIAL PORT FUNCS     -------------
// ----------------------------------------------------


//
// Timer event: pulse the RTS pin to generate
// Scopemeter PM9080 cable and DIY cable circuit supply voltages
//
void MyFrame::evtRtsTimer(wxTimerEvent& event)
{
    // safety checks
    if ( NULL==mySerial ) return;
    if ( false==mySerial->isOpen() ) {
        tmrToggleRTS->Stop();
        return;
    }
    // flip RTS state
    mySerial->setRTS(!mySerial->getRTS());
    return;
}


//
// Callback func for CSerial class, receive new data bytes
//
void MyFrame::OnCommEvent(const BYTE* rxbuf, const DWORD rxbytes)
{
   // checks
   if ( NULL==rxbuf || 0==rxbytes ) return;
   
   // set flag to indicate that the rx line is still alive
   bRxReceiverActive = true;

   // keep statistics
   RxTotalBytecount += rxbytes;
   
   //wxCriticalSectionLocker buflock(csRxBuf);
   
   BYTE currByte;
   for ( DWORD i=0; i<rxbytes; ++i ) {

        currByte = *(rxbuf+i);

        // -- handle characters in ASCII mode
        if ( true==this->bRxAsciiMode ) {

            if ( 10==currByte ) { // '\n'
                continue;
            }
            
            // a <cr> carriage return indicates the end of an ascii block
            if ( 13==currByte ) { // '\r'
                if ( CurrRxString.Length()>0 ) {
                    // insert at the start of the responses list, remove at end
                    ReceivedStrings.Insert(CurrRxString, 0, 1);
                    // check if we need to switch to binary mode after
                    // now having rxed a full ascii line (the <ack> code)
                    if ( true==this->bRxBinarymodeAfterACK ) {
                        this->bRxBinarymodeAfterACK = false;
                        this->bRxAsciiMode = false;
                    }
                }
                CurrRxString.Clear();
                // next bytes
                continue;
            }

            // check for valid characters
            if ( currByte<32 || currByte>127 ) {
                // illegal char, inc error counter, skip char
                if(RxErrorCounter<0xFFFF) { ++RxErrorCounter; }
                continue;
            }
            
        // -- handle bytes in Binary mode
        } else {
            // just append data, same as ascii mode, below:
        }
        
        // -- append character/byte to the current string
        CurrRxString.Append(currByte, 1);
        //CurrRxString.Shrink();
   }
   
   return;
}


//
// Wait to receive a complete response from the ScopeMeter
//
wxString MyFrame::GetFlukeResponse(DWORD msTimeout)
{
    wxString response="";
    DWORD cnt;
    bool respIsAscii = false; // flag, True=print response as text to txtSerialTrace
    
    // limits etc checks
    if ( msTimeout<200 || msTimeout>3000 ) { msTimeout=1000; }
    if ( NULL==mySerial || false==mySerial->isOpen() ) { return wxString(""); }

    // return a response that was received earlier? (ascii ack, or other ascii lines)
    size_t itemCount = ReceivedStrings.GetCount();
    if ( itemCount>0 ) {

        response = ReceivedStrings.Item(itemCount-1); // get last
        #if wxCHECK_VERSION(2,6,0)
        ReceivedStrings.RemoveAt(itemCount-1, 1); // remove from end
        #else
        ReceivedStrings.Remove(itemCount-1, 1); // remove from end
        #endif
        respIsAscii = true;

    // wait for new response data to come in...
    } else {

        itemCount = 0;
        cnt = 1+(msTimeout/200);
        this->bRxReceiverActive = false; // flag used to detect rx activity
        while ( cnt>0 ) {
            // wait
            #ifdef __WIN32__            
            SleepEx(200,TRUE);
            #else
            // linux task_sleep()
            usleep(200000L);
            #endif
            DoEvents();
            // check receiver activity - did OnCommEvent() set the flag?
            if ( true==this->bRxReceiverActive ) {
                this->bRxReceiverActive = false;
                cnt = 1+(msTimeout/200); // reset back to max timeout
            } else {
                --cnt; // no activity, dec timeout
            }
            // check current count of available responses
            itemCount = ReceivedStrings.GetCount();
            if ( itemCount>0 ) break;
            // in binary mode, just wait for timeout after last byte is received
        }

        // binary mode responses
        if ( false==this->bRxAsciiMode ) {

            // binary mode, if previous ASCII ack was received
            // then return it first!
            if ( itemCount>0 ) {
                response = ReceivedStrings.Item(itemCount-1); // get last
                #if wxCHECK_VERSION(2,6,0)
                ReceivedStrings.RemoveAt(itemCount-1, 1); // remove from end
                #else
                ReceivedStrings.Remove(itemCount-1, 1); // remove from end
                #endif
                respIsAscii = true;
            } else {
                // just return all currently received bytes
                response = CurrRxString;
                CurrRxString = "";       //?buffer access lock?
                // todo: hex dump of the data into txtSerialTrace?
            }

        // ascii mode responses
        } else {

            // timeout in ascii mode
            if ( 0==itemCount ) {
                txtSerialTrace->AppendText("Response: (timeout)\r\n");
                return wxString("");
            }

            // got at least 1 response string, remove it from array end
            response = ReceivedStrings.Item(itemCount-1); // get last
            #if wxCHECK_VERSION(2,6,0)
            ReceivedStrings.RemoveAt(itemCount-1, 1); // remove from end
            #else
            ReceivedStrings.Remove(itemCount-1, 1); // remove from end
            #endif
            respIsAscii = true;
            
        }//if(bin/ascii)

    }//if(existing rx data)

    // do a bit of ascii cleaned-up
    if ( true==respIsAscii ) {
        response.Replace("\r", "", true);
        response.Replace("\n", "", true);
        //response.Shrink();
        if ( response.Length()>0 ) {
            txtSerialTrace->AppendText("Response: " + response + "\r\n");
        }
    }
    
    // done
    return response;
}


//
// Send a query to the ScopeMeter and return the ACK response.
// The remaining response data, if it exists, can be retrieved
// with GetFlukeResponse
//
wxString MyFrame::QueryFluke(wxString cmdString, bool bAsciiMode,
    DWORD msTimeout, bool* ResponseIsOK)
{
    wxString response;
    wxString tmp;
    
    // clean up the command string, unless it contains also data
    if ( cmdString.IsAscii() ) {
       // allow also blank commands (just spaces), do check:
       tmp = cmdString;
       tmp = (tmp.Trim(true)).Trim(false);
       // clean up if not a blank command
       if ( false==tmp.IsEmpty() ) {
          cmdString = (cmdString.Trim(true)).Trim(false);
          cmdString.Replace("\r", " ", true);
          cmdString.Replace("\t", " ", true);
          cmdString.Replace("\n", " ", true);
          while ( cmdString.Replace("  ", " ", true) > 0);
          cmdString = cmdString.MakeUpper();
       }
    }
    
    // add cmd terminator char (carriage return)
    cmdString.Append("\r");
    
    // clear current rx buffer and reset settings to defaults
    this->RxErrorCounter = 0;
    this->CurrRxString = "";
    this->ReceivedStrings.Empty();
    this->bRxReceiverActive = false;
    this->bRxAsciiMode = true;
    this->bRxBinarymodeAfterACK = false; // full ascii rx

    // modify flag for response mode ascii+bin (ascii <ack> followed by binary)
    // the mode will be switched later in OnCommEvent
    if ( false == bAsciiMode ) { this->bRxBinarymodeAfterACK = true; }

    // send the command
    txtSerialTrace->AppendText("Query: " + cmdString + "\n");
    const char* dataToSend = cmdString.c_str();
    if ( false == mySerial->transmitData(
            (const BYTE*)dataToSend, cmdString.Length())
        )
    {
       txtSerialTrace->AppendText(
         wxString::Format("Query Error: transmitData failed, win32 error 0x%04lX\r\n",
            mySerial->getLastError()) );
       if ( NULL!=ResponseIsOK ) *ResponseIsOK = false;
       return "";
    }
    
    // get the <ack> response (ASCII always, even when bAsciiMode==false)
    response = GetFlukeResponse(msTimeout);
    response.Replace("\r", "", true);
    response.Replace("\n", "", true);
        
    // handle the <ack> response
    if ( NULL!=ResponseIsOK ) {
        *ResponseIsOK = false;
    }
    if ( RxErrorCounter>0 ) {
       txtSerialTrace->AppendText(wxString::Format("Response ACK: encountered %ld errors\r\n", RxErrorCounter));
    }
    if ( 0 == response.Length() ) {

        txtSerialTrace->AppendText("Response ACK timeout\r\n");

    } else if ( 1 == response.Length() ) {
        char ackCode = response.GetChar(0);
        switch(ackCode) {
         case '0': if ( NULL!=ResponseIsOK ) { *ResponseIsOK = true; }
                  break; // ACK0, No error
         case '1': txtSerialTrace->AppendText("Response ACK1: Syntax Error\r\n"); break;
         case '2': txtSerialTrace->AppendText("Response ACK2: Execution Error\r\n"); break;
         case '3': txtSerialTrace->AppendText("Response ACK3: Synchronization Error\r\n"); break;
         case '4': txtSerialTrace->AppendText("Response ACK4: Communication Error\r\n"); break;
         default : txtSerialTrace->AppendText(
                wxString::Format("Response ACK error: ACK%c is an unknown ACK code\r\n", ackCode) );
        }

    } else { // response ack length>1

        txtSerialTrace->AppendText("Response error: expected an ACK code. Instead got <"+ response + ">\r\n");

    }
    
    return response;
}

