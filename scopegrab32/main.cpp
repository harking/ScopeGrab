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

//#define SIMULATE // define to simulate a connected scopemeter97

// ************************************************************************
// ***                                                                  ***
// ***  ScopeGrab32 - A tool for the Fluke ScopeMeter series            ***
// ***  (C) 2004 Jan Wagner                                             ***
// ***                                                                  ***
// ***  Version: 2.1.0 alpha                                            ***
// ***                                                                  ***
// ***  Licence: GNU GPL                                                ***
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
// ***  This source was developed and compiled using:                   ***
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
// ************************************************************************

#include "main.h"

#define SG32_VERSION_STR    "ScopeGrab32 v2.1 alpha"

// instantiate the wxWidgets application MyAp (similar to WinMain())
IMPLEMENT_APP(MyApp)

// ----------------------------------------------------
// -------------      GUI EVENT MAP       -------------
// ----------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame,wxFrame)

    EVT_MENU(ID_MENU_ABOUT,MyFrame::OnMenuAbout)
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
    
    EVT_CHAR(MyFrame::evtKeyDown)
    
END_EVENT_TABLE()

// ----------------------------------------------------
// -------------        CONSTANTS         -------------
// ----------------------------------------------------

// list of baud rates that are supported by Fluke ScopeMeters
int fluke_baudrates[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600 };

// array for storing serial port ID's
int combo_portIDs[MAX_COMPORT_COUNT];

#define FLUKE90_WF_QUERY_NUM    11
wxString fluke90wfQueries[FLUKE90_WF_QUERY_NUM] = {
        "QW 101", "QW 102", "QW 103", "QW 104", "QW 105", "QW 106",
        "QW 107", "QW 108", "QW 109", "QW 110", "QW 111"
    };
wxString fluke90wfNames[FLUKE90_WF_QUERY_NUM] = {
        "Channel A", "Channel B", "Channel maths", "Temp 1", "Temp 2",
        "Temp 3", "Memory 4", "Memory 5", "Memory 6", "Memory 7",
        "Memory 8"
    };
#define FLUKE190_WF_QUERY_NUM   9
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
	frameMain->Show(TRUE);
	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned FALSE here, the
	// application would exit immediately.
	return TRUE;
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
    bRxAsciiMode = TRUE;
    bRxBinarymodeAfterACK = FALSE;
    bRxReceiverActive = FALSE;
    bFlukeDetected = FALSE;
    bGotScreenshot = FALSE;
    bEscKey = FALSE;
    RxErrorCounter = 0;
    RxTotalBytecount = 0;
    mScopemeterType = SCOPEMETER_NONE;
    strScopemeterID = "";
}





// ----------------------------------------------------
// -------------   MAIN WINDOW GUI INIT   -------------
// ----------------------------------------------------

void MyFrame::VwXinit()
{
    // create a serial RX/TX class and mutex
    mySerial = new CSerial(this);
    tmrToggleRTS = new wxTimer(this, ID_RTSTIMER);

    // -- add GUI components

    Show(false);
    SetBackgroundColour(wxSystemSettings::GetColour((wxSystemColour)wxSYS_COLOUR_3DLIGHT));

    // -- menu and status bar
    wxMenuItem *itemmenu;
    m_menuBar=new wxMenuBar();
    m_menuFile=new wxMenu();
        m_menuBar->Append(m_menuFile,wxT("File"));
    m_menuHelp=new wxMenu();
        m_menuBar->Append(m_menuHelp,wxT("Help"));
    itemmenu = new wxMenuItem(NULL,ID_MENU_EXIT,wxT("E&xit"),wxT(""),0);
        m_menuFile->Append(itemmenu);
    itemmenu = new wxMenuItem(NULL,ID_MENU_ABOUT,wxT("About"),wxT(""),0);
        m_menuHelp->Append(itemmenu);
    SetMenuBar(m_menuBar);
        statusBar=new wxStatusBar(this,-1,0,"statusBar");
        statusBar->SetFieldsCount(1,NULL);
        statusBar->SetStatusText("  ",0);
    SetStatusBar(statusBar);
  
    // -- fluke serial comm
    st_1=new wxStaticText(this,-1,wxT(""),wxPoint(11,11),wxSize(30,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_1->SetLabel(wxT("Serial port setup:"));
    comboCOM=new wxComboBox(this,ID_COMBO_COM,wxT(""),wxPoint(115,6),wxSize(65,21),0,NULL);
    comboBaud=new wxComboBox(this,ID_COMBO_BAUD,wxT(""),wxPoint(190,6),wxSize(80,21),0,NULL);
    btnReconnect=new wxButton(this,ID_BTN_RECONNECT,wxT(""),
        wxPoint(280,6),wxSize(65,21));
        btnReconnect->SetLabel(wxT("Connect"));
        btnReconnect->SetTitle(wxT("Connect"));

    st_2=new wxStaticText(this,-1,wxT(""),wxPoint(11,35),wxSize(30,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_2->SetLabel(wxT("Fluke ScopeMeter:"));
    stFlukeID=new wxStaticText(this,-1,wxT(""),wxPoint(115,35),wxSize(130,19),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        stFlukeID->SetLabel(wxT("<not detected>"));

    nbNote=new wxNotebook(this,-1,wxPoint(6,60),
        wxSize((155+FLUKESCREEN_WIDTH),(45+FLUKESCREEN_HEIGHT)));
        nbNote->SetTitle(wxT("State"));

    // -- screen capture
    pnlCapture=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(20+FLUKESCREEN_HEIGHT)),wxNO_BORDER|wxCLIP_CHILDREN);
        nbNote->AddPage(pnlCapture,"Screen capture",false);
        pnlCapture->SetTitle(wxT("Screen capture"));
    imgScreenshot=new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
    sbmpScreenshot=new wxStaticBitmap(pnlCapture,ID_SCREENSHOT,
            wxBitmap(imgScreenshot,IMAGE_BITDEPTH),
            wxPoint(5,5),wxSize(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT),
            wxTHICK_FRAME,"sbmpScreenshot");
    imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
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
    st_4->SetLabel(wxT("Select waveform to download:"));
    comboWaveforms=new wxComboBox(pnlTools,-1,wxT(""),wxPoint(160,11),
        wxSize(120,22),0,NULL);
    btnGetWaveform=new wxButton(pnlTools,ID_BTN_WAVE,wxT(""),
        wxPoint(290,11),wxSize(70,21));
        btnGetWaveform->SetLabel(wxT("Download"));
        btnGetWaveform->SetTitle(wxT("Download")); 
    txtWavestring=new wxTextCtrl(pnlTools,-1,wxT(""),
        wxPoint(11,35),wxSize(FLUKESCREEN_WIDTH,22),wxTE_READONLY|wxTE_NOHIDESEL);
        txtWavestring->SetFont(wxFont(8,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE,"Arial",wxFONTENCODING_SYSTEM));
        txtWavestring->SetForegroundColour(*wxLIGHT_GREY);
        txtWavestring->SetValue("<Matlab vector of downloaded waveform data>");
            
    // -- serial trace/console
    pnlConsole=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(20+FLUKESCREEN_HEIGHT)),wxNO_BORDER);
        nbNote->AddPage(pnlConsole,"Serial console",false);
        pnlConsole->SetTitle(wxT("Serial console"));
    txtSerialTrace=new wxTextCtrl(pnlConsole,-1,wxT(""),
        wxPoint(1,1),wxSize(FLUKESCREEN_WIDTH+145,FLUKESCREEN_HEIGHT-30),
        wxTE_RICH|wxTE_MULTILINE|wxTE_READONLY);
        txtSerialTrace->SetFont(wxFont(8,wxDEFAULT,wxNORMAL,wxNORMAL,
            FALSE,"Arial",wxFONTENCODING_SYSTEM));
    st_3=new wxStaticText(pnlConsole,-1,wxT(""),
        wxPoint(1,FLUKESCREEN_HEIGHT-10),wxSize(50,13),
        wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_3->SetLabel(wxT("Command:"));
    txtCommandToSend=new wxTextCtrl(pnlConsole,ID_TXTCTL_CONSOLECMD,wxT(""),
        wxPoint(55,FLUKESCREEN_HEIGHT-12),
        wxSize(320,18),wxTE_DONTWRAP|wxTE_PROCESS_ENTER);
    btnSendCommand=new wxButton(pnlConsole,ID_BTN_SEND,wxT(""),
        wxPoint(380,FLUKESCREEN_HEIGHT-12),wxSize(60,18));
        btnSendCommand->SetLabel(wxT("send"));
        btnSendCommand->SetTitle(wxT("Send"));


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
        comboCOM->Append(wxString::Format("/dev/ttyS%d",i).c_str(), &combo_portIDs[portCnt])
    }
    #endif
    comboCOM->SetSelection(0);

    // next, add ScopeMeter supported baud rates
    for(unsigned int i=0;i<(sizeof(fluke_baudrates)/sizeof(int));++i) {
        comboBaud->Append(
            wxString::Format("%d", fluke_baudrates[i]), &fluke_baudrates[i]
        );
    }
    comboBaud->SetSelection(0);
  
    // apply default port settings COM1 1200baud) now
    this->ChangeComPort();
  
    return;
}


//
// Reset GUI entries (combo boxes etc) with ScopeMeter
// model dependend infos
//
void MyFrame::ResetModeldependendGUI()
{
    if ( FALSE==bFlukeDetected ) return;

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
            break;
    }
    
    // (add other processing here)
    
    return;
}


//
// Safe GUI handling, enable or disable the input controls
//
void MyFrame::GUI_State(BOOL on)
{
    comboBaud->Enable(on); comboCOM->Enable(on);
    comboWaveforms->Enable(on);
    m_menuBar->EnableTop(0,on);
    m_menuBar->EnableTop(1,on);
    btnGetScreenshot->Enable(on); btnSaveScreenshot->Enable(on);
    btnCopyScreenshot->Enable(on);
    btnSavePostscript->Enable(on && (SCOPEMETER_190_SERIES==mScopemeterType)); // only 190/190C does postscript
    btnReconnect->Enable(on); btnSendCommand->Enable(on);
    btnGetWaveform->Enable(on);   
    return;
}

void MyFrame::GUI_Up()
{
    GUI_State(TRUE);
    return;
}

void MyFrame::GUI_Down()
{
    GUI_State(FALSE);
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
        this->bEscKey = TRUE; 
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
    // call ChangeComPort() to apply the settings, but skip
    // duplicate events
    static BOOL firstCall = TRUE;
    if ( TRUE==firstCall ) { this->ChangeComPort(); }
    firstCall = !firstCall;
}

//
// Apply new port settings
//
void MyFrame::ChangeComPort()
{
    BOOL ret;
    BOOL respOk;
    
    bFlukeDetected = FALSE;
    mScopemeterType = SCOPEMETER_NONE;

    // get the current/new port number
    int idx = comboCOM->GetSelection();
    if ( NULL==comboCOM->GetClientData(idx) ) return;
    int port = * (int*)comboCOM->GetClientData(idx);
    
    // also get the baud rate
    idx = comboBaud->GetSelection();
    if ( NULL==comboBaud->GetClientData(idx) ) return;
    int baud = * (int*)comboBaud->GetClientData(idx);

    // (re)open the port
    // fluke comm is always 8 databit, 1 stopbit, no parity
    if(NULL==mySerial) { mySerial = new CSerial(this); }
    wxBusyCursor wait;
    tmrToggleRTS->Stop();
    CurrRxString.Clear();
    ReceivedStrings.Clear();

    GUI_Down();
    ret = mySerial->openPort(port, baud, 8, ONESTOPBIT, 'N', 0);
    if(FALSE==ret)
    {
        // show error in status bar
        wxString errStr = wxString::Format(
            "error opening COM%d @ %d baud - system error 0x%04lX",
            port, baud, mySerial->getLastError() );
        statusBar->SetStatusText(errStr,0);

    } else {

        wxString infoStr = wxString::Format("COM%d @ %d baud opened", port, baud);

        // init circuit supply voltage 'generator'
        tmrToggleRTS->Start(TIMER_TOGGLERATE, FALSE); // given rate, not single-shot
        DoEvents();
        SleepEx(400,FALSE);
        DoEvents();

        // try to get identification from the Fluke : <acknowledge><cr><infos><cr>
        wxString response = QueryFluke("ID",TRUE,1000,&respOk); // just the <ack>
        if(TRUE==respOk) {
           response = GetFlukeResponse(250); // <infos> string
        }

        if(response.Freq(';')>1) {
            bFlukeDetected = TRUE;
            strScopemeterID = response;
            // show ID in text label
            stFlukeID->SetLabel(response.BeforeFirst(';'));
            // ... and in the status bar
            infoStr = infoStr + " - ident=" + response;
            statusBar->SetStatusText(infoStr,0);
            // next, try to extract the scopemeter series info
            response = response.MakeUpper();
            response = response.BeforeFirst(';');
            if(1!=response.Contains("SCOPE")) {
                // no Fluke ScopeMeter
                mScopemeterType = SCOPEMETER_NONE;
                bFlukeDetected = FALSE;
                infoStr = "(unsupported) " + infoStr;
                statusBar->SetStatusText(infoStr,0);
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
                bFlukeDetected = FALSE;
                infoStr = "(unsupported) " + infoStr;
                statusBar->SetStatusText(infoStr,0);
            }
        } else {
            bFlukeDetected = FALSE;
            // update text label and status bar
            stFlukeID->SetLabel("<not detected>");
            infoStr = infoStr + " - no ScopeMeter";
            statusBar->SetStatusText(infoStr,0);
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
    
    if ( NULL==mySerial || FALSE==mySerial->isOpen() ) {
        txtSerialTrace->AppendText("Error: port not opened, can't send the command.\r\n");
        txtCommandToSend->SetValue("");
        return;
    }
    
    // send the command and dump all response data (can be multiple lines)
    BOOL respOk;
    GUI_Down();

    wxString sendStr = txtCommandToSend->GetValue();
    wxString response = QueryFluke(sendStr,TRUE,1000,&respOk); // <ack>
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
    BOOL gotImage = FALSE;
    BOOL imageStart = FALSE;
    long rxbytesremaining = 0;
    unsigned char currbyte = 0;
    unsigned int y_counter = 0, x_counter = 0;
    unsigned int bitnr = 0, bitval = 0;
    BYTE color = 0;
    wxBitmap* bmpTempBitmap = NULL;
    unsigned int GraphicsFormat = GFXFORMAT_NONE;

    if ( FALSE==bFlukeDetected ) {
        statusBar->SetStatusText("screenshot: no ScopeMeter detected",0);
        return;
    }

    wxString command="", response = "", strOldBaud="";
    BOOL respOk=FALSE;

    GUI_Down();
    
    wxBusyCursor wait; // displays hourglass, reverts back to normal automatically
    
    // -- increase baud rate to 19200 (first the Fluke, then the PC)
    DWORD oldbaud = 0;
    oldbaud = mySerial->getBaudrate();
    if ( oldbaud < 19200 ) {
        txtSerialTrace->AppendText("Temporarily setting Fluke baudrate to 19200...\r\n");
        statusBar->SetStatusText("screenshot: changing Fluke to 19200 baud",0);

        switch(mScopemeterType) {
            case SCOPEMETER_190_SERIES:
            case SCOPEMETER_120_SERIES:
                command = "PC 19200";
                strOldBaud = wxString::Format("PC %ld", oldbaud);
                break;
            case SCOPEMETER_90_SERIES:
                command = "PC19200,N,8,1,XONXOFF";
                strOldBaud = wxString::Format("PC%ld,N,8,1,XONXOFF", oldbaud);
                break;
            case SCOPEMETER_97_SERIES:
                command = "PC 19200,N,8,1,XONXOFF";
                strOldBaud = wxString::Format("PC %ld,N,8,1,XONXOFF", oldbaud);
                break;
        }
        response = QueryFluke(command,TRUE,1000,&respOk); // <ack><cr>
        
        if ( TRUE==respOk ) {
            txtSerialTrace->AppendText("Temporarily setting PC baudrate to 19200...\r\n");
            statusBar->SetStatusText("screenshot: changing PC to 19200 baud",0);
            mySerial->setBaudrate(19200);
            //??possibly need to add: set CSerial to Xon/Xoff handshaking for SM90/97 series
        } else {
            txtSerialTrace->AppendText("Couldn't set Fluke baurdate to 19200.\r\n");
            statusBar->SetStatusText("screenshot: failed to set ScopeMeter to 19200 baud, aborted",0);
            GUI_Up();
            return;
        }
    }
    
    // -- request screenshot

    bGotScreenshot = FALSE;
    RxTotalBytecount = 0;
    
    switch(mScopemeterType) {
        case SCOPEMETER_190_SERIES:
        case SCOPEMETER_120_SERIES:
            // Query Print screen 0 and postscript (3) format
            command = "QP 0,3";
            GraphicsFormat = GFXFORMAT_POSTSCRIPT;
            statusBar->SetStatusText("screenshot: requesting postscript image",0);
            break;
        case SCOPEMETER_90_SERIES:
            // Query Print live screen and Epson ESC sequence format
            command = "QP"; // ???correct??
            GraphicsFormat = GFXFORMAT_EPSONESC;
            statusBar->SetStatusText("screenshot: requesting screenshot",0);
            break;
        case SCOPEMETER_97_SERIES:
            // Query Graphics live screen and Epson ESC sequence format
            command = "QP"; // QP seems to work too (was "QG129")
            GraphicsFormat = GFXFORMAT_EPSONESC;
            statusBar->SetStatusText("screenshot: requesting screen graphics",0);
            break;
    }

    // -- wait for graphics data to arrive
    this->bEscKey = FALSE;
    if ( GFXFORMAT_POSTSCRIPT==GraphicsFormat ) {
        //
        // Receive postscript data in ASCII mode
        //
        this->strPostscript = "";
        response = QueryFluke(command,TRUE,1000,&respOk); // first just the <ack>
        while ( (response.Length()>0) && (TRUE==respOk) ) {
            // get next line
            response = GetFlukeResponse(1000); // <printer or image data><cr>
            
            // interrupt if user wants
            if ( TRUE==this->bEscKey ) {
                statusBar->SetStatusText("screenshot: user cancelled (ESC)",0);                
                gotImage = FALSE;
                this->bEscKey = FALSE;
                break;    
            }
            
            // store to postscript temp var
            this->strPostscript.Append(response);
            this->strPostscript.Append("\r\n");
        
            // scan up to the
            //   "... { currentfile 60 string readstring pop } image" line
            // in the response - the image data starts on the next line
            if ( FALSE==imageStart ) {
                if ( response.Find("image") >= 0 ) {
                    imageStart = TRUE;
                    imgScreenshot=new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                    this->imgScreenshot->Create(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                    this->imgScreenshot->SetMask(FALSE);
                    this->imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
                    statusBar->SetStatusText("screenshot: receiving image...",0);
                }
                continue;
            }

            // a line with "showpage" indicates the end of the image data
            if ( response.Find("showpage") >= 0 ) {
                gotImage = TRUE;
                break;
            }

            // process the current line, all upper case HEX data strings
            response = response.MakeUpper();
            for ( unsigned int idx=0; idx<response.Length(); ++idx ) {
                // check hex format
                currbyte = response.GetChar(idx);
                if ( !(currbyte>='0' && currbyte<='9') &&
                     !(currbyte>='A' && currbyte<='F') ) { continue; }
                // hex to dec
                if(currbyte>='0' && currbyte<='9') { currbyte -= '0'; }
                else { currbyte = 10+(currbyte-'A'); }
                // one character (0..F) marks 4 pixels (bits), with MSb on the left
                bitval=0x08;
                for ( bitnr=3; bitnr>=0; --bitnr, bitval/=2 ) {
                    color = 0xFF;
                    if ( 0==(currbyte&bitval) )
                        { color = 0x00; } // bit cleared: black
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
            if ( NULL!=bmpTempBitmap ) { delete bmpTempBitmap; }
            bmpTempBitmap = new wxBitmap(imgScreenshot,IMAGE_BITDEPTH);
            sbmpScreenshot->SetBitmap(*bmpTempBitmap);
            sbmpScreenshot->Refresh(false, NULL);
        }
        // done receiving postscript image
        if ( TRUE==imageStart ) { bGotScreenshot = TRUE; }
        
    } else if ( GFXFORMAT_EPSONESC==GraphicsFormat ) {
        //
        // Receive Epson ESC code data in binary mode
        //
        // Before the binary graphics data starts, there's still the
        // ack code and then the count of bytes to receive
        // e.g. "0<cr>7454,<epson esc binary data><cr><more data><cr>...
        //
        rxbytesremaining = 0;
        BOOL dataStart = FALSE;
        wxString newstr = "";
        const char STR_GFXSTART[] = { 0x1B, 0x2A, 0x04, '\0' };
        const int  LEN_STR_GRFXSTART = 3;
        unsigned int data_len = 0;
        unsigned int imageblock_retries = 0;
        
        response = QueryFluke(command,FALSE,1000,&respOk); // false=>binary mode, <ack> still rx'ed in ASCII mode
        response = "";
        
        while ( (TRUE==respOk) && (FALSE==gotImage) ) {

            DoEvents();
            
            // get more data
            if ( FALSE==dataStart || response.Length()<=256 || imageblock_retries>0 ) {
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
                gotImage = TRUE; // finished receiving the screenshot                
                if ( (FALSE==imageStart) || (FALSE==dataStart) ) {
                    txtSerialTrace->AppendText("Epson ESC: Fluke didn't send data, timeout error.");
                    respOk = FALSE;
                    gotImage = FALSE;
                } else if ( rxbytesremaining>240 || imageblock_retries>=3 ) {
                    txtSerialTrace->AppendText("Timeout while waiting for more data. Image may be incomplete.\r\n");
                    gotImage = FALSE;
                }
                break;
            }

            // -- interrupt now if user wants
            if ( TRUE==this->bEscKey ) {
                statusBar->SetStatusText("screenshot: user cancelled (ESC)",0);                
                gotImage = FALSE;
                this->bEscKey = FALSE;
                break;    
            }
            
            // -- start of data? read the byte count e.g. "7453,<data>"
            if ( FALSE==dataStart ) {
                if ( response.Length()<5 ) continue;
                size_t commaidx = response.Index(',');
                if ( (size_t)wxNOT_FOUND!=commaidx ) {
                    long ltmp = 0;
                    wxString lenStr = response.Left(commaidx);
                    lenStr.Replace("\r", "", TRUE);
                    lenStr.Replace("\n", "", TRUE);
                    if ( TRUE==lenStr.ToLong(&ltmp,10) ) {
                        rxbytesremaining = ltmp + 1; // fluke sends x+1 bytes...
                        txtSerialTrace->AppendText("Epson ESC byte count string: '"+lenStr+"'\r\n");
                        imgScreenshot=new wxImage(EPSONSCREEN_WIDTH,EPSONSCREEN_HEIGHT);
                        this->imgScreenshot->Create(EPSONSCREEN_WIDTH,EPSONSCREEN_HEIGHT);
                        this->imgScreenshot->SetMask(FALSE);
                        this->imgScreenshot->Replace(0,0,0, 0xFF,0xFF,0xFF);
                        statusBar->SetStatusText("screenshot: receiving Epson ESC...",0);
                        dataStart = TRUE; // actual data starts after the comma
                    } else {
                        statusBar->SetStatusText("screenshot: response format error",0);
                        txtSerialTrace->AppendText("Error, unexpected Epson ESC byte count string: '"+lenStr+"'\r\n");
                        respOk = FALSE; // quit
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
                if ( (response.Length()>256) && (FALSE==imageStart) ) {
                   txtSerialTrace->AppendText("Epson ESC: no graphics in first 256 bytes of response, aborting rx\r\n");
                   respOk = FALSE;
                } else {
                    continue;
                }
            } else { 
                txtSerialTrace->AppendText(wxString::Format("Epson ESC: got start sequence, line %d\r\n", y_counter));
                imageStart = TRUE; 
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
            rxbytesremaining -= LEN_STR_GRFXSTART+2+data_len; // data block
            rxbytesremaining -= escIndex+1; // stuff before data block
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
                gotImage = TRUE; // stop drawing if outside 260x260
            }
            imageblock_retries = 0; // reset, until next block
            
            // update image
            if ( NULL!=bmpTempBitmap ) { delete bmpTempBitmap; }
            bmpTempBitmap = new wxBitmap(imgScreenshot,IMAGE_BITDEPTH);
            sbmpScreenshot->SetBitmap(*bmpTempBitmap);
            //sbmpScreenshot->Refresh(false, 
            //                    new wxRect(0,y_counter-8*EPSONSCREEN_SCALEFACTOR,
            //                    EPSONSCREEN_WIDTH*EPSONSCREEN_SCALEFACTOR,y_counter) );
        }//while(rx)

        // done receiving epson esc sequence image
        if ( TRUE==dataStart ) { bGotScreenshot = TRUE; }
    }

    txtSerialTrace->AppendText(wxString::Format("debug: received %ld bytes in total.\r\n", RxTotalBytecount));

    // -- image done, restore old baud
    if ( oldbaud < 19200 ) {
        txtSerialTrace->AppendText("Restoring old baudrate on Fluke...\r\n");
        statusBar->SetStatusText("screenshot: restoring baudrate",0);
        response = QueryFluke(strOldBaud,TRUE,1000,&respOk); // <ack><cr>
        if ( FALSE==respOk ) {
            txtSerialTrace->AppendText("Couldn't set Fluke baudrate.\r\n");
        } else {
            mySerial->setBaudrate(oldbaud);
        }
    }

    // -- show the final result of the operation
    if ( TRUE==gotImage ) {
        statusBar->SetStatusText("screenshot: complete image received",0);
        txtSerialTrace->AppendText("Screenshot complete.\r\n");
    } else {
        if ( FALSE==imageStart ) {
            txtSerialTrace->AppendText("Screenshot error: the response did not contain any image\r\n");
            statusBar->SetStatusText("screenshot: no image received",0);
        } else {
            txtSerialTrace->AppendText("Screenshot error: ran out of data before image end\r\n");
            statusBar->SetStatusText("screenshot: incomplete image received",0);
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
    if ( FALSE==bGotScreenshot ) {
        statusBar->SetStatusText("save image: error, no screenshot available!",0);
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
    if ( FALSE==this->imgScreenshot->SaveFile (filePath,wxBITMAP_TYPE_BMP ) ) {
        statusBar->SetStatusText("save BMP: error during saving!",0);
    } else {
        statusBar->SetStatusText("save BMP: done.",0);
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
    if ( FALSE==bGotScreenshot ) {
        statusBar->SetStatusText("copy to clipbrd: error, no screenshot available!",0);
        return;
    }
    // try to place bitmap on the clipboard
    if( TRUE==wxTheClipboard->Open() ) {
        wxTheClipboard->SetData ( new wxBitmapDataObject(sbmpScreenshot->GetBitmap()) );
        wxTheClipboard->Flush(); // keeps image on clipbrd even after this app exits
        wxTheClipboard->Close();
        statusBar->SetStatusText("copy to clipbrd: done.",0);
    } else {
        statusBar->SetStatusText("copy to clipbrd: error, couldn't open clipboard!",0);
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
        statusBar->SetStatusText("save postscript: error, no screenshot available!",0);
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
    if ( TRUE==outputFile.IsOpened() ) {
        outputFile.Seek(0, wxFromStart);
        if ( TRUE==outputFile.Write(this->strPostscript,wxConvLibc) ) {
            statusBar->SetStatusText("save postscript: done",0);
        } else {
            statusBar->SetStatusText("save postscript: error, file write failed",0);
        }
        outputFile.Flush();
        outputFile.Close();
    } else {
        statusBar->SetStatusText("save postscript: error, couldn't create file!",0);
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
    BOOL        respOk=FALSE, gotFullWF=FALSE;
    wxString    matlabStr="", csvStr="", str="", preStr="";
    double      x_offset=0, y_offset=0, delta_x=0, delta_y=0;
    wxString    strXUnit="", strYUnit="";
    long        wavlen=0, idx;
    unsigned char wbyte=0;

    // need to be connected first
    if ( FALSE==bFlukeDetected ) {
        statusBar->SetStatusText("waveforms: no ScopeMeter detected",0);
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
    
    gotFullWF = FALSE;
    response = QueryFluke(query,FALSE,1000,&respOk); // <ack><cr>
    if ( !respOk ) {
        
        txtSerialTrace->AppendText("Waveform: query failed.\r\n");
        statusBar->SetStatusText("waveforms: download request failed",0);
        
    } else {

        statusBar->SetStatusText("waveforms: downloading",0);
        response = GetFlukeResponse(5000); // <binary data string>
        if ( response.Length()<=0 ) {
            txtSerialTrace->AppendText("Waveform: Timed out waiting for data.\r\n");
            gotFullWF = FALSE;
        } else {        
            statusBar->SetStatusText("waveforms: got waveform",0);
            txtSerialTrace->AppendText("Waveform data:" + response + "\r\n");        
            gotFullWF = TRUE;            
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
            // TODO: add decoding
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
        }
        
        // -- store the Matlab code snippet in the textbox and on the clipboard
        txtWavestring->SetValue(matlabStr);
        if( TRUE==wxTheClipboard->Open() ) {
            wxTheClipboard->SetData ( new wxTextDataObject(matlabStr) );
            wxTheClipboard->Flush();
            wxTheClipboard->Close();
            statusBar->SetStatusText("waveforms: got waveform - Matlab code on clipboard",0);
        }

        // -- save a .CSV file
        if ( strPrevSavePath.Length()<1 ) {
            strPrevSavePath = wxGetCwd(); // default to current directory
        }
        // select a new file
        wxFileDialog* saveDialog = new wxFileDialog (
            (wxWindow *)this, "Save waveform data to CSV text file",
            strPrevSavePath, "", "Character Separated Values file (*.csv)|*.csv",
            wxSAVE|wxOVERWRITE_PROMPT
        );
        if ( wxID_CANCEL==saveDialog->ShowModal() ) { return; }
        str = saveDialog->GetPath();
        saveDialog->Destroy();
        // write info and waveform string to CSV file
        wxFile *outfile = new wxFile(str.c_str(), wxFile::write);
        if ( (NULL!=outfile) && (TRUE==outfile->IsOpened()) ) {
            wxDateTime now = wxDateTime::Now();
            wxString modelStr = this->strScopemeterID;
            modelStr.Replace(";"," ",TRUE);
            wxString infoStr = "## Fluke scopemeter waveform capture - generated with "
                + wxString(SG32_VERSION_STR) + "\r\n"
                + "## wave downloaded " + now.FormatISODate() + " " + now.FormatISOTime() + "\r\n"
                + "## SM model: " +  modelStr + "\r\n"
                + "## data in decimal notation (use find/replace . to , if necessary)\r\n"
                + "\r\n";
            if ( !outfile->Write(infoStr) || !outfile->Write(csvStr) ) {
                txtSerialTrace->AppendText("Waveform: could not write data to CSV file!\r\n");
            } else {
                txtSerialTrace->AppendText("Waveform: stored to CSV file\r\n");
                statusBar->SetStatusText("waveforms: Matlab code on clipboard, data in CSV file",0);
            }
            outfile->Close();
        }
        // store the path name for the next Save File dialog
        strPrevSavePath = str.BeforeLast('\\');
        
    }

    GUI_Up();
    
    return;    
}

//
// Menu: display the About dialog
//
void MyFrame::OnMenuAbout(wxMenuEvent& event)
{
    wxMessageBox(wxString(SG32_VERSION_STR)+"\r\n\rAn open source tool for communicating\r\n"
        "with a Fluke Scopemeter and also do a\r\nscreen capture.\r\n\r\n"
        "Supported models: 190 and 190C series\r\n\r\n"
        "Alpha 'support': 123,124,91,92,96 and 97,99\r\n\r\n"
        "(C) 2005 Jan Wagner",
        "About ScopeGrab32",
        wxOK | wxICON_INFORMATION, this);
    return;
}


//
// Menu: exit the program
//
void MyFrame::OnMenuExit(wxMenuEvent& event)
{
    mySerial->closePort();
    Close(); // exit the app
}



// ----------------------------------------------------
// -------------    SERIAL PORT FUNCS     -------------
// ----------------------------------------------------


//
// Timer event: pulse the RTS pin to generate
// Scopemeter DIY cable / circuit supply voltages
//
void MyFrame::evtRtsTimer(wxTimerEvent& event)
{
    // safety checks
    if ( NULL==mySerial ) return;
    if ( FALSE==mySerial->isOpen() ) {
        tmrToggleRTS->Stop();
        return;
    }
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
   bRxReceiverActive = TRUE;

   // keep statistics
   RxTotalBytecount += rxbytes;
   
   //wxCriticalSectionLocker buflock(csRxBuf);
   
   BYTE currByte;
   for ( DWORD i=0; i<rxbytes; ++i ) {

        currByte = *(rxbuf+i);

        // -- handle characters in ASCII mode
        if ( TRUE==this->bRxAsciiMode ) {

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
                    if ( TRUE==this->bRxBinarymodeAfterACK ) {
                        this->bRxBinarymodeAfterACK = FALSE;
                        this->bRxAsciiMode = FALSE;
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
    BOOL respIsAscii = FALSE; // flag, True=print response as text to txtSerialTrace
 
    // limits etc checks
    if ( msTimeout<200 || msTimeout>3000 ) { msTimeout=1000; }
    if ( NULL==mySerial || FALSE==mySerial->isOpen() ) { return wxString(""); }

    // return a response that was received earlier? (ascii ack, or other ascii lines)
    size_t itemCount = ReceivedStrings.GetCount();
    if ( itemCount>0 ) {

        response = ReceivedStrings.Item(itemCount-1); // get last
        ReceivedStrings.Remove(itemCount-1, 1); // remove from end
        respIsAscii = TRUE;

    // wait for new response data to come in...
    } else {

        itemCount = 0;
        cnt = 1+(msTimeout/200);
        this->bRxReceiverActive = FALSE; // flag used to detect rx activity
        while ( cnt>0 ) {
            // wait
            SleepEx(200,FALSE);
            DoEvents();
            // check receiver activity - did OnCommEvent() set the flag?
            if ( TRUE==this->bRxReceiverActive ) {
                this->bRxReceiverActive = FALSE;
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
        if ( FALSE==this->bRxAsciiMode ) {

            // binary mode, if previous ASCII ack was received
            // then return it first!
            if ( itemCount>0 ) {
                response = ReceivedStrings.Item(itemCount-1); // get last
                ReceivedStrings.Remove(itemCount-1, 1); // remove from end
                respIsAscii = TRUE;
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
            ReceivedStrings.Remove(itemCount-1, 1); // remove from end
            respIsAscii = TRUE;
            
        }//if(bin/ascii)

    }//if(existing rx data)

    // do a bit of ascii cleaned-up
    if ( TRUE==respIsAscii ) {
        response.Replace("\r", "", TRUE);
        response.Replace("\n", "", TRUE);
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
wxString MyFrame::QueryFluke(wxString cmdString, BOOL bAsciiMode,
    DWORD msTimeout, BOOL* ResponseIsOK)
{
    wxString response;
    
    // clean up the command string
    cmdString = (cmdString.Trim(TRUE)).Trim(FALSE);
    cmdString.Replace("\r", " ", TRUE);
    cmdString.Replace("\t", " ", TRUE);
    cmdString.Replace("\n", " ", TRUE);
    while ( cmdString.Replace("  ", " ", TRUE) > 0);
    cmdString = cmdString.MakeUpper();
    // add cmd terminator char (carriage return)
    cmdString.Append("\r");
    
    // clear current rx buffer and reset settings to defaults
    this->RxErrorCounter = 0;
    this->CurrRxString = "";
    this->ReceivedStrings.Empty();
    this->bRxReceiverActive = FALSE;
    this->bRxAsciiMode = TRUE;
    this->bRxBinarymodeAfterACK = FALSE; // full ascii rx

    // modify flag for response mode ascii+bin (ascii <ack> followed by binary)
    // the mode will be switched later in OnCommEvent
    if ( FALSE == bAsciiMode ) { this->bRxBinarymodeAfterACK = TRUE; }

    // send the command
    txtSerialTrace->AppendText("Query: " + cmdString + "\n");
    wxCharBuffer dataToSend = cmdString.mb_str();
    if ( FALSE == mySerial->transmitData(
            (const BYTE*)((const char*)dataToSend), cmdString.Length())
        )
    {
       txtSerialTrace->AppendText(
         wxString::Format("Query Error: transmitData failed, win32 error 0x%04lX\r\n",
            mySerial->getLastError()) );
       if ( NULL!=ResponseIsOK ) *ResponseIsOK = FALSE;
       return "";
    }
    
    // get the <ack> response (ASCII always, even when bAsciiMode==FALSE)
    response = GetFlukeResponse(msTimeout);
    response.Replace("\r", "", TRUE);
    response.Replace("\n", "", TRUE);
        
    // handle the <ack> response
    if ( NULL!=ResponseIsOK ) {
        *ResponseIsOK = FALSE;
    }
    if ( RxErrorCounter>0 ) {
       txtSerialTrace->AppendText(wxString::Format("Response ACK: encountered %ld errors\r\n", RxErrorCounter));
    }
    if ( 0 == response.Length() ) {

        txtSerialTrace->AppendText("Response ACK timeout\r\n");

    } else if ( 1 == response.Length() ) {
        char ackCode = response.GetChar(0);
        switch(ackCode) {
         case '0': if ( NULL!=ResponseIsOK ) { *ResponseIsOK = TRUE; }
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

